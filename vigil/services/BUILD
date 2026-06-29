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

cc_test(
    name = "trade_service_test",
    srcs = ["trade_service_test.cc"],
    deps = [
        ":trade_service",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/db:holding",
        "//vigil/db:holdings_dao",
        "//vigil/db:time",
        "//vigil/db:trade",
        "//vigil/db:trades_dao",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:result_or_die",
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

cc_test(
    name = "transaction_service_test",
    srcs = ["transaction_service_test.cc"],
    deps = [
        ":transaction_service",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/db:holding",
        "//vigil/db:holdings_dao",
        "//vigil/db:time",
        "//vigil/db:transaction",
        "//vigil/db:transactions_dao",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:result_or_die",
    ],
)
