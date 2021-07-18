#include "ip.h"

#include "arp.h"
#include "eth.h"
#include "serializable_map.h"

#include <utility>
#include <fstream>

#include "absl/flags/flag.h"

ABSL_FLAG(bool, dump_ip, false, "Set to log IP debugging information.");
ABSL_FLAG(std::string, routing_table, "/tmp/routing_table",
  "Routing table file location.");

namespace orange_tcp {
namespace ip {

constexpr uint8_t kIpv4 = 4;
constexpr uint8_t kHeaderLen = 5;
constexpr uint8_t kTos = 0;
constexpr uint8_t kTtl = 64;
constexpr uint8_t kProtoUdp = 16;
constexpr uint8_t kProtoTcp = 6;
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

void DumpDatagram(Datagram *datagram) {
  printf("[ip] %s  ", datagram->hdr.str().c_str());
  auto len = static_cast<int>(datagram->hdr.total_len) - sizeof(Ipv4Header);
  DumpHex(datagram->data, len);
}

// https://datatracker.ietf.org/doc/html/rfc1071
uint16_t Checksum(void *buffer, int count) {
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

  return ~sum;
}

// Construct an IP datagram. The returned `Datagram` does not own `data`.
// `data` must remain a valid pointer for the lifetime of the datagram.
Datagram MakeDatagram(IpAddr src, IpAddr dst,
                      uint8_t *data, size_t size) {
  uint16_net total_len = uint16_net(sizeof(Ipv4Header) + size);
  // TODO(jmecom) Check these defaults.
  Ipv4Header hdr = Ipv4Header(kIpv4, kHeaderLen, kTos,
    total_len, uint16_net(0), 0, 0, kTtl, kProtoUdp, uint16_net(0), src, dst);
  uint16_net checksum = uint16_net(Checksum(&hdr, sizeof(hdr)));
  hdr.checksum = checksum;
  return Datagram(hdr, data);
}

absl::Status SendDatagram(Socket *socket, IpAddr dst,
                          uint8_t *data, size_t size) {
  MaybeLoadRoutingTable();

  auto mac_ip_status = socket->GetHostMacAndIp();
  std::pair<MacAddr, IpAddr> pair = mac_ip_status.value();
  auto src_mac = pair.first;
  auto src_ip = pair.second;

  auto routed_dst = dst;

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

  auto datagram = MakeDatagram(src_ip, dst, data, size);

  if (absl::GetFlag(FLAGS_dump_ip)) {
    DumpDatagram(&datagram);
  }

  return SendEthernetFrame(socket, src_mac, dst_mac, &datagram,
    static_cast<int>(datagram.hdr.total_len));
}

}  // namespace ip
}  // namespace orange_tcp
