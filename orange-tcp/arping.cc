#include "socket.h"
#include "arp.h"

#include "absl/strings/str_format.h"

namespace orange_tcp {
namespace arping {

int Main() {
  IpAddr ip = {};
  MacAddr mac = {};

  auto result = arp::Request(ip, mac);
  if (!result.ok()) {
    printf("%s\n", absl::StrFormat("Arp request failed: %s",
      result.message()).c_str());
    return -1;
  }

  return 0;
}

}  // namespace arping
}  // namespace orange_tcp

int main(void) {
  return orange_tcp::arping::Main();
}
