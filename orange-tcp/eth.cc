#include "eth.h"

#include <algorithm>

#include "absl/strings/str_format.h"
#include "absl/flags/flag.h"

ABSL_FLAG(bool, dump_ethernet, false, "Set to log ethernet traffic.");

namespace orange_tcp {

// https://stackoverflow.com/questions/21001659/crc32-algorithm-implementation-in-c-without-a-look-up-table-and-with-a-public-li
uint32_t crc32(uint8_t *message, int length) {
  uint32_t byte, crc, mask;

  crc = 0xFFFFFFFF;
  for (int i = 0; i < length; i++) {
    byte = message[i];  // Get next byte.
    crc = crc ^ byte;
    for (int j = 7; j >= 0; j--) {  // Do eight times.
      mask = -(crc & 1);
      crc = (crc >> 1) ^ (0xEDB88320 & mask);
    }
  }
  return ~crc;
}

absl::Status SendEthernetFrame(Socket *socket,
  const MacAddr &src, const MacAddr &dst,
  void *payload, size_t payload_size,
  uint16_net ether_type) {
  if (payload_size > kEthernetPayloadMax) {
    return absl::InvalidArgumentError(
      absl::StrFormat("Size too big: %d > 1500", payload_size));
  }

  EthernetHeader header = EthernetHeader(dst, src, ether_type);
  size_t padded_payload_size = payload_size < kEthernetPayloadMin ?
    kEthernetPayloadMin : payload_size;

  // header + payload + frame check sequence
  size_t frame_size = sizeof(header) + padded_payload_size + kCrcSize;

  std::vector<uint8_t> frame;
  frame.resize(frame_size);
  memset(&frame[0], 0, frame.size());
  memcpy(&frame[0], &header, sizeof(header));
  memcpy(&frame[sizeof(header)], payload, payload_size);

  uint32_t crc = crc32(frame.data(), frame.size() - kCrcSize);

  printf("CRC: %04x\n", crc);
  printf("CRC is over: ");
  DumpHex(frame.data(), frame.size() - kCrcSize);

  memcpy(&frame[frame.size() - kCrcSize], &crc, kCrcSize);

  if (absl::GetFlag(FLAGS_dump_ethernet)) {
    DumpEthernetFrame(frame.data(), frame.size());
  }

  printf("Full data: (%d) ", frame.size());
  DumpHex(frame.data(), frame.size());
  if (socket->SendTo(frame.data(), frame.size(), dst) == -1) {
    return absl::InternalError(absl::StrFormat("Send failed ('%s')",
      strerror(errno)));
  }

  return absl::OkStatus();
}

//
// TODO(jmecom)
// Issues:
// 1) Raw socket is receiving its own packet
//    https://stackoverflow.com/questions/16543191/missing-arp-packets-in-sock-raw-socket
//    https://stackoverflow.com/questions/55517735/raw-sockets-receiving-messages-sent-by-itself
// 2) Mismatch between data being sent and received.
//

absl::Status RecvEthernetFrame(Socket *socket,
  std::vector<uint8_t> *payload, size_t payload_size) {
  if (payload_size < kEthernetPayloadMin) {
    payload_size = kEthernetPayloadMin;
  }

  uint8_t data[payload_size + kEthernetOverhead] = {0};

  ssize_t size = socket->RecvAll(data, sizeof(data));
  if (size == -1) {
    return absl::InternalError("No data");
  }
  if (absl::GetFlag(FLAGS_dump_ethernet)) {
    DumpEthernetFrame(data, size);
  }

  uint32_t expected_crc =
    *(reinterpret_cast<uint32_t *>(data + size - kCrcSize));
  uint32_t crc = crc32(data, size - kCrcSize);

  printf("CRC is over: ");
  DumpHex(data, size - kCrcSize);
  printf("Full data: (%d) ", size);
  DumpHex(data, size);

  if (crc != expected_crc) {
    return absl::InternalError(
      absl::StrFormat("CRC mismatch: 0x%04x vs 0x%04x", crc, expected_crc));
  }

  uint8_t *sent_payload = data + sizeof(EthernetHeader);
  payload->resize(size - kEthernetOverhead);
  memcpy(&((*payload)[0]), sent_payload, payload->size());

  return absl::OkStatus();
}

}  // namespace orange_tcp
