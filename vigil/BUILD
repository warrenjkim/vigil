cc_binary(
    name = "vigil",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//vigil/db:accounts_dao",
        "//vigil/db:database",
        "//vigil/handlers:create_account_handler",
        "//vigil/handlers:delete_account_handler",
        "//vigil/handlers:get_account_handler",
        "//vigil/handlers:health_handler",
        "@pulse//pulse/core:result_or_die",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:router",
        "@pulse//pulse/http:server",
    ],
)
