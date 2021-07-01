#pragma once

#include <vector>

#include "net.h"
#include "socket.h"

#include "absl/status/status.h"

namespace orange_tcp {

const uint16_net kEtherTypeArp = uint16_net(0x0806);
const uint16_net kEtherTypeIp = uint16_net(0x0800);

struct EthernetHeader {
  MacAddr dst_mac;
  MacAddr src_mac;
  uint16_net ether_type;
} __attribute__((packed));

struct EthernetFrame {
  EthernetHeader hdr;
  uint8_t *payload;
  uint32_t crc;
};

// Max and min payload sizes.
constexpr int kEthernetMax = 1500;
constexpr int kEthernetMin = 46;

constexpr int kCrcSize = 4;

constexpr int kEthernetOverhead = sizeof(EthernetHeader) + kCrcSize;

EthernetFrame *ToEthernetFrame(uint8_t *data, size_t size);

absl::Status SendEthernetFrame(Socket *socket,
  const MacAddr &src, const MacAddr &dst,
  void *payload, size_t payload_size,
  uint16_net ether_type = kEtherTypeIp);

absl::Status RecvEthernetFrame(Socket *socket,
  std::vector<uint8_t> *payload);

inline void DumpEthernetFrame(EthernetFrame *frame, size_t size) {
  printf("eth: %s -> %s (%x) (crc: 0x%x)  ",
    frame->hdr.dst_mac.ToString().c_str(),
    frame->hdr.src_mac.ToString().c_str(),
    uint16_t(frame->hdr.ether_type),
    frame->crc);
  DumpHex(frame->payload, size - kEthernetOverhead);
}

}  // namespace orange_tcp
