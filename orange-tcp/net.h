#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <algorithm>
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

  std::string str() const {
    return absl::StrFormat("%02x:%02x:%02x:%02x:%02x:%02x",
      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  }

  operator std::string() const {
    return this->str();
  }

  static MacAddr FromString(std::string mac) {
    MacAddr m;
    sscanf(mac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
      &m.addr[0], &m.addr[1], &m.addr[2],
      &m.addr[3], &m.addr[4], &m.addr[5]);
    return m;
  }
};

struct IpAddr {
  uint32_t addr;

  bool operator==(const IpAddr& other) const {
    return addr == other.addr;
  }

  bool operator<(const IpAddr& other) const {
    return addr < other.addr;
  }

  std::string str() const {
    return std::string(inet_ntoa(static_cast<in_addr>(addr)));
  }

  operator std::string() const {
    return this->str();
  }

  static IpAddr FromString(std::string ip) {
    IpAddr addr;
    addr.addr = inet_addr(ip.c_str());
    return addr;
  }
};

const MacAddr kBroadcastMac = {
  .addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

const IpAddr kBroadcastIp = {
  .addr = 0xFFFFFFFF,
};

struct Address {
  Address(IpAddr addr, int port) :
    addr(addr), port(port) {}
  IpAddr addr;
  int port;
};

inline void DumpHex(uint8_t *buffer, size_t size) {
  for (size_t i = 0; i < size; i++) {
    printf("%02x", buffer[i]);
  }
  printf("\n");
}

}  // namespace orange_tcp
