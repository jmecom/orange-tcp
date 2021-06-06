#include "net.h"

#include <string.h>
#include <assert.h>

namespace orange_tcp {

std::vector<uint8_t> EthernetFrame::Pack() {
  constexpr int kEtherOverhead = kMacAddrLen * 2 + sizeof(uint16_t);
  std::vector<uint8_t> packed =
    std::vector<uint8_t>(kEtherOverhead + data_.size());

  assert(packed.size() < kEthernetMaxMtu);

  size_t off = 0;
  memcpy(packed.data(), dst_mac_.addr, kMacAddrLen);
  off += kMacAddrLen;
  memcpy(packed.data() + off, src_mac_.addr, kMacAddrLen);
  off += kMacAddrLen;
  auto et = htons(ether_type_);
  memcpy(packed.data() + off, &et, sizeof(uint16_t));
  off += sizeof(uint16_t);
  memcpy(packed.data() + off, data_.data(), data_.size());
  off += data_.size();

  assert(off == packed.size());

  return packed;
}

void Address::FillSockaddr(sockaddr_in *sai) {
  const uint8_t *ip = addr.addr;
  sai->sin_family = AF_INET;
  // TODO(jmecom) Check.
  uint32_t i = ip[0] | (ip[1] << 8) | (ip[2] << 16) | (ip[3] << 24);
  sai->sin_addr.s_addr = htonl(i);
  sai->sin_port = htons(port);
}

}  // namespace orange_tcp
