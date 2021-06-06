#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include <memory>
#include <vector>

namespace orange_tcp {

constexpr int kMacAddrLen = 6;
constexpr int kIpAddrLen = 4;

enum EtherType : uint16_t {
  kArp = 0x0806,
  kIp = 0x0800,
};

constexpr int kEthernetMaxMtu = 1500;

struct MacAddr {
  uint8_t addr[kMacAddrLen];
};

struct IpAddr {
  uint8_t addr[kIpAddrLen];
};

const MacAddr kBroadcastMac = {
  .addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

const IpAddr kBroadcastIp = {
  .addr = {0xFF, 0xFF, 0xFF, 0xFF}
};

class EthernetFrame {
 public:
  EthernetFrame(
    const MacAddr &dst_mac,
    const MacAddr &src_mac,
    EtherType ether_type = EtherType::kIp,
    std::vector<uint8_t> data = std::vector<uint8_t>()) :
      dst_mac_(dst_mac),
      src_mac_(src_mac),
      ether_type_(ether_type),
      data_(data) {}

  EthernetFrame(
    const MacAddr &dst_mac,
    const MacAddr &src_mac,
    EtherType ether_type,
    uint8_t *data, size_t size) :
      dst_mac_(dst_mac),
      src_mac_(src_mac),
      ether_type_(ether_type),
      data_(std::vector<uint8_t>(data, data + size)) {}

  std::vector<uint8_t> Pack();

 private:
  MacAddr dst_mac_;
  MacAddr src_mac_;
  EtherType ether_type_;
  std::vector<uint8_t> data_;
};

struct Address {
  Address(IpAddr addr, int port) :
    addr(addr), port(port) {}
  IpAddr addr;
  int port;

  void FillSockaddr(sockaddr_in *sai);
};

}  // namespace orange_tcp
