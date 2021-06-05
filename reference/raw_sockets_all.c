// https://squidarth.com/networking/systems/rc/2018/05/28/using-raw-sockets.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>

void hexdump(unsigned char *buf, int len) {
  for (int i = 0; i < len; i++) printf("%02x", buf[i]);
  printf("\n");
}

int main(void) {
  // Structs that contain source IP addresses
  struct sockaddr_in source_socket_address, dest_socket_address;

  int packet_size;

  // Allocate string buffer to hold incoming packet data
  unsigned char *buffer = (unsigned char *) malloc(65536);
  // Open the raw socket
  int sock = socket(AF_PACKET, SOCK_RAW, ETH_P_ALL);
  if (sock == -1) {
    // socket creation failed, may be because of non-root privileges
    perror("Failed to create socket");
    exit(1);
  }

  while (1) {
    // recvfrom is used to read data from a socket
    packet_size = recvfrom(sock, buffer, 65536, 0, NULL, NULL);
    if (packet_size == -1) {
      printf("Failed to get packets\n");
      return 1;
    }

    hexdump(buffer, packet_size);
  }

  return 0;
}
