cc_library(
    name = "account_service",
    srcs = ["account_service.cc"],
    hdrs = ["account_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:transactions_dao",
        "@pulse//pulse/core:log",
        "@pulse//pulse/core:result",
    ],
)

cc_library(
    name = "conduit_service",
    srcs = ["conduit_service.cc"],
    hdrs = ["conduit_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:log",
        "@pulse//pulse/core:result",
        "@pulse//pulse/json:parse",
        "@pulse//pulse/json:value",
        "@pulse//pulse/process:output",
        "@pulse//pulse/process:run",
        "@pulse//pulse/strings:cat",
    ],
)

cc_library(
    name = "trade_service",
    srcs = ["trade_service.cc"],
    hdrs = ["trade_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:database",
        "//vigil/db:holding",
        "//vigil/db:holdings_dao",
        "//vigil/db:time",
        "//vigil/db:trade",
        "//vigil/db:trades_dao",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
        "@pulse//pulse/strings:cat",
    ],
)

cc_library(
    name = "transaction_service",
    srcs = ["transaction_service.cc"],
    hdrs = ["transaction_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/db:holding",
        "//vigil/db:holdings_dao",
        "//vigil/db:time",
        "//vigil/db:transaction",
        "//vigil/db:transactions_dao",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
        "@pulse//pulse/strings:cat",
    ],
)
