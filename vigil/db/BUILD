exports_files(["schema.sql"])

cc_library(
    name = "database",
    hdrs = ["database.h"],
    data = [":schema.sql"],
    deps = [
        ":sqlite_traits",
        "@pulse//dsa:error",
        "@pulse//dsa:result",
        "@sqlite3",
    ],
)

cc_test(
    name = "database_test",
    srcs = ["database_test.cc"],
    deps = [
        ":database",
        "@googletest//:gtest_main",
        "@pulse//dsa:error",
        "@pulse//dsa:result",
        "@sqlite3",
    ],
)

cc_library(
    name = "sqlite_traits",
    hdrs = ["sqlite_traits.h"],
    deps = [
        "@sqlite3",
    ],
)

cc_library(
    name = "account",
    hdrs = ["account.h"],
    deps = [
        "@pulse//core:enum_macros",
        "@pulse//core:stringify",
        "@pulse//dsa:error",
        "@pulse//dsa:result",
    ],
)

cc_library(
    name = "accounts_dao",
    srcs = ["accounts_dao.cc"],
    hdrs = ["accounts_dao.h"],
    deps = [
        ":account",
        ":database",
        "@pulse//core:stringify",
        "@pulse//dsa:error",
        "@pulse//dsa:result",
    ],
)

cc_test(
    name = "accounts_dao_test",
    srcs = ["accounts_dao_test.cc"],
    deps = [
        ":account",
        ":accounts_dao",
        ":database",
        "@googletest//:gtest_main",
        "@pulse//dsa:error",
        "@sqlite3",
    ],
)
