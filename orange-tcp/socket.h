#include "net.h"

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

  ssize_t SendTo(void *buffer, size_t length, Address addr);
  ssize_t RecvFrom(void *buffer, size_t length, Address addr);

  absl::Status GetHostMacAddress(uint8_t(&addr_out)[kMacAddrLen]);
  absl::Status GetHostIpAddress(uint8_t(&addr_out)[kIpAddrLen]);

  const int fd() { return fd_; }

 private:
  int fd_;

  // TODO(jmecom) Shouldn't be hardcoded, but good enough for now.
  const char *kEthDevice = "eth0";
};


}  // namespace orange_tcp
