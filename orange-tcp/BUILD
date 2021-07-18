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

cc_library(
  name = "net",
  hdrs = ["net.h", "netint.h"],
)

cc_library(
  name = "eth",
  hdrs = ["eth.h"],
  srcs = ["eth.cc"],
  deps = [
    ":net",
    ":socket",
    ":absl",
  ]
)

cc_library(
  name = "arp",
  srcs = ["arp.cc"],
  hdrs = ["arp.h"],
  deps = [
    ":serializable_map",
    ":socket",
    ":eth",
    ":net",
    ":absl",
  ],
)

cc_library(
  name = "ip",
  srcs = ["ip.cc"],
  hdrs = ["ip.h"],
  deps = [
    ":net",
    ":socket",
    ":serializable_map",
    ":eth",
    ":arp",
    ":absl",
  ],
)

cc_library(
  name = "icmp",
  srcs = ["icmp.cc"],
  hdrs = ["icmp.h"],
  deps = [
    ":net",
    ":absl",
    ":ip",
  ],
)

cc_library(
  name = "socket",
  srcs = ["raw_socket.cc"],
  hdrs = ["socket.h"],
  deps = [
    ":net",
    ":absl",
  ],
)

cc_library(
  name = "serializable_map",
  hdrs = ["serializable_map.h"],
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

cc_test(
  name = "serializable_map_test",
  srcs = ["serializable_map_test.cc"],
  deps = [
    ":serializable_map",
    ":gtest",
    ":net",
    ":absl",
  ]
)


cc_test(
  name = "eth_test",
  srcs = ["eth_test.cc"],
  deps = [
    ":eth",
    ":gtest",
  ]
)


cc_binary(
  name = "eth_echo",
  srcs = ["eth_echo.cc"],
  deps = [
    ":net",
    ":eth",
    ":absl",
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

cc_binary(
  name = "ping",
  srcs = ["ping.cc"],
  deps = [
    ":ip",
    ":icmp",
    ":absl",
  ]
)
