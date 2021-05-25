#pragma once

#include <string>
#include "absl/status/status.h"

class TapDevice {
 public:
  ~TapDevice() {
    close(fd_);
  }

  // Create a TAP device using the file at the specified `path`.
  // After the TAP device is created, an interface will be created
  // and assigned the supplied `address`.
  absl::Status Init(std::string path,
                    std::string address = "10.0.0.1",
                    std::string route = "10.0.0.0/24");

  // Blocking.
  int Read(void *buf, size_t length);
  int Write(void *buf, size_t length);

 private:
  int fd_;
  std::string name_;
};
