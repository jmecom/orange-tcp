#include "socket.h"
#include <memory>

#include "net.h"
#include "eth.h"

#include "absl/strings/str_format.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

ABSL_FLAG(bool, server, false, "True -> server, false -> client");

namespace orange_tcp {
namespace eth_echo {

static const MacAddr kServerMac = {
  .addr = {0x02, 0x42, 0xac, 0x11, 0x00, 0x03}
};
static const MacAddr kClientMac = {
  .addr = {0x02, 0x42, 0xac, 0x11, 0x00, 0x02}
};

int Server() {
  auto socket = PosixSocket::CreateOrDie();

  std::vector<uint8_t> payload;
  size_t payload_size = 32;

  for (;;) {
    auto status = RecvEthernetFrame(socket.get(), &payload, payload_size);
    if (!status.ok()) {
      puts(absl::StrFormat("[eth_echo] Err: %s",
        status.message()).c_str());
    }

    memset(payload.data(), 0xde, payload.size());

    status = SendEthernetFrame(socket.get(),
      kServerMac, kClientMac, payload.data(), payload.size(),
      kEtherTypeArp);

    usleep(100);
  }
  return 0;
}

int Client() {
  auto socket = PosixSocket::CreateOrDie();

  constexpr int kPayloadSize = 32;
  uint8_t payload[kPayloadSize];
  memset(payload, 0xab, sizeof(payload));
  std::vector<uint8_t> received_payload;

  auto status = SendEthernetFrame(socket.get(), kClientMac, kBroadcastMac,
    payload, sizeof(payload), kEtherTypeArp);

  if (!status.ok()) {
    printf("[eth_echo] %s\n", absl::StrFormat("Request failed: %s",
      status.message()).c_str());
    return -1;
  }

  status = RecvEthernetFrame(socket.get(), &received_payload, kPayloadSize);
  if (!status.ok()) {
    puts(absl::StrFormat("[eth_echo] Err: %s",
      status.message()).c_str());
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

}  // namespace eth_echo
}  // namespace orange_tcp

int main(int argc, char **argv) {
  absl::SetProgramUsageMessage("Ethernet echo test");
  absl::ParseCommandLine(argc, argv);
  return orange_tcp::eth_echo::Main();
}
