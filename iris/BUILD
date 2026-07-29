load(":defs.bzl", "bundle")
load(":embed.bzl", "embed_file")

[
    embed_file(
        name = f.replace("/", "_").replace(".", "_"),
        src = f,
    )
    for f in glob(
        include = [
            "**/*.html",
            "**/*.js",
            "**/*.css",
        ],
        allow_empty = True,
        exclude = [],
    )
]

filegroup(
    name = "common_js",
    srcs = [
        "api.js",
        "domain.js",
        "format.js",
    ],
    visibility = ["//visibility:public"],
)

bundle(
    name = "index",
    html = "index.html",
)

bundle(
    name = "dashboard",
    css = "iris.css",
    html = "dashboard.html",
    js = [
        "dashboard.js",
        ":common_js",
    ],
)

bundle(
    name = "accounts_detail",
    css = "iris.css",
    html = "accounts/detail.html",
    js = [
        "accounts/detail.js",
        ":common_js",
    ],
)

bundle(
    name = "accounts_new",
    html = "accounts/new.html",
)

bundle(
    name = "console",
    css = "console.css",
    js = "console.js",
)
