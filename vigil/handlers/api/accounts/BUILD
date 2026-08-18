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
        "@pulse//pulse/reflect:schema",
        "@pulse//pulse/strings:cat",
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

cc_library(
    name = "list_accounts_handler",
    srcs = ["list_accounts_handler.cc"],
    hdrs = ["list_accounts_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/services:account_service",
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

cc_library(
    name = "conduit_sync_handler",
    srcs = ["conduit_sync_handler.cc"],
    hdrs = ["conduit_sync_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/services:conduit_service",
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
