cc_library(
    name = "handlers",
    visibility = ["//visibility:public"],
    deps = [
        ":create_account_handler",
        ":delete_account_handler",
        ":get_account_handler",
        ":handler_registry",
        ":health_handler",
    ],
)

cc_library(
    name = "handler_registry",
    hdrs = ["handler_registry.h"],
    visibility = ["//visibility:public"],
    deps = ["@pulse//http:handler"],
)

cc_library(
    name = "health_handler",
    srcs = ["health_handler.cc"],
    visibility = ["//visibility:public"],
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
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//http:handler",
        "@pulse//http:request",
        "@pulse//http:response",
    ],
)

cc_library(
    name = "get_account_handler",
    srcs = ["get_account_handler.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":handler_registry",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "@pulse//core:log",
        "@pulse//core:result",
        "@pulse//core:stringify",
        "@pulse//http:handler",
        "@pulse//http:request",
        "@pulse//http:response",
    ],
)

cc_test(
    name = "get_account_handler_test",
    srcs = ["get_account_handler_test.cc"],
    deps = [
        ":get_account_handler",
        ":handler_registry",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//core:result_or_die",
        "@pulse//http:handler",
        "@pulse//http:method",
        "@pulse//http:request",
        "@pulse//http:response",
    ],
)

cc_library(
    name = "insert_account_handler",
    srcs = ["insert_account_handler.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":handler_registry",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "@pulse//core:log",
        "@pulse//core:result",
        "@pulse//http:handler",
        "@pulse//http:request",
        "@pulse//http:response",
    ],
)

cc_test(
    name = "insert_account_handler_test",
    srcs = ["insert_account_handler_test.cc"],
    deps = [
        ":handler_registry",
        ":insert_account_handler",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//core:result",
        "@pulse//core:result_or_die",
        "@pulse//core:stringify",
        "@pulse//http:handler",
        "@pulse//http:method",
        "@pulse//http:request",
        "@pulse//http:response",
    ],
)
