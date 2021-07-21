#include "icmp.h"

namespace orange_tcp {
namespace icmp {

EchoBody MakeEchoRequest() {
  static int seq = 0;

  EchoBody out;
  out.hdr.type = 8;
  out.hdr.code = 0;
  out.hdr.checksum = 0;
  out.id = uint16_net(0x1337);
  out.sequence = seq++;
  out.hdr.checksum = ip::Checksum(&out, sizeof(out));

  return out;
}

}  // namespace icmp
}  // namespace orange_tcp
