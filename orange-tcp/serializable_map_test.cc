#include "serializable_map.h"
#include "gtest/gtest.h"
#include "net.h"

#include <fstream>

namespace orange_tcp {

TEST(SerializableMap, SerializeToDisk) {
  serializable_map<IpAddr, MacAddr> cache;
  serializable_map<IpAddr, MacAddr> cache2;

  IpAddr ip;
  ip.addr = 0xC0A86301;
  MacAddr mac;
  uint8_t m[] = {0x0a, 0x00, 0x27, 0x00, 0x00, 0x00};
  memcpy(mac.addr, m, 6);

  cache[ip] = mac;

  std::vector<char> buffer = cache.serialize();
  std::ofstream outfile("/tmp/arp_cache",
    std::ios::trunc);

  outfile.write(buffer.data(), buffer.size());
  outfile.close();

  std::ifstream infile("/tmp/arp_cache");
  std::vector<char> inbuffer(32);
  infile.read(inbuffer.data(), inbuffer.size());

  // printf("inbuffer: %s\n", inbuffer.data());
  cache2.deserialize(inbuffer);

  EXPECT_EQ(cache, cache2);
}

}  // namespace orange_tcp
