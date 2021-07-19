#include "icmp.h"

namespace orange_tcp {
namespace icmp {

EchoRequest MakeEchoRequest() {
  static int seq = 0;

  EchoRequest out;
  out.hdr.type = 8;
  out.hdr.code = 0;
  out.hdr.checksum = ip::Checksum(&out.hdr, sizeof(out.hdr));
  out.id = uint16_net(0x8181);
  out.sequence = seq++;

  return out;
}

}  // namespace icmp
}  // namespace orange_tcp
