#include "arp.h"
#include "socket.h"
#include "net.h"

#include <arpa/inet.h>
#include <memory>
#include <utility>

#include "absl/strings/str_format.h"

namespace orange_tcp {
namespace arp {

absl::Status Request(const IpAddr &ip, const MacAddr &mac) {
  auto socket_result = Socket::Create();
  if (!socket_result.ok()) {
    return absl::InternalError("Failed to create socket");
  }
  std::unique_ptr<Socket> socket = std::move(socket_result.value());

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

}  // namespace arp
}  // namespace orange_tcp
