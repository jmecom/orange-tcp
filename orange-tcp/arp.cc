#include "arp.h"
#include "net.h"
#include "eth.h"

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

  return SendEthernetFrame(socket, src_mac, kBroadcastMac,
    reinterpret_cast<void *>(&arp_request),
    sizeof(arp_request), kEtherTypeArp);
}

absl::Status MaybeHandleResponse(Socket *socket) {
  std::vector<uint8_t> payload;
  auto status = RecvEthernetFrame(socket, &payload);
  if (!status.ok()) {
    return status;
  }

  Packet *arp_request = reinterpret_cast<Packet *>(&payload[0]);
  printf("arp recv %s\n", arp_request->src_hw_addr.ToString().c_str());

  return absl::OkStatus();
}

}  // namespace arp
}  // namespace orange_tcp
