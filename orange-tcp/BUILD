load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
  name = "absl",
  deps = [
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/flags:flag",
    "@com_google_absl//absl/flags:parse",
    "@com_google_absl//absl/flags:usage",
    "@com_google_absl//absl/status",
    "@com_google_absl//absl/status:statusor",
  ]
)

cc_library(
  name = "gtest",
  deps = [
    "@com_google_googletest//:gtest_main"
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
  srcs = ["posix_socket.cc"],
  hdrs = ["socket.h"],
  deps = [
    ":net",
    ":absl",
  ],
)

cc_library(
  name = "network_sim",
  hdrs = ["network_sim.h"],
  srcs = ["network_sim.cc"],
  deps = [
    ":net",
    ":socket",
    ":absl"
  ],
  testonly = 1,
)

cc_test(
  name = "arp_test",
  srcs = ["arp_test.cc"],
  deps = [
    ":arp",
    ":network_sim",
    ":gtest",
  ]
)
