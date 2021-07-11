#include "ip.h"

#include "arp.h"
#include "eth.h"

#include <utility>

#include "absl/flags/flag.h"

ABSL_FLAG(bool, dump_ip, false, "Set to log IP debugging information.");

namespace orange_tcp {
namespace ip {

constexpr uint8_t kIpv4 = 4;
constexpr uint8_t kHeaderLen = 5;
constexpr uint8_t kTos = 0;
constexpr uint8_t kTtl = 64;
constexpr uint8_t kProtoUdp = 16;
constexpr uint8_t kProtoTcp = 6;

void DumpDatagram(Datagram *datagram) {
  printf("[ip] %s  ", datagram->hdr.ToString().c_str());
  DumpHex(datagram->data, datagram->hdr.total_len - sizeof(Ipv4Header));
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

Datagram MakeDatagram(IpAddr src, IpAddr dst, std::vector<uint8_t> data) {
  uint16_net total_len = uint16_net(sizeof(Datagram) + data.size());
  // TODO(jmecom) Check these defaults.
  Ipv4Header hdr = Ipv4Header(kIpv4, kHeaderLen, kTos,
    total_len, uint16_net(0), 0, 0, kTtl, kProtoUdp, uint16_net(0), src, dst);
  uint16_net checksum = uint16_net(Checksum(&hdr, sizeof(hdr)));
  hdr.checksum = checksum;
  return Datagram(hdr, data.data());
}

absl::Status SendDatagram(Socket *socket, IpAddr dst,
                          std::vector<uint8_t> data) {
  std::pair<MacAddr, IpAddr> p = socket->GetHostMacAndIpOrDie();
  auto src_mac = p.first;
  auto src_ip = p.second;

  printf("1\n");

  // TODO(jmecom) Two issues.
  // 1) This doesn't have a timeout, and it really shouldn't be blocking.
  // 2) Do I need to consult an IP routing table? Of course ARP requests
  //    to e.g. 1.1.1.1 will fail. Should the 'dst' actually be like,
  //    my local router?
  MacAddr dst_mac = arp::GetMacOrDie(socket, dst);
  printf("2\n");

  auto datagram = MakeDatagram(src_ip, dst, data);
  printf("3\n");

  if (absl::GetFlag(FLAGS_dump_ip)) {
    DumpDatagram(&datagram);
  }

  printf("4\n");
  return SendEthernetFrame(socket, src_mac, dst_mac, &datagram,
    datagram.hdr.total_len);
}

}  // namespace ip
}  // namespace orange_tcp
