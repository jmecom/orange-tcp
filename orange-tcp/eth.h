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

absl::Status SendEthernetFrame(Socket *socket,
  const MacAddr &src, const MacAddr &dst,
  void *payload, size_t payload_size,
  uint16_net ether_type = kEtherTypeIp);

absl::Status RecvEthernetFrame(Socket *socket,
  std::vector<uint8_t> *payload);

inline void DumpEthernetFrame(uint8_t *frame, size_t size) {
  EthernetHeader *hdr = reinterpret_cast<EthernetHeader *>(frame);
  uint8_t *payload = frame + sizeof(EthernetHeader);
  uint32_t crc = *(reinterpret_cast<uint32_t *>(frame + size - kCrcSize));

  printf("[eth] %s -> %s (%x) (crc: 0x%x)  ",
    hdr->src_mac.ToString().c_str(),
    hdr->dst_mac.ToString().c_str(),
    uint16_t(hdr->ether_type),
    crc);

  DumpHex(payload, size - kEthernetOverhead);
}

}  // namespace orange_tcp
