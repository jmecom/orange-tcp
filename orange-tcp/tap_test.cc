#include "tap.h"
#include "gtest/gtest.h"

TEST(Tap, Initializes) {
  TapDevice tap_device;
  char buf[128] = {0};
  EXPECT_EQ(tap_device.Init("tap0"), absl::OkStatus());
  printf("%d\n", tap_device.Write(buf, 128));
}
