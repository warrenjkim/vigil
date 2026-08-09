cc_library(
    name = "console_handler",
    hdrs = ["console_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//iris:console_css",
        "//iris:console_js",
        "@pulse//pulse/core:type_list",
        "@pulse//pulse/html:attributes",
        "@pulse//pulse/html:element",
        "@pulse//pulse/html:tags",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/http:router",
        "@pulse//pulse/strings:cat",
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
        "//iris:iris_css",
        "@pulse//pulse/http:handler",
        "@pulse//pulse/http:method",
        "@pulse//pulse/http:request",
        "@pulse//pulse/http:response",
        "@pulse//pulse/strings:string_literal",
    ],
)
