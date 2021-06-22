#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <memory>
#include <vector>
#include <string>
#include <cstring>

#include "netint.h"

#include "absl/strings/str_format.h"

namespace orange_tcp {

constexpr int kMacAddrLen = 6;
constexpr int kIpAddrLen = 4;

struct MacAddr {
  uint8_t addr[kMacAddrLen];

  bool operator==(const MacAddr& other) const {
    return (memcmp(addr, other.addr, kMacAddrLen) == 0);
  }
  bool operator<(const MacAddr& other) const {
    return (memcmp(addr, other.addr, kMacAddrLen) < 0);
  }

  std::string ToString() {
    return absl::StrFormat("%x:%x:%x:%x:%x:%x",
      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  }
};

struct IpAddr {
  uint8_t addr[kIpAddrLen];

  bool operator==(const IpAddr& other) {
    return (memcmp(addr, other.addr, kIpAddrLen) == 0);
  }

  std::string ToString() {
    return absl::StrFormat("%x:%x:%x:%x",
      addr[0], addr[1], addr[2], addr[3]);
  }
};

const MacAddr kBroadcastMac = {
  .addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

const IpAddr kBroadcastIp = {
  .addr = {0xFF, 0xFF, 0xFF, 0xFF}
};

const uint16_net kEtherTypeArp = uint16_net(0x0806);
const uint16_net kEtherTypeIp = uint16_net(0x0800);
constexpr int kEthernetMaxMtu = 1500;

struct EthernetFrame {
  MacAddr dst_mac;
  MacAddr src_mac;
  uint16_net ether_type;
  uint8_t data[kEthernetMaxMtu];
  // TODO(jmecom) Checksum?
} __attribute__((packed));

struct Address {
  Address(IpAddr addr, int port) :
    addr(addr), port(port) {}
  IpAddr addr;
  int port;
};

inline void DumpHex(uint8_t *buffer, size_t size) {
  for (size_t i = 0; i < size; i++) printf("%02x", buffer[i]);
  printf("\n");
}


}  // namespace orange_tcp
