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
