#include "arp.h"
#include "socket.h"
#include "net.h"

#include <arpa/inet.h>
#include <memory>
#include <utility>

#include "absl/strings/str_format.h"

namespace orange_tcp {

absl::Status Arp::Request(const uint8_t(&ip_addr)[kIpAddrLen],
    uint8_t(&mac_addr)[kMacAddrLen]) {
  auto socket_result = Socket::Create();
  if (!socket_result.ok()) {
    return absl::InternalError("Failed to create socket");
  }
  std::unique_ptr<Socket> socket = std::move(socket_result.value());

  Arp::Packet arp_request = {
    .hw_type = htons(kEthernetHwType),
    .p_type = htons(kIpProtocolType),
    .hw_addr_len = kMacAddrLen,
    .p_len = kIpAddrLen,
    .opcode = htons(kArpRequest),
  };

  if (!socket->GetHostMacAddress(arp_request.src_hw_addr).ok()) {
    return absl::InternalError("Failed to get source MAC address");
  }
  if (!socket->GetHostIpAddress(arp_request.src_ip_addr).ok()) {
    return absl::InternalError("Failed to get source IP address");
  }

  memcpy(arp_request.dst_hw_addr, kBroadcastMac, kMacAddrLen);
  memcpy(arp_request.dst_ip_addr, kBroadcastIp, kIpAddrLen);

  auto frame = MakeEthernetFrame(arp_request.src_hw_addr,
    kBroadcastMac, reinterpret_cast<uint8_t *>(&arp_request),
    sizeof(arp_request));
  if (!frame) return absl::InternalError("Failed to make ethernet frame");
  frame->ether_type = EtherType::kArp;

  Address addr;
  memcpy(addr.ip_addr, kBroadcastIp, kIpAddrLen);

  if (socket->SendTo(static_cast<void *>(frame.get()),
                     sizeof(frame), addr) == -1) {
    return absl::InternalError(absl::StrFormat("Send failed ('%s')",
      strerror(errno)));
  }

  return absl::OkStatus();
}

}  // namespace orange_tcp
