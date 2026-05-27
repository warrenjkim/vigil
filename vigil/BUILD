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
        "//vigil/handlers:handler_registry",
        "//vigil/handlers:health_handler",
        "@pulse//core:result_or_die",
        "@pulse//http:handler",
        "@pulse//http:method",
        "@pulse//http:router",
        "@pulse//http:server",
    ],
)
