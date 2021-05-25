#include "tap.h"
#include "gtest/gtest.h"

TEST(Tap, Initializes) {
  TapDevice tap_device;
  char buf[128] = {0};
  ASSERT_EQ(tap_device.Init("tap0"), absl::OkStatus());
  EXPECT_EQ(tap_device.Write(buf, 128), 128);
  // EXPECT_EQ(tap_device.Read(buf, 1), 1);
}
