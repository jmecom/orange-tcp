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

absl::Status TapDevice::Create(std::string path) {
    struct ifreq ifr = {0};

    fd_ = open("/dev/net/tap", O_RDWR);
    if (fd_ < 0) {
        return absl::InternalError("Cannot open TUN/TAP device.");
    }

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, path.c_str(), IFNAMSIZ - 1);

    if (ioctl(fd_, TUNSETIFF, reinterpret_cast<void *>(&ifr)) < 0) {
        printf("ERR: Could not ioctl tun: %s\n", strerror(errno));
        close(fd_);
        return absl::InternalError("ioctl TUNSETIFF failed.");
    }

    name_ = std::string(ifr.ifr_name);
    return absl::OkStatus();
}
