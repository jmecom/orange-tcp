#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <memory>
#include <vector>

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

constexpr int kEthernetMaxMtu = 1500;

// struct EthernetFrame {
//   uint8_t dst_mac[kMacAddrLen];
//   uint8_t src_mac[kMacAddrLen];
//   uint16_t ether_type;
//   std::vector<uint8_t> data;
// } __attribute__((packed));

void *PackEthernetFrame(
  const uint8_t(&dst_mac)[kMacAddrLen],
  const uint8_t(&src_mac)[kMacAddrLen],
  std::vector<uint8_t> data);

struct Address {
  uint8_t ip_addr[kIpAddrLen];
  int port;

  void FillSockaddr(sockaddr_in *sai);
};

}  // namespace orange_tcp
