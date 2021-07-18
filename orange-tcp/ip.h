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
  uint8_t version : 4;
  uint8_t header_len : 4;
  uint8_t tos;
  uint16_net total_len;
  uint16_net frag_id;
  uint16_t frag_flags : 3;
  uint16_t frag_offset : 13;
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
      "  checksum: %d\n"
      "  src: %s\n"
      "  dst: %s\n",
      version, header_len, tos, total_len,
      frag_id, frag_flags, frag_offset, ttl,
      proto, checksum, src.str(), dst.str());
    // clang-format on
  }
} __attribute__((packed));

struct Datagram {
  Ipv4Header hdr;
  uint8_t *data;
} __attribute__((packed));

absl::Status SendDatagram(Socket *socket, IpAddr dst,
  uint8_t *data, size_t size);

void DumpDatagram(Datagram *datagram);

}  // namespace ip
}  // namespace orange_tcp
