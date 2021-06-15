#include "socket.h"
#include "arp.h"

#include <memory>

#include "absl/strings/str_format.h"

namespace orange_tcp {
namespace arping {

int Main() {
  IpAddr ip = {};
  MacAddr mac = {};

  auto socket_result = PosixSocket::Create();
  if (!socket_result.ok()) {
    printf("Failed to create socket\n");
    return -1;
  }
  std::unique_ptr<Socket> socket = std::move(socket_result.value());

  auto result = arp::Request(socket.get(), ip, mac);
  if (!result.ok()) {
    printf("%s\n", absl::StrFormat("Arp request failed: %s",
      result.message()).c_str());
    return -1;
  }

  printf("done\n");
  return 0;
}

}  // namespace arping
}  // namespace orange_tcp

int main(void) {
  return orange_tcp::arping::Main();
}
