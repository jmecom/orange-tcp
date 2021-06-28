#include "socket.h"

#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "absl/strings/str_format.h"

namespace orange_tcp {

absl::StatusOr<std::unique_ptr<Socket>> PosixSocket::Create() {
  int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  // int fd = socket(AF_INET, SOCK_STREAM, 0);
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

  return std::make_unique<PosixSocket>(fd);
}

std::unique_ptr<Socket> PosixSocket::CreateOrDie() {
  auto socket_result = PosixSocket::Create();
  if (!socket_result.ok()) {
    fprintf(stderr, "Failed to create socket\n");
    exit(1);
  }
  return std::move(socket_result.value());
}

ssize_t PosixSocket::Send(void *buffer, size_t length) {
  return send(fd_, buffer, length, 0);
}

ssize_t PosixSocket::Recv(void *buffer, size_t length) {
  return recv(fd_, buffer, length, 0);
}

ssize_t PosixSocket::SendTo(void *buffer, size_t length, MacAddr dst) {
  auto sock_result = MakeSockAddr(dst);
  if (!sock_result.ok()) return -123;

  struct sockaddr_ll ll = sock_result.value();
  return sendto(fd_, buffer, length, 0,
    reinterpret_cast<struct sockaddr *>(&ll), sizeof(ll));
}

ssize_t PosixSocket::RecvFrom(void *buffer, size_t length, MacAddr src) {
  auto sock_result = MakeSockAddr(src);
  if (!sock_result.ok()) return -123;

  struct sockaddr_ll ll = sock_result.value();
  socklen_t size = sizeof(ll);
  return recvfrom(fd_, buffer, length, 0,
    reinterpret_cast<struct sockaddr *>(&ll), &size);
}

absl::StatusOr<int> PosixSocket::GetInterfaceIndex() {
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

absl::StatusOr<MacAddr> PosixSocket::GetHostMacAddress() {
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

absl::StatusOr<IpAddr> PosixSocket::GetHostIpAddress() {
  static bool cached = false;
  if (cached) return host_ip_;

  struct ifreq ifr = {0};
  strncpy(ifr.ifr_name, kEthDevice, IFNAMSIZ);
  ifr.ifr_addr.sa_family = AF_INET;

  if (ioctl(fd_, SIOCGIFADDR, &ifr) < 0) {
    return absl::InternalError(strerror(errno));
  }

  memcpy(host_ip_.addr, ifr.ifr_addr.sa_data, kIpAddrLen);
  cached = true;

  return host_ip_;
}

absl::StatusOr<struct sockaddr_ll> PosixSocket::MakeSockAddr(MacAddr dst) {
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
