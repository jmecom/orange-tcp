#pragma once

#include <string>

class TapDevice {
 public:
  // Create a TAP device using the file at the specified `path`.
  // `path` should be in an interface in /dev/net; for example
  // /dev/net/tap/tap0.
  // Returns true upon success.
  bool Create(std::string path);
 private:
  int fd_;
  std::string name_;
};
