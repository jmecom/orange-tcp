#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <map>

#include "net.h"
#include "socket.h"

#include "absl/status/statusor.h"
#include "absl/status/status.h"

namespace orange_tcp {

class MessageQueue {
 public:
  void Push(std::vector<uint8_t> vec) {
    message_queue_.push(vec);
  }

  std::vector<uint8_t> Pop() {
    auto result = message_queue_.front();
    message_queue_.pop();
    return result;
  }

 private:
  std::queue<std::vector<uint8_t>> message_queue_;
};

class Host {
 public:
  explicit Host(IpAddr ip, MacAddr mac) :
    ip_(ip), mac_(mac) {}

  void Push(MacAddr sender, std::vector<uint8_t> data) {
    if (!queues_.contains(sender)) Die("Sender not found");
    queues_[sender].Push(data);
  }

  std::vector<uint8_t> Pop(MacAddr sender) {
    if (!queues_.contains(sender)) Die("Sender not found");
    return queues_[sender].Pop();
  }

  const MacAddr mac() { return mac_; }

 private:
  IpAddr ip_;
  MacAddr mac_;

  // Queues holding data waiting to be received on this host.
  std::map<MacAddr, MessageQueue> queues_;
};

class SimulatedNetwork {
 public:
  Host *HostForMac(MacAddr mac) {
    for (auto& h : hosts_)
      if (h->mac() == mac) return h.get();
    Die("Couldn't find host");
  }

 private:
  std::vector<std::unique_ptr<Host>> hosts_;
};

class FakeSocket : public Socket {
 public:
  explicit FakeSocket(SimulatedNetwork *network) : network_(network) {}

  ssize_t Send(void *buffer, size_t length);
  ssize_t Recv(void *buffer, size_t length);

  ssize_t SendTo(void *buffer, size_t length, MacAddr dst);
  ssize_t RecvFrom(void *buffer, size_t length, MacAddr src);

  absl::StatusOr<MacAddr> GetHostMacAddress() { return host_mac_; }
  absl::StatusOr<IpAddr> GetHostIpAddress() { return host_ip_; }

  void SetHostMacAddress(MacAddr mac) { host_mac_ = mac; }
  void SetHostIpAddress(IpAddr ip) { host_ip_ = ip; }

 private:
  SimulatedNetwork *network_;
  MacAddr host_mac_;
  IpAddr host_ip_;
};

}  // namespace orange_tcp
