#include "arp.h"
#include "gtest/gtest.h"
#include "network_sim.h"
#include "absl/status/status.h"

#include <memory>

namespace orange_tcp {
namespace arp {

class ArpTest : public ::testing::Test {
 public:
 protected:
  void SetUp() {
    network_ = std::make_unique<SimulatedNetwork>();
  }

  std::unique_ptr<SimulatedNetwork> network_;
};

TEST_F(ArpTest, RequestResponse) {
  MacAddr mac1 = {
    .addr = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}
  };
  MacAddr mac2 = {
    .addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}
  };
  network_->AddHostWithMac(mac1);
  network_->AddHostWithMac(mac2);

  IpAddr ip1 = network_->AllHosts()[0]->ip();
  IpAddr ip2 = network_->AllHosts()[1]->ip();

  network_->PrintHosts();

  auto sock1 = std::make_unique<FakeSocket>(network_.get(), mac1, ip1);
  auto sock2 = std::make_unique<FakeSocket>(network_.get(), mac2, ip2);

  // TODO(jmecom) left off here. Need to actually test.
  // Broadcast out the ARP request.
  EXPECT_EQ(arp::Request(sock1.get()), absl::OkStatus());

  // Receive the request on the intended host.
  EXPECT_EQ(arp::MaybeHandleResponse(sock2.get()), absl::OkStatus());
}

}  // namespace arp
}  // namespace orange_tcp
