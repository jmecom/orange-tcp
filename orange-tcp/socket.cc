#include "socket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "absl/strings/str_format.h"

namespace orange_tcp {

absl::StatusOr<std::unique_ptr<Socket>> Socket::Create() {
  int fd = socket(AF_PACKET, SOCK_RAW, ETH_P_ALL);
  if (fd < 0) {
    return absl::InternalError(
      absl::StrFormat("Failed to create socket: %s",
        strerror(errno)));
  }

  // TODO(jmecom) Needed?
  // int ret = setsockopt(fd, SOL_SOCKET, SO_BROADCAST,
  //   &broadcastEnable, sizeof(broadcastEnable));

  return std::make_unique<Socket>(fd);
}

ssize_t Socket::Send(void *buffer, size_t length) {
  return send(fd_, buffer, length, 0);
}

ssize_t Socket::Recv(void *buffer, size_t length) {
  return recv(fd_, buffer, length, 0);
}

ssize_t Socket::SendTo(void *buffer, size_t length, Address addr) {
  sockaddr_in dest;
  addr.FillSockaddr(&dest);
  return sendto(fd_, buffer, length, 0,
    reinterpret_cast<sockaddr *>(&dest), sizeof(sockaddr_in));
}

ssize_t Socket::RecvFrom(void *buffer, size_t length, Address addr) {
  sockaddr src;
  addr.FillSockaddr(reinterpret_cast<sockaddr_in *>(&src));
  socklen_t size = sizeof(sockaddr);
  return recvfrom(fd_, buffer, length, 0, &src, &size);
}

absl::Status Socket::GetHostMacAddress(uint8_t(&addr_out)[kMacAddrLen]) {
  struct ifreq ifr;

  // Retrieve the ethernet interface index.
  strncpy(ifr.ifr_name, kEthDevice, IFNAMSIZ);
  if (ioctl(fd_, SIOCGIFINDEX, &ifr) < 0) {
    return absl::InternalError("ioctl SIOCGIFINDEX failed");
  }

  // Retrieve corresponding MAC address.
  if (ioctl(fd_, SIOCGIFHWADDR, &ifr) < 0) {
    return absl::InternalError("ioctl SIOCGIFHWADDR failed");
  }

  memcpy(addr_out, ifr.ifr_hwaddr.sa_data, kMacAddrLen);

  return absl::OkStatus();
}

absl::Status Socket::GetHostIpAddress(uint8_t(&addr_out)[kIpAddrLen]) {
  struct ifreq ifr;
  strncpy(ifr.ifr_name, kEthDevice, IFNAMSIZ);
  ifr.ifr_addr.sa_family = AF_INET;

  if (ioctl(fd_, SIOCGIFADDR, &ifr) < 0) {
    return absl::InternalError("ioctl SIOCGIFADDR failed");
  }

  memcpy(addr_out, ifr.ifr_addr.sa_data, kIpAddrLen);

  return absl::OkStatus();
}

}  // namespace orange_tcp
