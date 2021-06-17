#include "network_sim.h"

#include <vector>

namespace orange_tcp {

ssize_t FakeSocket::Send(void *buffer, size_t length) {
  return -1;
}
ssize_t FakeSocket::Recv(void *buffer, size_t length) {
  return -1;
}

// TODO(jmecom) Check blocking / nonblocking?
ssize_t FakeSocket::SendTo(void *buffer, size_t length, MacAddr dst) {
  Host *recipient = network_->HostForMac(dst);
  uint8_t *b = static_cast<uint8_t *>(buffer);
  std::vector<uint8_t> vec(b, b + length);
  recipient->Push(host_mac_, vec);
  return length;
}

ssize_t FakeSocket::RecvFrom(void *buffer, size_t length, MacAddr src) {
  Host *self = network_->HostForMac(host_mac_);
  std::vector<uint8_t> received = self->Pop(src, length);
  memcpy(buffer, received.data(), length);
  return length;
}

}  // namespace orange_tcp
