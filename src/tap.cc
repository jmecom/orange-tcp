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

bool TapDevice::Create(std::string path) {
    struct ifreq ifr = {0};

    fd_ = open("/dev/net/tap", O_RDWR);
    if (fd_ < 0) {
        printf("Cannot open TUN/TAP dev\n");
        return false;
    }

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, path.c_str(), IFNAMSIZ - 1);

    if (ioctl(fd_, TUNSETIFF, reinterpret_cast<void *>(&ifr)) < 0) {
        printf("ERR: Could not ioctl tun: %s\n", strerror(errno));
        close(fd_);
        return false;
    }

    name_ = std::string(ifr.ifr_name);
    return true;
}
