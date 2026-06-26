cc_library(
    name = "handlers",
    visibility = ["//visibility:public"],
    deps = [
        ":create_account_handler",
        ":create_trade_handler",
        ":create_transaction_handler",
        ":delete_account_handler",
        ":get_account_handler",
        ":get_holding_handler",
        ":get_new_account_handler",
        ":health_handler",
        ":list_accounts_handler",
        ":list_holdings_handler",
        ":list_trades_handler",
        ":list_transactions_handler",
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
    name = "get_new_account_handler",
    srcs = ["get_new_account_handler.cc"],
    hdrs = ["get_new_account_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
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
    name = "get_holding_handler",
    srcs = ["get_holding_handler.cc"],
    hdrs = ["get_holding_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:holding",
        "//vigil/db:holdings_dao",
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
    name = "get_holding_handler_test",
    srcs = ["get_holding_handler_test.cc"],
    deps = [
        ":get_holding_handler",
        "//vigil:trade_service",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/db:holdings_dao",
        "//vigil/db:trade",
        "//vigil/db:trades_dao",
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
    name = "create_trade_handler",
    srcs = ["create_trade_handler.cc"],
    hdrs = ["create_trade_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil:trade_service",
        "//vigil/db:trade",
        "@pulse//pulse/core:error",
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
    name = "create_trade_handler_test",
    srcs = ["create_trade_handler_test.cc"],
    deps = [
        ":create_trade_handler",
        "//vigil:trade_service",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/db:holdings_dao",
        "//vigil/db:trades_dao",
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

cc_library(
    name = "list_holdings_handler",
    srcs = ["list_holdings_handler.cc"],
    hdrs = ["list_holdings_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:holding",
        "//vigil/db:holdings_dao",
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
    name = "list_holdings_handler_test",
    srcs = ["list_holdings_handler_test.cc"],
    deps = [
        ":list_holdings_handler",
        "//vigil:trade_service",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/db:holdings_dao",
        "//vigil/db:trade",
        "//vigil/db:trades_dao",
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
    name = "list_trades_handler",
    srcs = ["list_trades_handler.cc"],
    hdrs = ["list_trades_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:trade",
        "//vigil/db:trades_dao",
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
    name = "list_trades_handler_test",
    srcs = ["list_trades_handler_test.cc"],
    deps = [
        ":list_trades_handler",
        "//vigil:trade_service",
        "//vigil/db:account",
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/db:holdings_dao",
        "//vigil/db:trade",
        "//vigil/db:trades_dao",
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
    name = "list_transactions_handler",
    srcs = ["list_transactions_handler.cc"],
    hdrs = ["list_transactions_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:transaction",
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
    name = "list_transactions_handler_test",
    srcs = ["list_transactions_handler_test.cc"],
    deps = [
        ":list_transactions_handler",
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
