load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "tap",
    srcs = ["tap.cc"],
    hdrs = ["tap.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings"
    ],
)

cc_test(
    name = "tap_test",
    srcs = ["tap_test.cc"],
    deps = [
        ":tap",
        "@com_google_googletest//:gtest_main",
    ]
)
