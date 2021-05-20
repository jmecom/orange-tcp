#pragma once

#include <string>
#include "absl/status/status.h"

class TapDevice {
 public:
  ~TapDevice() {
    close(fd_);
  }

  // Create a TAP device using the file at the specified `path`.
  // `path` should be in an interface in /dev/net; for example
  // /dev/net/tap/tap0.
  // Returns true upon success.
  absl::Status Create(std::string path);

  template <class T>
  int Read(T buf, size_t length);

  template <class T>
  int Write(T buf, size_t length);

 private:
  int fd_;
  std::string name_;
};
