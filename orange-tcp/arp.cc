#include "arp.h"
#include "net.h"
#include "eth.h"

#include <arpa/inet.h>
#include <memory>
#include <utility>

#include "absl/strings/str_format.h"
#include "absl/flags/flag.h"

ABSL_FLAG(bool, dump_arp, false, "Set to log ARP debugging information.");

namespace orange_tcp {
namespace arp {

absl::Status Request(Socket *socket, const IpAddr& dst_ip) {
  auto mac_result = socket->GetHostMacAddress();
  if (!mac_result.ok())
    return absl::InternalError("Failed to get source MAC address");
  auto src_mac = mac_result.value();

  auto ip_result = socket->GetHostIpAddress();
  if (!ip_result.ok())
    return absl::InternalError("Failed to get source IP address");

  Packet arp_request = Packet(kEthernetHwType, kIpProtocolType,
    kMacAddrLen, kIpAddrLen, kArpRequest, src_mac, ip_result.value(),
    kBroadcastMac, dst_ip);

  if (absl::GetFlag(FLAGS_dump_arp)) {
    printf("[arp] Sending request %s\n", arp_request.ToString().c_str());
  }

  return SendEthernetFrame(socket, src_mac, kBroadcastMac,
    reinterpret_cast<void *>(&arp_request),
    sizeof(arp_request), kEtherTypeArp);
}

absl::Status HandleRequest(Socket *socket) {
  bool log = absl::GetFlag(FLAGS_dump_arp);

  std::vector<uint8_t> payload;
  auto status = RecvEthernetFrame(socket, &payload);
  if (!status.ok()) {
    return status;
  }

  Packet *arp_request = reinterpret_cast<Packet *>(&payload[0]);

  // Is the request valid?
  if (arp_request->opcode != kArpRequest) {
    return absl::InternalError(absl::StrFormat("%d is not an ARP repuest?",
      uint16_t(arp_request->opcode)));
  }

  // Is the request for this host?
  auto ip_result = socket->GetHostIpAddress();
  if (!ip_result.ok()) {
    return absl::InternalError("Failed to get IP address");
  }
  auto ip = ip_result.value();

  if (arp_request->dst_ip_addr != ip) {
    if (log) {
      printf("[arp] Ignoring request: got %s but host is %s\n",
      arp_request->dst_ip_addr.ToString().c_str(),
      ip.ToString().c_str());
    }
    return absl::OkStatus();
  }

  if (log) {
    printf("[arp] Handling request %s\n", arp_request->ToString().c_str());
  }

  // Get own MAC address
  auto mac_result = socket->GetHostMacAddress();
  if (!mac_result.ok())
    return absl::InternalError("Failed to get MAC address");
  auto mac = mac_result.value();

  // Build response
  Packet arp_response = Packet(kEthernetHwType, kIpProtocolType,
    kMacAddrLen, kIpAddrLen, kArpResponse, mac, arp_request->dst_ip_addr,
    arp_request->src_hw_addr, arp_request->src_ip_addr);

  if (log) {
    printf("[arp] Sending response %s\n", arp_response.ToString().c_str());
  }

  // return absl::OkStatus();
  return SendEthernetFrame(socket, mac, arp_response.dst_hw_addr,
    &arp_response, sizeof(arp_response), kEtherTypeArp);
}

absl::StatusOr<MacAddr> HandleResponse(Socket *socket) {
  std::vector<uint8_t> payload;
  auto status = RecvEthernetFrame(socket, &payload);
  if (!status.ok()) {
    return status;
  }

  Packet *arp_response = reinterpret_cast<Packet *>(&payload[0]);
  if (arp_response->opcode != kArpResponse) {
    return absl::InternalError(absl::StrFormat("%d is not an ARP reply?",
      uint16_t(arp_response->opcode)));
  }

  MacAddr mac;
  memcpy(mac.addr, arp_response->dst_hw_addr.addr, kMacAddrLen);

  if (absl::GetFlag(FLAGS_dump_arp)) {
    printf("[arp] Got MAC %s for IP %s\n",
      mac.ToString().c_str(),
      arp_response->dst_ip_addr.ToString().c_str());
  }

  return mac;
}

}  // namespace arp
}  // namespace orange_tcp
