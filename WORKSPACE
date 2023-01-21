load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Used by //openconsult/test:*
http_archive(
  name = "gtest",
  urls = ["https://github.com/google/googletest/archive/release-1.12.1.zip"],
  strip_prefix = "googletest-release-1.12.1",
)

# Used solely by //cli/src:openconsult_cli
http_archive(
  name = "bazel_skylib",
  urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz"],
  sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
)
http_archive(
  name = "com_google_absl",
  urls = ["https://github.com/abseil/abseil-cpp/archive/20220623.1.zip"],
  strip_prefix = "abseil-cpp-20220623.1",
)
