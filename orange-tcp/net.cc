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

}  // namespace orange_tcp
