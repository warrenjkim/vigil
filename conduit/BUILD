load("@rules_go//go:def.bzl", "go_binary")

go_binary(
    name = "conduit",
    srcs = ["main.go"],
    data = ["plaid-config.json"],
    visibility = ["//visibility:public"],
)
