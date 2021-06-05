#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

namespace orange_tcp {

constexpr int kMacAddrLen = 6;
constexpr int kIpAddrLen = 4;

const uint8_t kBroadcastMac[kMacAddrLen] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

const uint8_t kBroadcastIp[kIpAddrLen] = {
  0xFF, 0xFF, 0xFF, 0xFF,
};

enum EtherType {
  kArp = 0x0806,
  kIp = 0x0800,
};

struct EthernetFrame {
  uint8_t dst_mac[kMacAddrLen];
  uint8_t src_mac[kMacAddrLen];
  uint16_t ether_type;
  uint8_t *data;
} __attribute__((packed));

struct Address {
  uint8_t ip_addr[kIpAddrLen];
  int port;

  void FillSockaddr(sockaddr_in *sai) {
    sai->sin_family = AF_INET;
    // TODO(jmecom) Check.
    uint32_t i = ip_addr[0] | (ip_addr[1] << 8) |
      (ip_addr[2] << 16) | (ip_addr[3] << 24);
    sai->sin_addr.s_addr = htonl(i);
    sai->sin_port = htons(port);
  }
};

}  // namespace orange_tcp
