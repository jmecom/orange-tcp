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
  uint16_net checksum;
  IpAddr src;
  IpAddr dst;

  std::string str() const {
    // clang-format off
    return absl::StrFormat(
      "ip_datagram:\n"
      "  version: %d\n"
      "  header_len: %d\n"
      "  tos: %d\n"
      "  total_len: %d\n"
      "  frag_id: %d\n"
      "  frag_flags: %d\n"
      "  frag_offset: %d\n"
      "  ttl: %d\n"
      "  proto: %d\n"
      "  checksum: 0x%02x\n"
      "  src: %s\n"
      "  dst: %s\n",
      (version_and_header_len & 0xf0) >> 4,
      version_and_header_len & 0x0f,
      tos, total_len,
      frag_id,
      (frag_flags_and_offset & 0b1110000000000000) >> 13,
      frag_flags_and_offset & 0b0001111111111111,
      ttl,
      proto, checksum, src.str(), dst.str());
    // clang-format on
  }
} __attribute__((packed));

enum Protocol: uint8_t {
  icmp = 1,
  tcp = 6,
  udp = 17,
};

uint16_net Checksum(void *buffer, int count);

absl::Status SendDatagram(Socket *socket, IpAddr dst,
  uint8_t *data, size_t size,
  Protocol proto = Protocol::udp);

}  // namespace ip
}  // namespace orange_tcp
