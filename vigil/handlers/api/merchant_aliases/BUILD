cc_library(
    name = "create_merchant_alias_handler",
    srcs = ["create_merchant_alias_handler.cc"],
    hdrs = ["create_merchant_alias_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:merchant_aliases_dao",
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
        "@pulse//pulse/reflect:schema",
    ],
)

cc_library(
    name = "list_merchant_aliases_handler",
    srcs = ["list_merchant_aliases_handler.cc"],
    hdrs = ["list_merchant_aliases_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:merchant_alias",
        "//vigil/db:merchant_aliases_dao",
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
