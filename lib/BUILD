load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "openconsult",
    srcs = [
        "connection.cpp",
        "log_replay.cpp",
        "serial.posix.cpp",
    ],
    hdrs = [
        "byte_interface.h",
        "common.h",
        "connection.h",
        "log_replay.h",
        "serial.h",
    ],
    visibility = ["//visibility:public"],
)
