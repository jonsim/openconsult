cc_library(
    name = "openconsult",
    deps = [
        "consult_interface",
        "log_recorder",
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
    name = "consult_fault_codes",
    hdrs = ["consult_fault_codes.h"],
    srcs = ["consult_fault_codes.cpp"],
    deps = [
        "common",
    ],
    visibility = ["//openconsult/test:__pkg__"],
)

cc_library(
    name = "consult_engine_parameters",
    hdrs = ["consult_engine_parameters.h"],
    srcs = ["consult_engine_parameters.internal.h",
            "consult_engine_parameters.cpp"],
    deps = [
        "common",
    ],
    visibility = ["//openconsult/test:__pkg__"],
)

cc_library(
    name = "consult_interface",
    hdrs = ["consult_interface.h"],
    srcs = ["consult_interface.cpp"],
    deps = [
        "byte_interface",
        "common",
        "consult_engine_parameters",
        "consult_fault_codes",
    ],
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
    name = "log_recorder",
    hdrs = ["log_recorder.h"],
    srcs = ["log_recorder.cpp"],
    deps = [
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
