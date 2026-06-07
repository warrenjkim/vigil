cc_library(
    name = "handlers",
    visibility = ["//visibility:public"],
    deps = [
        ":create_account_handler",
        ":delete_account_handler",
        ":get_account_handler",
        ":health_handler",
    ],
)

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
    name = "create_account_handler",
    srcs = ["create_account_handler.cc"],
    hdrs = ["create_account_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "@pulse//pulse/core:log",
        "@pulse//pulse/core:result",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
    ],
)

cc_test(
    name = "create_account_handler_test",
    srcs = ["create_account_handler_test.cc"],
    deps = [
        ":create_account_handler",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:result_or_die",
        "@pulse//pulse/core:stringify",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/http:router",
    ],
)

cc_library(
    name = "get_account_handler",
    srcs = ["get_account_handler.cc"],
    hdrs = ["get_account_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "@pulse//pulse/core:log",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:stringify",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
    ],
)

cc_test(
    name = "get_account_handler_test",
    srcs = ["get_account_handler_test.cc"],
    deps = [
        ":get_account_handler",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:result_or_die",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/http:router",
    ],
)

cc_library(
    name = "delete_account_handler",
    srcs = ["delete_account_handler.cc"],
    hdrs = ["delete_account_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:accounts_dao",
        "@pulse//pulse/core:log",
        "@pulse//pulse/core:result",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
    ],
)

cc_test(
    name = "delete_account_handler_test",
    srcs = ["delete_account_handler_test.cc"],
    deps = [
        ":delete_account_handler",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:result_or_die",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/http:router",
    ],
)

cc_library(
    name = "create_transaction_handler",
    srcs = ["create_transaction_handler.cc"],
    hdrs = ["create_transaction_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:transaction",
        "//vigil/db:transactions_dao",
        "@pulse//pulse/core:log",
        "@pulse//pulse/core:result",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/json:bind",
        "@pulse//pulse/json:parse",
        "@pulse//pulse/json:value",
    ],
)

cc_test(
    name = "create_transaction_handler_test",
    srcs = ["create_transaction_handler_test.cc"],
    deps = [
        ":create_transaction_handler",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/db:transaction",
        "//vigil/db:transactions_dao",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:result_or_die",
        "@pulse//pulse/core:stringify",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/http:router",
    ],
)
