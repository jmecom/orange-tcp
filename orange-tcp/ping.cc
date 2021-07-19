#include "socket.h"
#include <memory>

#include "ip.h"
#include "icmp.h"

#include "absl/strings/str_format.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

ABSL_FLAG(std::string, ip, "",
  "Destination IP address (numbers-and-dots notation)");
ABSL_FLAG(int, num_requests, 1,
  "How many times to issue the same request");

namespace orange_tcp {
namespace ping {

int Ping(Socket *socket, const IpAddr& ip) {
  icmp::EchoRequest request = icmp::MakeEchoRequest();
  auto status = ip::SendDatagram(socket, ip,
    reinterpret_cast<uint8_t *>(&request), sizeof(request),
    ip::Protocol::icmp);
  if (!status.ok()) {
    printf("[ping] Failed to send");
    return 1;
  }

  return 0;
}

int Main() {
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

}  // namespace ping
}  // namespace orange_tcp

int main(int argc, char **argv) {
  absl::SetProgramUsageMessage("Issue ICMP echo requests");
  absl::ParseCommandLine(argc, argv);
  return orange_tcp::ping::Main();
}
