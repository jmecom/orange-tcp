#include "icmp.h"

namespace orange_tcp {
namespace icmp {

EchoRequest MakeEchoRequest() {
  static int seq = 0;

  EchoRequest out;
  out.hdr.type = 8;
  out.hdr.code = 0;
  out.hdr.checksum = 0;
  // out.hdr.checksum = ip::Checksum(&out.hdr, sizeof(out.hdr));
  out.id = uint16_net(0x1337);
  out.sequence = seq++;
  out.hdr.checksum = ip::Checksum(&out, sizeof(out));
  printf("0x%04x\n", (uint16_t)out.hdr.checksum);
  printf("0x%04x\n", uint16_net(out.hdr.checksum));

  return out;
}

}  // namespace icmp
}  // namespace orange_tcp
