#include "socket.h"
#include <memory>

#include "ip.h"
#include "icmp.h"

#include "absl/strings/str_format.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

ABSL_FLAG(bool, server, false, "True -> server, false -> client");
ABSL_FLAG(std::string, ip, "",
  "Destination IP address (numbers-and-dots notation)");
ABSL_FLAG(int, num_requests, 1,
  "How many times to issue the same request");

namespace orange_tcp {
namespace ping {

int Server() {
  auto socket = RawSocket::CreateOrDie();

  for (;;) {
  }

  return 0;
}

int Ping(Socket *socket, const IpAddr& ip) {
  // TODO(jmecom) This is wrong
  std::vector<uint8_t> empty;
  auto status = ip::SendDatagram(socket, ip, empty);
  if (!status.ok()) {
    printf("[ping] Failed to send");
    return 1;
  }

  return 0;
}

int Client() {
  std::string ip_str = absl::GetFlag(FLAGS_ip);
  if (ip_str.empty()) {
    printf("[ping] must set ip address\n");
    return -1;
  }
  const IpAddr ip = IpAddr::FromString(ip_str);
  auto socket = RawSocket::CreateOrDie();

  int ret = 0;
  for (int i = 0; i < absl::GetFlag(FLAGS_num_requests); i++) {
    ret = Ping(socket.get(), ip);
    if (ret != 0) return ret;
    usleep(100);
  }

  return 0;
}

int Main() {
  if (absl::GetFlag(FLAGS_server)) {
    return Server();
  } else {
    return Client();
  }
}

}  // namespace ping
}  // namespace orange_tcp

int main(int argc, char **argv) {
  absl::SetProgramUsageMessage("Ethernet echo test");
  absl::ParseCommandLine(argc, argv);
  return orange_tcp::ping::Main();
}
