#pragma once

#include "net.h"

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#include <cstddef>
#include <unistd.h>
#include <memory>

#include "absl/status/statusor.h"
#include "absl/status/status.h"

namespace orange_tcp {

// A wrapper around standard socket-related syscalls, allowing for
// unit-testability.
class Socket {
 public:
  static absl::StatusOr<std::unique_ptr<Socket>> Create();

  // Prefer factory function Create() instead of using constructor.
  explicit Socket(int fd) : fd_(fd) {}

  ssize_t Send(void *buffer, size_t length);
  ssize_t Recv(void *buffer, size_t length);

  ssize_t SendTo(void *buffer, size_t length, MacAddr dst);
  ssize_t RecvFrom(void *buffer, size_t length, MacAddr src);

  absl::StatusOr<MacAddr> GetHostMacAddress();
  absl::StatusOr<IpAddr> GetHostIpAddress();

  const int fd() { return fd_; }

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
