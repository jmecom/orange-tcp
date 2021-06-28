#include "arp.h"
#include "net.h"

#include <arpa/inet.h>
#include <memory>
#include <utility>

#include "absl/strings/str_format.h"

namespace orange_tcp {
namespace arp {

absl::Status Request(Socket *socket) {
  auto mac_result = socket->GetHostMacAddress();
  if (!mac_result.ok())
    return absl::InternalError("Failed to get source MAC address");
  auto src_mac = mac_result.value();

  auto ip_result = socket->GetHostIpAddress();
  if (!ip_result.ok())
    return absl::InternalError("Failed to get source IP address");

  Packet arp_request = Packet(kEthernetHwType, kIpProtocolType,
    kMacAddrLen, kIpAddrLen, kArpRequest, src_mac, ip_result.value(),
    kBroadcastMac, kBroadcastIp);

  EthernetFrame frame = EthernetFrame(kBroadcastMac, src_mac, kEtherTypeArp);
  memcpy(frame.data, &arp_request, sizeof(arp_request));

  if (socket->SendTo(static_cast<void *>(&frame),
                     sizeof(frame), kBroadcastMac) == -1) {
    return absl::InternalError(absl::StrFormat("Send failed ('%s')",
      strerror(errno)));
  }

  return absl::OkStatus();
}

// TODO(jmecom) Implement... figure out how raw sockets work w/ broadcasting.
// Plan: get ARP working for real before trying to unit test, so I don't implement it
// incorrectly.
absl::Status MaybeHandleResponse(Socket *socket) {
  std::vector<uint8_t> data;
  data.reserve(kEthernetMtu);

  ssize_t size = socket->Recv(data.data(), data.size());
  if (size == -1) {
    return absl::InternalError("No data");
  }

  DumpHex(data.data(), size);

  return absl::OkStatus();
}

}  // namespace arp
}  // namespace orange_tcp
