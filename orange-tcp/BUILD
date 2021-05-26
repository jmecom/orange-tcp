load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
  name = "status_macros",
  hdrs = ["status_macros.h"],
  deps = [
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
  ]
)

cc_library(
  name = "arp",
  srcs = ["arp.cc"],
  hdrs = ["arp.h"],
  deps = [
    "@com_google_absl//absl/status",
    ":socket",
    ":status_macros",
  ],
)

cc_library(
  name = "socket",
  srcs = ["socket.cc"],
  hdrs = ["socket.h"],
  deps = [
    "@com_google_absl//absl/status:statusor",
    "@com_google_absl//absl/strings",
  ],
)
