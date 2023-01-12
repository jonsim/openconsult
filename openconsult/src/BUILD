cc_library(
    name = "openconsult",
    deps = [
        "byte_interface",
        "common",
        "connection",
        "log_replay",
        "serial.posix",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "byte_interface",
    hdrs = ["byte_interface.h"],
    visibility = ["//openconsult/test:__pkg__"],
)

cc_library(
    name = "common",
    hdrs = ["common.h"],
    visibility = ["//openconsult/test:__pkg__"],
)

cc_library(
    name = "connection",
    hdrs = ["connection.h"],
    srcs = ["connection.cpp"],
    visibility = ["//openconsult/test:__pkg__"],
)

cc_library(
    name = "log_replay",
    hdrs = ["log_replay.h"],
    srcs = ["log_replay.cpp"],
    deps = [
        "byte_interface",
        "common",
    ],
    visibility = ["//openconsult/test:__pkg__"],
)

cc_library(
    name = "serial.posix",
    hdrs = ["serial.h"],
    srcs = ["serial.posix.cpp"],
    deps = [
        "byte_interface",
        "common",
    ],
    visibility = ["//openconsult/test:__pkg__"],
)
