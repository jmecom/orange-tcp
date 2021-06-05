#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <linux/filter.h> // CHANGE: include lsf

struct ethernet {
    unsigned char dest[6];
    unsigned char source[6];
    uint16_t eth_type;
};

struct arp {
    uint16_t htype;
    uint16_t ptype;
    unsigned char hlen;
    unsigned char plen;
    uint16_t oper;
    /* addresses */
    unsigned char sender_ha[6];
    unsigned char sender_pa[4];
    unsigned char target_ha[6];
    unsigned char target_pa[4];
};

#define ETH_HDR_LEN 14
#define BUFF_SIZE 2048

/* CHANGE
   Linux socket filters use the Berkeley packet filter syntax.
   This was adapted from BSDs "man 4 bpf" example for RARP.
*/
struct sock_filter arpfilter[] = {
    BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12), /* Skip 12 bytes */
    BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ETH_P_ARP, 0, 1), /* if eth type != ARP
                                                         skip next instr. */
    BPF_STMT(BPF_RET+BPF_K, sizeof(struct arp) +
                 sizeof(struct ethernet)),
    BPF_STMT(BPF_RET+BPF_K, 0), /* Return, either the ARP packet or nil */
};

static void dump_arp(struct arp *arp_hdr);

int main(void)
{
    int sock;
    void *buffer = NULL;
    ssize_t recvd_size;
    struct ethernet *eth_hdr = NULL;
    struct arp *arp_hdr = NULL;
    struct sock_filter *filter;
    struct sock_fprog  fprog;

    if( (sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
    {
        perror("socket(): ");
        exit(-1);
    }

    /* CHANGE prepare linux packet filter */
    if ((filter = malloc(sizeof(arpfilter))) == NULL) {
        perror("malloc");
        close(sock);
        exit(1);
    }
    memcpy(filter, &arpfilter, sizeof(arpfilter));
    fprog.filter = filter;
    fprog.len = sizeof(arpfilter)/sizeof(struct sock_filter);

    /* CHANGE add filter */
    if (setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &fprog, sizeof(fprog)) == -1) {
        perror("setsockopt");
        close(sock);
        exit(1);
    }

    buffer = malloc(BUFF_SIZE);
    while(1)
    {
        if( (recvd_size = recv(sock, buffer, BUFF_SIZE, 0)) < 0)
        {
            perror("recv(): ");
            free(buffer);
            close(sock);
            exit(-1);
        }
        if((size_t)recvd_size < (sizeof(struct ethernet) + sizeof(struct arp)))
        {
            printf("Short packet. Packet len: %ld\n", recvd_size);
            continue;
        }
        eth_hdr = (struct ethernet *)buffer;
        if(ntohs(eth_hdr->eth_type) != ETH_P_ARP) {
            printf("Received wrong ethernet type: %X\n", eth_hdr->eth_type);
            exit(1);
        }
        arp_hdr = (struct arp *)(buffer+ETH_HDR_LEN);
        dump_arp(arp_hdr);
    }
    free(buffer);
    close(sock);
}

static void
dump_arp(struct arp *arp_hdr)
{
    uint16_t htype = ntohs(arp_hdr->htype);
    uint16_t ptype = ntohs(arp_hdr->ptype);
    uint16_t oper = ntohs(arp_hdr->oper);
    switch(htype)
    {
        case 0x0001:
            printf("ARP HTYPE: Ethernet(0x%04X)\n", htype);
            break;
        default:
            printf("ARP HYPE: 0x%04X\n", htype);
            break;
    }
    switch(ptype)
    {
        case 0x0800:
            printf("ARP PTYPE: IPv4(0x%04X)\n", ptype);
            break;
        default:
            printf("ARP PTYPE: 0x%04X\n", ptype);
            break;
    }
    printf("ARP HLEN: %d\n", arp_hdr->hlen);
    printf("ARP PLEN: %d\n", arp_hdr->plen);
    switch(oper)
    {
        case 0x0001:
            printf("ARP OPER: Request(0x%04X)\n", oper);
            break;
        case 0x0002:
            printf("ARP OPER: Response(0x%04X)\n", oper);
            break;
        default:
            printf("ARP OPER: 0x%04X\n", oper);
            break;
    }
    printf("ARP Sender HA: %02X:%02X:%02X:%02X:%02X:%02X\n",
           arp_hdr->sender_ha[0],arp_hdr->sender_ha[1],arp_hdr->sender_ha[2],
           arp_hdr->sender_ha[3], arp_hdr->sender_ha[4], arp_hdr->sender_ha[5]);
    printf("ARP Sender PA: %d.%d.%d.%d\n", arp_hdr->sender_pa[0],
           arp_hdr->sender_pa[1], arp_hdr->sender_pa[2], arp_hdr->sender_pa[3]);
    printf("ARP Target HA: %02X:%02X:%02X:%02X:%02X:%02X\n",
           arp_hdr->target_ha[0],arp_hdr->target_ha[1],arp_hdr->target_ha[2],
           arp_hdr->target_ha[3], arp_hdr->target_ha[4], arp_hdr->target_ha[5]);
    printf("ARP Target PA: %d.%d.%d.%d\n", arp_hdr->target_pa[0],
           arp_hdr->target_pa[1], arp_hdr->target_pa[2], arp_hdr->target_pa[3]);
    printf("ARP DONE =====================\n");
}
