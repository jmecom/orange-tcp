#pragma once

#include "net.h"
#include "socket.h"

#include <vector>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"

namespace orange_tcp {
namespace ip {

struct Ipv4Header {
  uint8_t version_and_header_len;
  uint8_t tos;
  uint16_net total_len;
  uint16_net frag_id;
  uint16_t frag_flags_and_offset;
  uint8_t ttl;
  uint8_t proto;
  uint16_t checksum;
  IpAddr src;
  IpAddr dst;
} __attribute__((packed));

enum Protocol : uint8_t {
  icmp = 1,
  tcp = 6,
  udp = 17,
};

uint16_t Checksum(void *buffer, int count);

absl::Status SendDatagram(Socket *socket, IpAddr dst,
  uint8_t *data, size_t size,
  Protocol proto = Protocol::udp);

}  // namespace ip
}  // namespace orange_tcp
