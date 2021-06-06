#include "net.h"

#include <stdint.h>
#include <map>
#include <vector>

#include "absl/status/status.h"

namespace orange_tcp {
namespace arp {

// Only IEEE 802.3 Ethernet is supported.
constexpr int kEthernetHwType = 6;

// Only IP is supported.
constexpr int kIpProtocolType = 2048;

enum Opcodes {
  kArpRequest = 1,
  kArpResponse = 2,
};

class Packet {
 public:
  uint16_t hw_type_;
  uint16_t p_type_;
  uint8_t hw_addr_len_;
  uint8_t p_len_;
  uint16_t opcode_;
  MacAddr src_hw_addr_;
  IpAddr src_ip_addr_;
  MacAddr dst_hw_addr_;
  IpAddr dst_ip_addr_;

  std::vector<uint8_t> Pack();
};

absl::Status Request(const IpAddr &ip, const MacAddr &mac);
absl::Status Response();

class Cache {
 private:
  std::map<IpAddr, MacAddr> cache_;
};

}  // namespace arp
}  // namespace orange_tcp
