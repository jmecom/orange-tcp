load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
  name = "absl",
  deps = [
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
  ]
)

cc_binary(
  name = "arping",
  srcs = ["arping.cc"],
  deps = [
    ":arp",
    ":absl",
  ]
)

cc_library(
  name = "net",
  hdrs = ["net.h", "netint.h"],
)

cc_library(
  name = "arp",
  srcs = ["arp.cc"],
  hdrs = ["arp.h"],
  deps = [
    ":socket",
    ":net",
    ":absl",
  ],
)

cc_library(
  name = "socket",
  srcs = ["socket.cc"],
  hdrs = ["socket.h"],
  deps = [
    ":net",
    ":absl",
  ],
)
