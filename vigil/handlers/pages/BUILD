cc_library(
    name = "health_handler",
    srcs = ["health_handler.cc"],
    hdrs = ["health_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
    ],
)

cc_test(
    name = "health_handler_test",
    srcs = ["health_handler_test.cc"],
    deps = [
        ":health_handler",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:result",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/http:router",
    ],
)

cc_library(
    name = "index_handler",
    srcs = ["index_handler.cc"],
    hdrs = ["index_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//iris:iris_html",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
    ],
)

cc_library(
    name = "get_new_account_handler",
    srcs = ["get_new_account_handler.cc"],
    hdrs = ["get_new_account_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//iris:accounts_new_html",
        "@pulse//pulse/html:attributes",
        "@pulse//pulse/html:element",
        "@pulse//pulse/html:tags",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
    ],
)

cc_test(
    name = "get_new_account_handler_test",
    srcs = ["get_new_account_handler_test.cc"],
    deps = [
        ":get_new_account_handler",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
    ],
)
