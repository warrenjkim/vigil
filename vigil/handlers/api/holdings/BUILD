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
