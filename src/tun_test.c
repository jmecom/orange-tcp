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

int tun_alloc(char *dev) {
    struct ifreq ifr = {0};
    int fd, err;

    if ((fd = open("/dev/net/tap", O_RDWR)) < 0) {
        printf("Cannot open TUN/TAP dev\n");
        exit(1);
    }

    // TODO(jmecom) Needed?
    // CLEAR(ifr);

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        printf("ERR: Could not ioctl tun: %s\n", strerror(errno));
        close(fd);
        return err;
    }

    strncpy(dev, ifr.ifr_name, IFNAMSIZ - 1);
    return fd;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <name of interface in /dev/net>", argv[0]);
        return 1;
    }

    char if_name[IFNAMSIZ] = {0};
    strncpy(if_name, argv[1], IFNAMSIZ - 1);
    int fd = tun_alloc(if_name);

    printf("fd = %d\n", fd);
    return 0;
}
