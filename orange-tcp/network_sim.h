#pragma once

#include <memory>
#include <deque>
#include <vector>
#include <map>
#include <utility>

#include "net.h"
#include "socket.h"

#include "absl/status/statusor.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"

namespace orange_tcp {

IpAddr RandomIp() {
  IpAddr ip; //  TODO
  return ip;
}

class Host {
 public:
  explicit Host(IpAddr ip, MacAddr mac) :
    ip_(ip), mac_(mac) {}

  void Push(MacAddr sender, std::vector<uint8_t> data) {
    if (sender == kBroadcastMac) {
      for (auto &[mac, q] : queues_)
        q.insert(q.end(), data.begin(), data.end());
    } else {
      // Sender is automatically added to queues_ if not present.
      auto q = queues_[sender];
      q.insert(q.end(), data.begin(), data.end());
    }
  }

  std::vector<uint8_t> Pop(MacAddr sender, size_t length) {
    auto q = queues_[sender];

    if (length > q.size())
      throw std::invalid_argument("Length is too large");

    std::vector<uint8_t> result;
    for (size_t i = 0; i < length; i++) {
      result.push_back(q.front());
      q.pop_front();
    }
    return result;
  }

  const MacAddr mac() { return mac_; }
  const IpAddr ip() { return ip_; }

  void Print() {
    printf("(ip: %s, mac: %s)\n", ip_.str().c_str(),
      mac_.str().c_str());
  }

 private:
  IpAddr ip_;
  MacAddr mac_;

  // Queues holding data waiting to be received on this host.
  std::map<MacAddr, std::deque<uint8_t>> queues_;
};

class SimulatedNetwork {
 public:
  Host *HostForMac(MacAddr mac) {
    for (auto& h : hosts_)
      if (h->mac() == mac) return h.get();
    throw std::invalid_argument(absl::StrFormat("No host for MAC %s",
      mac.str()));
  }

  void AddHostWithMac(MacAddr mac) {
    auto h = std::make_unique<Host>(RandomIp(), mac);
    hosts_.push_back(std::move(h));
  }

  std::vector<std::unique_ptr<Host>>& AllHosts() { return hosts_; }

  void PrintHosts() { for (auto& h : hosts_) h->Print(); }

 private:
  std::vector<std::unique_ptr<Host>> hosts_;
};

class FakeSocket : public Socket {
 public:
  explicit FakeSocket(SimulatedNetwork *network,
    MacAddr mac, IpAddr ip) : network_(network), host_mac_(mac), host_ip_(ip) {}

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
