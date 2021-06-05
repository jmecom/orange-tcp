#include "net.h"

#include <stdint.h>
#include <map>

#include "absl/status/status.h"

namespace orange_tcp {

// Only IEEE 802.3 Ethernet is supported.
constexpr int kEthernetHwType = 6;

// Only IP is supported.
constexpr int kIpProtocolType = 2048;

class Arp {
 public:
  absl::Status Request(const uint8_t(&ip_addr)[kIpAddrLen],
                       uint8_t(&mac_addr)[kMacAddrLen]);
  absl::Status Response();

 private:
  struct Packet {
    uint16_t hw_type;
    uint16_t p_type;
    uint8_t hw_addr_len;
    uint8_t p_len;
    uint16_t opcode;
    uint8_t src_hw_addr[kMacAddrLen];
    uint8_t src_ip_addr[kIpAddrLen];
    uint8_t dst_hw_addr[kMacAddrLen];
    uint8_t dst_ip_addr[kIpAddrLen];
  } __attribute__((packed));

  enum Opcodes {
    kArpRequest = 1,
    kArpResponse = 2,
  };

  std::map<uint8_t[kIpAddrLen], uint8_t[kMacAddrLen]> cache_;
};

}  // namespace orange_tcp
