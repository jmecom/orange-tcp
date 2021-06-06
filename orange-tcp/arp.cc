#include "arp.h"
#include "socket.h"
#include "net.h"

#include <arpa/inet.h>
#include <memory>
#include <utility>

#include "absl/strings/str_format.h"

namespace orange_tcp {
namespace arp {

std::vector<uint8_t> Packet::Pack() {
  auto hw_type = htons(hw_type_);
  auto p_type = htons(p_type_);
  auto opcode = htons(opcode_);

  const int size =
    sizeof(hw_type_) + sizeof(p_type_) + sizeof(hw_addr_len_) +
    sizeof(p_len_) + sizeof(opcode_) + hw_addr_len_ * 2 + kIpAddrLen * 2;

  std::vector<uint8_t> packed = std::vector<uint8_t>(size);
  int off = 0;
  memcpy(packed.data() + off, &hw_type, sizeof(hw_type));
  off += sizeof(hw_type);
  memcpy(packed.data() + off, &p_type, sizeof(p_type));
  off += sizeof(p_type);
  memcpy(packed.data() + off, &hw_addr_len_, sizeof(hw_addr_len_));
  off += sizeof(hw_addr_len_);
  memcpy(packed.data() + off, &p_len_, sizeof(p_len_));
  off += sizeof(p_len_);
  memcpy(packed.data() + off, &opcode, sizeof(opcode));
  off += sizeof(opcode);

  memcpy(packed.data() + off, src_hw_addr_.addr, hw_addr_len_);
  off += hw_addr_len_;
  memcpy(packed.data() + off, src_ip_addr_.addr, kIpAddrLen);
  off += kIpAddrLen;

  memcpy(packed.data() + off, dst_hw_addr_.addr, hw_addr_len_);
  off += hw_addr_len_;
  memcpy(packed.data() + off, dst_ip_addr_.addr, kIpAddrLen);
  off += kIpAddrLen;

  assert(off == size);

  return packed;
}

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

  std::vector<uint8_t> arp_request = Packet(kEthernetHwType, kIpProtocolType,
    kMacAddrLen, kIpAddrLen, kArpRequest, src_mac, ip_result.value(),
    kBroadcastMac, kBroadcastIp).Pack();

  std::vector<uint8_t> frame = EthernetFrame(kBroadcastMac,
    src_mac, EtherType::kArp, arp_request.data(),
    arp_request.size()).Pack();

  auto addr = Address(kBroadcastIp, 1234); // TODO(jmecom) Which port?

  if (socket->SendTo(static_cast<void *>(frame.data()),
                     frame.size(), addr) == -1) {
    return absl::InternalError(absl::StrFormat("Send failed ('%s')",
      strerror(errno)));
  }

  return absl::OkStatus();
}

}  // namespace arp
}  // namespace orange_tcp
