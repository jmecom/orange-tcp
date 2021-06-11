#pragma once

#include <stdint.h>
#include <arpa/inet.h>

// https://stackoverflow.com/questions/6732127/is-there-a-way-to-enforce-specific-endianness-for-a-c-or-c-struct
class uint16_net {
 public:
  uint16_net() : be_val_(0) {}
  explicit uint16_net(const uint16_t &val) : be_val_(htons(val)) {}
  operator uint16_t() const { return ntohs(be_val_); }
  void operator = (uint16_t val) { be_val_ = htons(val); }
 private:
  uint16_t be_val_;
} __attribute__((packed));

class uint32_net {
 public:
  uint32_net() : be_val_(0) {}
  explicit uint32_net(const uint32_t &val) : be_val_(htonl(val)) {}
  operator uint32_t() const { return ntohl(be_val_); }
 private:
  uint16_t be_val_;
} __attribute__((packed));
