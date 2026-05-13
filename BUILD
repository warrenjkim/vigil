cc_binary(
    name = "vigil",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":handler_registry",
        ":health_handler",
        "@pulse//http:handler",
        "@pulse//http:method",
        "@pulse//http:server",
    ],
)

cc_library(
    name = "handler_registry",
    hdrs = ["handler_registry.h"],
    include_prefix = "vigil",
    strip_include_prefix = ".",
    visibility = ["//visibility:public"],
    deps = ["@pulse//http:handler"],
)

cc_library(
    name = "health_handler",
    srcs = ["health_handler.cc"],
    deps = [
        ":handler_registry",
        "@pulse//http:handler",
        "@pulse//http:request",
        "@pulse//http:response",
    ],
)

cc_test(
    name = "health_handler_test",
    srcs = ["health_handler_test.cc"],
    deps = [
        ":handler_registry",
        ":health_handler",
        "@googletest//:gtest_main",
    ],
)
