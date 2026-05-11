cc_binary(
    name = "vigil",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":health_handler",
        "@pulse//http:handler",
        "@pulse//http:method",
        "@pulse//http:server",
    ],
)

cc_library(
    name = "health_handler",
    srcs = ["health_handler.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "@pulse//http:handler",
        "@pulse//http:request",
        "@pulse//http:response",
    ],
)
