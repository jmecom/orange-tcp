#pragma once

#include "net.h"

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#include <cstddef>
#include <unistd.h>
#include <memory>
#include <utility>

#include "absl/status/statusor.h"
#include "absl/status/status.h"

namespace orange_tcp {

class Socket {
 public:
  virtual ~Socket() = default;

  // Wrappers around send/recv
  virtual ssize_t Send(void *buffer, size_t length) = 0;
  virtual ssize_t Recv(void *buffer, size_t length) = 0;
  virtual ssize_t SendTo(void *buffer, size_t length, MacAddr dst) = 0;
  virtual ssize_t RecvFrom(void *buffer, size_t length, MacAddr src) = 0;

  // Blocking calls which guarantees all `length` bytes are sent/received.
  virtual ssize_t RecvAll(void *buffer, size_t length) = 0;

  virtual absl::StatusOr<MacAddr> GetHostMacAddress() = 0;
  virtual absl::StatusOr<IpAddr> GetHostIpAddress() = 0;
  virtual absl::StatusOr<std::pair<MacAddr, IpAddr>> GetHostMacAndIp() = 0;
};

class RawSocket : public Socket {
 public:
  static absl::StatusOr<std::unique_ptr<Socket>> Create();
  static std::unique_ptr<Socket> CreateOrDie();

  // Prefer factory function Create() instead of using constructor.
  explicit RawSocket(int fd) : fd_(fd) {}

  ssize_t Send(void *buffer, size_t length);
  ssize_t Recv(void *buffer, size_t length);
  ssize_t SendTo(void *buffer, size_t length, MacAddr dst);
  ssize_t RecvFrom(void *buffer, size_t length, MacAddr src);
  ssize_t RecvAll(void *buffer, size_t length);

  absl::StatusOr<MacAddr> GetHostMacAddress();
  absl::StatusOr<IpAddr> GetHostIpAddress();
  absl::StatusOr<std::pair<MacAddr, IpAddr>> GetHostMacAndIp();

 private:
  absl::StatusOr<int> GetInterfaceIndex();
  absl::StatusOr<struct sockaddr_ll> MakeSockAddr(MacAddr dst);

  int fd_;

  MacAddr host_mac_;
  IpAddr host_ip_;
  int interface_index_;

  // TODO(jmecom) Shouldn't be hardcoded, but good enough for now.
  const char *kEthDevice = "eth0";
};

}  // namespace orange_tcp
