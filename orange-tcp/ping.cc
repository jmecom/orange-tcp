#include "socket.h"
#include <memory>

#include "ip.h"
#include "icmp.h"

#include "absl/strings/str_format.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

ABSL_FLAG(std::string, ip, "",
  "Destination IP address (numbers-and-dots notation)");
ABSL_FLAG(int, num_requests, 1,
  "How many times to issue the same request");

namespace orange_tcp {
namespace ping {

int Ping(Socket *sock, const IpAddr& ip) {
  icmp::EchoBody request = icmp::MakeEchoRequest();
  auto status = ip::SendDatagram(sock, ip,
    reinterpret_cast<uint8_t *>(&request), sizeof(request),
    ip::Protocol::icmp);
  if (!status.ok()) {
    printf("[ping] Failed to send");
    return 1;
  }

  // int saddr_size, data_size;
	// struct sockaddr saddr;

	// unsigned char *buffer = (unsigned char *)malloc(65536); //Its Big!

	// int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	// // int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP | ETH_P_ARP));

	// if (sock_raw < 0)
	// {
	// 	//Print the error with proper message
	// 	perror("Socket Error");
	// 	return 1;
	// }
	// while (1)
	// {
	// 	saddr_size = sizeof saddr;
	// 	//Receive a packet
	// 	data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_size);
  //   DumpHex(buffer, data_size);
  // // auto *response = reinterpret_cast<icmp::EchoBody *>(response_buffer.data());
  // }

  std::vector<uint8_t> response_buffer;
  status = ip::RecvDatagram(sock, &response_buffer, sizeof(request));
  if (!status.ok()) {
    printf("[ping] Failed to recv");
    return 1;
  }

  auto *response = reinterpret_cast<icmp::EchoBody *>(response_buffer.data());

  return 0;
}

int Main() {
  std::string ip_str = absl::GetFlag(FLAGS_ip);
  if (ip_str.empty()) {
    printf("[ping] must set ip address\n");
    return -1;
  }
  const IpAddr ip = IpAddr::FromString(ip_str);
  auto socket = RawSocket::CreateOrDie();

  int ret = 0;
  for (int i = 0; i < absl::GetFlag(FLAGS_num_requests); i++) {
    ret = Ping(socket.get(), ip);
    if (ret != 0) return ret;
    sleep(1);
  }

  return 0;
}

}  // namespace ping
}  // namespace orange_tcp

int main(int argc, char **argv) {
  absl::SetProgramUsageMessage("Issue ICMP echo requests");
  absl::ParseCommandLine(argc, argv);
  return orange_tcp::ping::Main();
}
