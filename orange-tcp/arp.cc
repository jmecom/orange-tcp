#include "arp.h"
#include "net.h"
#include "eth.h"
#include "serializable_map.h"

#include <arpa/inet.h>
#include <memory>
#include <utility>
#include <iterator>
#include <algorithm>
#include <fstream>

#include "absl/strings/str_format.h"
#include "absl/flags/flag.h"

ABSL_FLAG(bool, dump_arp, false, "Set to log ARP debugging information.");
ABSL_FLAG(std::string, arp_cache, "/tmp/arp_cache", "ARP cache file location.");

namespace orange_tcp {
namespace arp {

static serializable_map<IpAddr, MacAddr> g_arp_cache;

void MaybeLoadArpCache() {
  static bool loaded = false;
  if (loaded) return;

  std::ifstream cache(absl::GetFlag(FLAGS_arp_cache));
  std::vector<char> buffer((std::istreambuf_iterator<char>(cache)),
                            std::istreambuf_iterator<char>());

  g_arp_cache.deserialize(buffer);

  loaded = true;
}

void SerializeArpCache() {
  std::vector<char> buffer = g_arp_cache.serialize();
  std::ofstream file(absl::GetFlag(FLAGS_arp_cache),
    std::ios::binary | std::ios::ate);
  file.write(buffer.data(), buffer.size());
}

absl::StatusOr<MacAddr> GetMac(Socket *socket, const IpAddr& ip) {
  MacAddr dst_mac;
  auto result = Request(socket, ip, &dst_mac);
  if (absl::IsAlreadyExists(result)) {
    return dst_mac;
  }

  if (!result.ok()) {
    return absl::InternalError(
      absl::StrFormat("ARP request failed: %s", result.message()));
  }

  result = HandleResponse(socket, &dst_mac);
  if (!result.ok()) {
    return absl::InternalError(
      absl::StrFormat("Failed to handle ARP response: %s", result.message()));
  }

  return dst_mac;
}

absl::Status Request(Socket *socket, const IpAddr& dst_ip,
                     MacAddr *mac_addr_out) {
  MaybeLoadArpCache();

  if (g_arp_cache.find(dst_ip) != g_arp_cache.end()) {
    *mac_addr_out = g_arp_cache[dst_ip];
    return absl::AlreadyExistsError("MAC found");
  }

  auto mac_result = socket->GetHostMacAddress();
  if (!mac_result.ok()) {
    return absl::InternalError("Failed to get source MAC address");
  }
  auto src_mac = mac_result.value();

  auto ip_result = socket->GetHostIpAddress();
  if (!ip_result.ok()) {
    return absl::InternalError("Failed to get source IP address");
  }

  Packet arp_request = Packet(kEthernetHwType, kIpProtocolType,
    kMacAddrLen, kIpAddrLen, kArpRequest, src_mac, ip_result.value(),
    kBroadcastMac, dst_ip);

  if (absl::GetFlag(FLAGS_dump_arp)) {
    printf("[arp] Sending request %s\n", arp_request.str().c_str());
  }

  return SendEthernetFrame(socket, src_mac, kBroadcastMac,
    reinterpret_cast<void *>(&arp_request),
    sizeof(arp_request), kEtherTypeArp);
}

absl::Status HandleRequest(Socket *socket) {
  bool log = absl::GetFlag(FLAGS_dump_arp);

  std::vector<uint8_t> payload;
  auto status = RecvEthernetFrame(socket, &payload, sizeof(Packet));
  if (!status.ok()) {
    return status;
  }

  Packet *arp_request = reinterpret_cast<Packet *>(&payload[0]);

  if (log) {
    printf("[arp] Handling request %s\n", arp_request->str().c_str());
  }

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
      arp_request->dst_ip_addr.str().c_str(),
      ip.str().c_str());
    }
    return absl::OkStatus();
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
    printf("[arp] Sending response %s\n",
      arp_response.str().c_str());
  }

  return SendEthernetFrame(socket, mac, arp_response.dst_hw_addr,
    &arp_response, sizeof(arp_response), kEtherTypeArp);
}

absl::Status HandleResponse(Socket *socket, MacAddr *mac_addr_out) {
  MaybeLoadArpCache();

  std::vector<uint8_t> payload;
  auto status = RecvEthernetFrame(socket, &payload, sizeof(Packet));
  if (!status.ok()) {
    return status;
  }

  Packet *arp_response = reinterpret_cast<Packet *>(&payload[0]);

  if (absl::GetFlag(FLAGS_dump_arp)) {
    printf("[arp] Got response %s\n",
      arp_response->str().c_str());
  }

  if (arp_response->opcode != kArpResponse) {
    return absl::InternalError(absl::StrFormat("%d is not an ARP reply?",
      uint16_t(arp_response->opcode)));
  }

  memcpy(mac_addr_out->addr, arp_response->src_hw_addr.addr, kMacAddrLen);

  g_arp_cache[arp_response->src_ip_addr] = *mac_addr_out;
  SerializeArpCache();

  return absl::OkStatus();
}

}  // namespace arp
}  // namespace orange_tcp
