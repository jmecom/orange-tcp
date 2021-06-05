#include "net.h"

#include <string.h>

namespace orange_tcp {

std::unique_ptr<EthernetFrame> MakeEthernetFrame(
  const uint8_t(&dst_mac)[kMacAddrLen],
  const uint8_t(&src_mac)[kMacAddrLen],
  uint8_t *data, size_t data_len) {
  if (data_len > kEthernetMaxMtu) return nullptr;
  auto f = std::make_unique<EthernetFrame>();
  memcpy(f->dst_mac, dst_mac, kMacAddrLen);
  memcpy(f->src_mac, src_mac, kMacAddrLen);
  f->ether_type = EtherType::kIp;
  memcpy(f->data, data, data_len);
  return f;
}

void Address::FillSockaddr(sockaddr_in *sai) {
  sai->sin_family = AF_INET;
  // TODO(jmecom) Check.
  uint32_t i = ip_addr[0] | (ip_addr[1] << 8) |
    (ip_addr[2] << 16) | (ip_addr[3] << 24);
  sai->sin_addr.s_addr = htonl(i);
  sai->sin_port = htons(port);
}

}  // namespace orange_tcp