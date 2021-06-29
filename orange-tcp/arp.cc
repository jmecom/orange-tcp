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
  uint8_t data[kEthernetMtu] = {0};

  ssize_t size = socket->Recv(data, sizeof(data));
  if (size == -1) {
    return absl::InternalError("No data");
  }

  // EthernetFrame *frame = reinterpret_cast<EthernetFrame *>(data);
  // DumpEthernetFrame(frame);
  // Packet *arp_request = reinterpret_cast<Packet *>(frame->data);

  return absl::OkStatus();
}

}  // namespace arp
}  // namespace orange_tcp
