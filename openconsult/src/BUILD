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

cc_library(
    name = "byte_interface",
    hdrs = ["byte_interface.h"],
    visibility = ["//lib/test:__pkg__"],
)

cc_library(
    name = "log_replay",
    srcs = ["log_replay.cpp"],
    hdrs = ["log_replay.h", "common.h"],
    visibility = ["//lib/test:__pkg__"],
    deps = ["byte_interface"]
)
