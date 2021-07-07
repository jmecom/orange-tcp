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

int Client() {
  std::string ip = absl::GetFlag(FLAGS_ip);
  if (ip.empty()) {
    printf("[arping] must set ip address\n");
    return -1;
  }

  auto socket = RawSocket::CreateOrDie();
  auto result = arp::Request(socket.get(), IpAddr::FromString(ip));
  if (!result.ok()) {
    printf("[arping] %s\n", absl::StrFormat("ARP request failed: %s",
      result.message()).c_str());
    return -1;
  }

  auto mac_result = arp::HandleResponse(socket.get());
  if (!mac_result.ok()) {
    printf("[arping] Failed to handle ARP response (%s)\n",
      mac_result.status().message().data());
    return -1;
  }

  printf("[arping] done, got %s\n",
    mac_result.value().ToString().c_str());
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
