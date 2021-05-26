load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "orange-tcp",
    srcs = ["arp.cc",
    ],
    hdrs = ["arp.h"],
    deps = [
        "@com_google_absl//absl/status",
    ],
)


cc_library(
    name = "socket",
    srcs = ["socket.cc"],
    hdrs = ["socket.h"],
    deps = [
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings"
    ],
)
