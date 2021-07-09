#include "socket.h"
#include "arp.h"

#include <memory>

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
namespace arping {

int Server() {
  auto socket = RawSocket::CreateOrDie();
  for (;;) {
    auto status = arp::HandleRequest(socket.get());
    if (!status.ok()) {
      puts(absl::StrFormat("[arping] Err: %s", status.message()).c_str());
    }
    printf("\n");
    usleep(100);
  }
  return 0;
}

int ArpRoundTrip(Socket *socket, const IpAddr& ip) {
  MacAddr dst_mac;
  auto result = arp::Request(socket, ip, &dst_mac);
  if (absl::IsAlreadyExists(result)) {
    printf("[arping] done, got %s (cached)\n",
      dst_mac.ToString().c_str());
    return 0;
  }

  if (!result.ok()) {
    printf("[arping] %s\n", absl::StrFormat("ARP request failed: %s",
      result.message()).c_str());
    return -1;
  }

  result = arp::HandleResponse(socket, &dst_mac);
  if (!result.ok()) {
    printf("[arping] Failed to handle ARP response (%s)\n",
      result.message().data());
    return -1;
  }

  printf("[arping] done, got %s\n", dst_mac.ToString().c_str());

  return 0;
}

int Client() {
  std::string ip_str = absl::GetFlag(FLAGS_ip);
  if (ip_str.empty()) {
    printf("[arping] must set ip address\n");
    return -1;
  }
  const IpAddr ip = IpAddr::FromString(ip_str);
  auto socket = RawSocket::CreateOrDie();

  int ret = 0;
  for (int i = 0; i < absl::GetFlag(FLAGS_num_requests); i++) {
    ret = ArpRoundTrip(socket.get(), ip);
    if (ret != 0) return ret;
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

}  // namespace arping
}  // namespace orange_tcp

int main(int argc, char **argv) {
  absl::SetProgramUsageMessage("Simple ARP server/client");
  absl::ParseCommandLine(argc, argv);
  return orange_tcp::arping::Main();
}
