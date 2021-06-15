#pragma once

#include "net.h"
#include "socket.h"

#include <stdint.h>
#include <map>
#include <vector>

#include "absl/status/status.h"

namespace orange_tcp {
namespace arp {

// Only IEEE 802.3 Ethernet is supported.
const uint16_net kEthernetHwType = uint16_net(6);

// Only IP is supported.
const uint16_net kIpProtocolType = uint16_net(2048);

const uint16_net kArpRequest = uint16_net(1);
const uint16_net kArpResponse = uint16_net(2);

struct Packet {
  uint16_net hw_type;
  uint16_net p_type;
  uint8_t hw_addr_len;
  uint8_t p_len;
  uint16_net opcode;
  MacAddr src_hw_addr;
  IpAddr src_ip_addr;
  MacAddr dst_hw_addr;
  IpAddr dst_ip_addr;
} __attribute__((packed));

absl::Status Request(Socket *socket,
                     const IpAddr &ip, const MacAddr &mac);
absl::Status Response();

class Cache {
 private:
  std::map<IpAddr, MacAddr> cache_;
};

}  // namespace arp
}  // namespace orange_tcp
