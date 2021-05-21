#include "tap.h"

#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "absl/strings/str_format.h"

absl::Status TapDevice::Init(std::string name, std::string address,
                             std::string route) {
  // First, create tap device.
  struct ifreq ifr = {0};

  fd_ = open("/dev/net/tap", O_RDWR);
  if (fd_ < 0) {
    return absl::InternalError("Cannot open TUN/TAP device.");
  }

  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);

  if (ioctl(fd_, TUNSETIFF, reinterpret_cast<void *>(&ifr)) < 0) {
    close(fd_);
    return absl::InternalError("ioctl TUNSETIFF failed.");
  }

  name_ = std::string(ifr.ifr_name);

  // Bring up the interface.
  int ret = system(absl::StrFormat("ip link set dev %s up", name_).c_str());
  if (ret != 0) return absl::InternalError("Failed to set interface");

  // Add a new route.
  ret = system(absl::StrFormat("ip route add dev %s %s", name_, route).c_str());
  if (ret != 0) return absl::InternalError("Failed to add route");

  // Assign an IP address.
  ret = system(absl::StrFormat("ip address add dev %s local %s", name_,
              address).c_str());
  if (ret != 0) return absl::InternalError("Failed to set adress");

  return absl::OkStatus();
}

template <class T>
int TapDevice::Read(T buf, size_t length) {
  return read(fd_, buf, length);
}

template <class T>
int TapDevice::Write(T buf, size_t length) {
  return write(fd_, buf, length);
}
