#pragma once

#include "net.h"
#include "ip.h"

namespace orange_tcp {
namespace icmp {

struct IcmpHeader {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
} __attribute__((packed));

// ICMP Control Messages

struct EchoBody {
  IcmpHeader hdr;
  uint16_net id;
  uint16_net sequence;
};

EchoBody MakeEchoRequest();

}  // namespace icmp
}  // namespace orange_tcp
