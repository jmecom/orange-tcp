#include "socket.h"

#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "absl/strings/str_format.h"

namespace orange_tcp {

absl::StatusOr<std::unique_ptr<Socket>> RawSocket::Create() {
  // Not using ETH_P_ALL since it also receives outgoing frames.
  int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP | ETH_P_ARP));
  if (fd < 0) {
    return absl::InternalError(
      absl::StrFormat("Failed to create socket: %s",
        strerror(errno)));
  }

  // TODO(jmecom) Needed?
  // int ret = setsockopt(fd, SOL_SOCKET, SO_BROADCAST,
  //   &broadcastEnable, sizeof(broadcastEnable));

  // TODO(jmecom) Needed?
  // setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, "eth0", 4);

  return std::make_unique<RawSocket>(fd);
}

std::unique_ptr<Socket> RawSocket::CreateOrDie() {
  auto socket_result = RawSocket::Create();
  if (!socket_result.ok()) {
    fprintf(stderr, "Failed to create socket\n");
    exit(1);
  }
  return std::move(socket_result.value());
}

ssize_t RawSocket::Send(void *buffer, size_t length) {
  return send(fd_, buffer, length, 0);
}

ssize_t RawSocket::Recv(void *buffer, size_t length) {
  return recv(fd_, buffer, length, 0);
}

ssize_t RawSocket::SendTo(void *buffer, size_t length, MacAddr dst) {
  auto sock_result = MakeSockAddr(dst);
  if (!sock_result.ok()) return -123;

  struct sockaddr_ll ll = sock_result.value();
  return sendto(fd_, buffer, length, 0,
    reinterpret_cast<struct sockaddr *>(&ll), sizeof(ll));
}

ssize_t RawSocket::RecvFrom(void *buffer, size_t length, MacAddr src) {
  auto sock_result = MakeSockAddr(src);
  if (!sock_result.ok()) return -123;

  struct sockaddr_ll ll = sock_result.value();
  socklen_t size = sizeof(ll);
  return recvfrom(fd_, buffer, length, 0,
    reinterpret_cast<struct sockaddr *>(&ll), &size);
}

ssize_t RawSocket::RecvAll(void *buffer, size_t length) {
  uint8_t *off = reinterpret_cast<uint8_t *>(buffer);
  size_t ret = 0, size = length;
  while (size > 0) {
    ret = Recv(off, size);
    if (ret < 0 || ret > size) return -1;
    size -= ret;
    off += ret;
  }
  return length;
}

absl::StatusOr<int> RawSocket::GetInterfaceIndex() {
  static bool cached = false;
  if (cached) return interface_index_;

  struct ifreq ifr = {0};
  strncpy(ifr.ifr_name, kEthDevice, IFNAMSIZ);
  if (ioctl(fd_, SIOCGIFINDEX, &ifr) < 0) {
    return absl::InternalError(strerror(errno));
  }

  interface_index_ = ifr.ifr_ifindex;
  cached = true;
  return interface_index_;
}

absl::StatusOr<MacAddr> RawSocket::GetHostMacAddress() {
  static bool cached = false;
  if (cached) return host_mac_;

  struct ifreq ifr = {0};
  strncpy(ifr.ifr_name, kEthDevice, IFNAMSIZ);
  if (ioctl(fd_, SIOCGIFINDEX, &ifr) < 0) {
    return absl::InternalError(strerror(errno));
  }
  if (ioctl(fd_, SIOCGIFHWADDR, &ifr) < 0) {
    return absl::InternalError(strerror(errno));
  }

  memcpy(host_mac_.addr, ifr.ifr_hwaddr.sa_data, kMacAddrLen);

  return host_mac_;
}

absl::StatusOr<IpAddr> RawSocket::GetHostIpAddress() {
  static bool cached = false;
  if (cached) return host_ip_;

  struct ifreq ifr = {0};
  strncpy(ifr.ifr_name, kEthDevice, IFNAMSIZ);
  ifr.ifr_addr.sa_family = AF_INET;

  if (ioctl(fd_, SIOCGIFADDR, &ifr) < 0) {
    return absl::InternalError(strerror(errno));
  }

  auto *s = reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
  host_ip_.addr = s->sin_addr.s_addr;

  cached = true;

  return host_ip_;
}

std::pair<MacAddr, IpAddr> RawSocket::GetHostMacAndIpOrDie() {
  auto ip_result = GetHostIpAddress();
  if (!ip_result.ok()) {
    fprintf(stderr, "Failed to get source IP address\n");
    exit(1);
  }
  auto ip = ip_result.value();

  auto mac_result = GetHostMacAddress();
  if (!mac_result.ok()) {
    fprintf(stderr, "Failed to get source MAC address\n");
    exit(1);
  }
  auto mac = mac_result.value();

  return std::pair<MacAddr, IpAddr>(mac, ip);
}

absl::StatusOr<struct sockaddr_ll> RawSocket::MakeSockAddr(MacAddr dst) {
  struct sockaddr_ll ll = {0};
  ll.sll_family = AF_PACKET;

  auto idx_result = GetInterfaceIndex();
  if (!idx_result.ok()) {
    return absl::InternalError("Failed to fill sockaddr");
  }
  ll.sll_ifindex = idx_result.value();

  // TODO(jmecom)
  // https://man7.org/linux/man-pages/man7/packet.7.html
  // PACKET_BROADCAST?

  ll.sll_halen = kMacAddrLen;
  memcpy(ll.sll_addr, dst.addr, kMacAddrLen);
  return ll;
}

}  // namespace orange_tcp
