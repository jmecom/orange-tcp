#include "eth.h"
#include "absl/strings/str_format.h"

namespace orange_tcp {

absl::Status SendEthernetFrame(Socket *socket,
  const MacAddr &src, const MacAddr &dst,
  void *payload, size_t payload_size,
  uint16_net ether_type) {
  if (payload_size > kEthernetMtu) {
    return absl::InvalidArgumentError(
      absl::StrFormat("Size too big: %d > 1500", payload_size));
  }

  EthernetHeader header = EthernetHeader(dst, src, ether_type);

  std::vector<uint8_t> frame;
  frame.resize(sizeof(header) + payload_size);
  memcpy(&frame[0], &header, sizeof(header));
  memcpy(&frame[sizeof(header)], payload, payload_size);

  DumpHex(frame.data(), frame.size());
  if (socket->SendTo(static_cast<void *>(frame.data()),
                     frame.size(), dst) == -1) {
    return absl::InternalError(absl::StrFormat("Send failed ('%s')",
      strerror(errno)));
  }

  return absl::OkStatus();
}

}  // namespace orange_tcp
