#include "serializable_map.h"
#include "gtest/gtest.h"
#include "net.h"

#include <fstream>

namespace orange_tcp {

static IpAddr kIp1 = { .addr = 0xC0A86301 };
static IpAddr kIp2 = { .addr = 0xC0A86302 };
static MacAddr kMac1 = { .addr = {0x0a, 0x00, 0x27, 0x00, 0x00, 0x01}};
static MacAddr kMac2 = { .addr = {0x0a, 0x00, 0x27, 0x00, 0x00, 0x02}};

TEST(SerializableMap, SerializeToDisk) {
  serializable_map<IpAddr, MacAddr> cache;
  serializable_map<IpAddr, MacAddr> cache2;

  cache[kIp1] = kMac1;
  cache[kIp2] = kMac2;

  std::vector<char> buffer = cache.serialize();
  std::ofstream outfile("/tmp/arp_cache",
    std::ios::trunc);

  outfile.write(buffer.data(), buffer.size());
  outfile.close();

  std::ifstream infile("/tmp/arp_cache");
  size_t size = buffer.size();
  std::vector<char> inbuffer(size);
  infile.read(inbuffer.data(), inbuffer.size());

  cache2.deserialize(inbuffer);

  EXPECT_EQ(cache, cache2);
}

}  // namespace orange_tcp
