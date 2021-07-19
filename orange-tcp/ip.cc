#include "ip.h"

#include "arp.h"
#include "eth.h"
#include "serializable_map.h"

#include <utility>
#include <fstream>

#include "absl/flags/flag.h"

ABSL_FLAG(std::string, routing_table, "/tmp/routing_table",
  "Routing table file location.");

namespace orange_tcp {
namespace ip {

constexpr uint8_t kIpv4AndDefaultHeaderLen = 0x45;
constexpr uint8_t kTos = 0;
constexpr uint8_t kTtl = 64;
static const IpAddr kDefaultDst = { .addr = 0 };

// Stores (destination, gateway) pairs.
// Uses 0.0.0.0 as 'default' for the destination, even though this is
// not standard. (https://en.wikipedia.org/wiki/0.0.0.0)
static serializable_map<IpAddr, IpAddr> g_routing_table;

void MaybeLoadRoutingTable() {
  static bool loaded = false;
  if (loaded) return;

  std::ifstream table(absl::GetFlag(FLAGS_routing_table));
  std::vector<char> buffer((std::istreambuf_iterator<char>(table)),
                            std::istreambuf_iterator<char>());

  g_routing_table.deserialize(buffer);

  loaded = true;
}

// https://datatracker.ietf.org/doc/html/rfc1071
uint16_net Checksum(void *buffer, int count) {
  uint32_t sum = 0;
  uint16_t *ptr = reinterpret_cast<uint16_t *>(buffer);

  while (count > 1) {
    sum += *ptr++;
    count -= 2;
  }

  if (count > 0)
    sum += *reinterpret_cast<uint8_t *>(ptr);

  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);

  sum = ~sum;
  return uint16_net(sum);
}

std::vector<uint8_t> MakeDatagram(IpAddr src, IpAddr dst,
                                  uint8_t *payload, size_t size,
                                  Protocol proto = Protocol::udp) {
  std::vector<uint8_t> datagram;
  uint16_net total_len = uint16_net(sizeof(Ipv4Header) + size);
  datagram.resize(total_len);

  Ipv4Header hdr = Ipv4Header(kIpv4AndDefaultHeaderLen, kTos,
    total_len, uint16_net(0), 0, kTtl, proto, uint16_net(0), src, dst);
  uint16_net checksum = Checksum(&hdr, sizeof(hdr));
  hdr.checksum = checksum;

  memcpy(&datagram[0], &hdr, sizeof(hdr));
  memcpy(&datagram[sizeof(hdr)], payload, size);

  return datagram;
}

absl::Status SendDatagram(Socket *socket, IpAddr dst,
                          uint8_t *data, size_t size,
                          Protocol proto) {
  MaybeLoadRoutingTable();

  auto mac_ip_status = socket->GetHostMacAndIp();
  std::pair<MacAddr, IpAddr> pair = mac_ip_status.value();
  MacAddr src_mac = pair.first;
  IpAddr src_ip = pair.second;

  IpAddr routed_dst = dst;

  if (g_routing_table.find(kDefaultDst) != g_routing_table.end()) {
    routed_dst = g_routing_table[kDefaultDst];
  } else if (g_routing_table.find(dst) != g_routing_table.end()) {
    routed_dst = g_routing_table[dst];
  }

  auto mac_status = arp::GetMac(socket, routed_dst);
  if (!mac_status.ok()) {
    return absl::InternalError("Failed to get MAC address");
  }
  MacAddr dst_mac = mac_status.value();

  auto datagram = MakeDatagram(src_ip, dst, data, size, proto);

  return SendEthernetFrame(socket, src_mac, dst_mac, datagram.data(),
    datagram.size());
}

}  // namespace ip
}  // namespace orange_tcp
