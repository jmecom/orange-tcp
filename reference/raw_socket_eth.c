#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <string.h>
#include <stdio.h>

// https://stackoverflow.com/questions/21411851/how-to-send-data-over-a-raw-ethernet-socket-using-sendto-without-using-sockaddr
struct ethernet_msg
{
  char destination_mac[6];
  char source_mac[6];
  char transport_protocol[2];
  char data1[6];
  char data2[6];
  char data3[8];
  char data4[8];
  char data5[8];
  char data6[2];
  char data7[2];
  char data8[6];
  char data9[4];
  char data10[6];
};
struct ethernet_msg my_msg = {
    {0x91, 0xe0, 0xf0, 0x01, 0x00, 0x00},             //destination_mac
    {0x08, 0x00, 0x27, 0x90, 0x5f, 0xae},             //source_mac
    {0x22, 0xf0},                                     //transport_protocol
    {0xfc, 0x06, 0x00, 0x2c, 0x00, 0x00},             //data1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},             //data2
    {0x08, 0x00, 0x27, 0xff, 0xfe, 0x90, 0x5f, 0xae}, //data3
    {0xd8, 0x80, 0x39, 0xff, 0xfe, 0xd0, 0xac, 0xb5}, //data4
    {0xd8, 0x80, 0x39, 0xff, 0xfe, 0xd0, 0x9b, 0xc8}, //data5
    {0x00, 0x00},                                     //data6
    {0x00, 0x00},                                     //data7
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},             //data8
    {0x00, 0x00, 0x00, 0x5f},                         //data9
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}              //data10
};

char foo[4] = {1,2,3,4};

void get_interface_MAC(char interface_name[IFNAMSIZ], int sockfd)
{
  struct ifreq if_mac;
  memset(&if_mac, 0, sizeof(struct ifreq));
  strncpy(if_mac.ifr_name, interface_name, IFNAMSIZ - 1);
  if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
    perror("SIOCGIFHWADDR");
}

int get_interface_index(char interface_name[IFNAMSIZ], int sockfd)
{
  struct ifreq if_idx;
  memset(&if_idx, 0, sizeof(struct ifreq));
  strncpy(if_idx.ifr_name, interface_name, IFNAMSIZ - 1);
  if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
    perror("SIOCGIFINDEX");

  return if_idx.ifr_ifindex;
}

int test(void)
{
  int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (sockfd < 0)
    return 1;

  struct sockaddr_ll socket_address;
  char interface_name[IFNAMSIZ] = "eth0";

  socket_address.sll_ifindex = get_interface_index(interface_name, sockfd);
  /* Address length*/
  socket_address.sll_halen = ETH_ALEN;
  /* Destination MAC */
  memcpy(socket_address.sll_addr, my_msg.destination_mac, ETH_ALEN);

  if (sendto(sockfd, &my_msg, sizeof(my_msg), 0,
            (struct sockaddr *) &socket_address,
             sizeof(struct sockaddr_ll)) < 0)
  {
    printf("Fail\n");
    return 2;
  }

  return 0;
}

int main(void)
{
  return test();
}