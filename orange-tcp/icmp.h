#pragma once

#include "net.h"
#include "ip.h"

namespace orange_tcp {
namespace icmp {

struct IcmpHeader {
  uint8_t type;
  uint8_t code;
  uint16_net checksum;
} __attribute__((packed));

// ICMP Control Messages

struct EchoRequest {
  IcmpHeader hdr;
  uint16_net id;
  uint16_net sequence;
};

}  // namespace icmp
}  // namespace orange_tcp
