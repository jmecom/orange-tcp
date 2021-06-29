#pragma once

#include <vector>

#include "net.h"
#include "socket.h"

#include "absl/status/status.h"

namespace orange_tcp {

const uint16_net kEtherTypeArp = uint16_net(0x0806);
const uint16_net kEtherTypeIp = uint16_net(0x0800);
constexpr int kEthernetMtu = 1500;

struct EthernetHeader {
  MacAddr dst_mac;
  MacAddr src_mac;
  uint16_net ether_type;
} __attribute__((packed));

absl::Status SendEthernetFrame(Socket *socket,
  const MacAddr &src, const MacAddr &dst,
  void *payload, size_t payload_size,
  uint16_net ether_type = kEtherTypeIp);

// absl::status RecvEthernetFrame(Socket *socket,
  // const MacAddr &src, const MacAddr &dst, std::vector data);

// inline void DumpEthernetFrame(EthernetFrame *frame,
//                               size_t size = kEthernetMtu) {
//   printf("eth: %s -> %s (%x)", frame->dst_mac.ToString().c_str(),
//     frame->src_mac.ToString().c_str(), uint16_t(frame->ether_type));
//   DumpHex(frame->data, size);
// }

}  // namespace orange_tcp
