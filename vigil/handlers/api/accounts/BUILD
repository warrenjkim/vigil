cc_library(
    name = "create_account_handler",
    srcs = ["create_account_handler.cc"],
    hdrs = ["create_account_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:log",
        "@pulse//pulse/core:result",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:parameters",
        "@pulse//pulse/http:parse_form",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/json:bind",
        "@pulse//pulse/json:parse",
        "@pulse//pulse/json:value",
        "@pulse//pulse/strings:cat",
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
    name = "get_account_handler",
    srcs = ["get_account_handler.cc"],
    hdrs = ["get_account_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:transactions_dao",
        "@pulse//pulse/core:log",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:stringify",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/json:value",
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
        "//vigil/db:transaction",
        "//vigil/db:transactions_dao",
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
    name = "list_accounts_handler",
    srcs = ["list_accounts_handler.cc"],
    hdrs = ["list_accounts_handler.h"],
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
        "@pulse//pulse/json:value",
    ],
)

cc_test(
    name = "list_accounts_handler_test",
    srcs = ["list_accounts_handler_test.cc"],
    deps = [
        ":list_accounts_handler",
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
