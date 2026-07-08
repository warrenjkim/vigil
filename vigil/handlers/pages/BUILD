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

cc_library(
    name = "static_handler",
    hdrs = ["static_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//iris:accounts_detail_html",
        "//iris:accounts_detail_js",
        "//iris:accounts_new_html",
        "//iris:api_js",
        "//iris:dashboard_html",
        "//iris:dashboard_js",
        "//iris:domain_js",
        "//iris:format_js",
        "//iris:index_html",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/strings:string_literal",
    ],
)
