#include "socket.h"
#include "arp.h"

#include <memory>

#include "absl/strings/str_format.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

ABSL_FLAG(bool, server, false, "True -> server, false -> client");

namespace orange_tcp {
namespace arping {

int Server() {
  auto socket = PosixSocket::CreateOrDie();
  for (;;) {
    auto status = arp::MaybeHandleResponse(socket.get());
    if (!status.ok()) {
      puts(absl::StrFormat("Err: %s", status.message()).c_str());
    }
    // received = socket->Recv(data, sizeof(data));
    // printf("Got %ld bytes:\n", received);
    // DumpHex(data, received);
    printf("\n");
    sleep(1);
  }
  return 0;
}

int Client() {
  auto socket = PosixSocket::CreateOrDie();
  auto result = arp::Request(socket.get());
  if (!result.ok()) {
    printf("%s\n", absl::StrFormat("Arp request failed: %s",
      result.message()).c_str());
    return -1;
  }

  printf("done\n");
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
