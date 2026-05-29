genrule(
    name = "schema_sql_h",
    srcs = ["schema.sql"],
    outs = ["schema_sql.h"],
    cmd = """(
        echo '#pragma once'
        echo 'namespace vigil {'
        echo 'inline constexpr char kSchemaSql[] = R"sql('
        cat $(location schema.sql)
        echo ')sql";'
        echo '}  // namespace vigil'
    ) > $@""",
)

cc_library(
    name = "schema_sql",
    hdrs = ["schema_sql.h"],
)

cc_library(
    name = "database",
    srcs = ["database.cc"],
    hdrs = ["database.h"],
    data = [":schema.sql"],
    visibility = ["//visibility:public"],
    deps = [
        ":schema_sql",
        ":sqlite_traits",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
        "@sqlite3",
    ],
)

cc_test(
    name = "database_test",
    srcs = ["database_test.cc"],
    deps = [
        ":database",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:result_or_die",
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
    visibility = ["//visibility:public"],
    deps = [
        "@pulse//pulse/core:enum_macros",
        "@pulse//pulse/core:stringify",
    ],
)

cc_library(
    name = "accounts_dao",
    srcs = ["accounts_dao.cc"],
    hdrs = ["accounts_dao.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":account",
        ":database",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:stringify",
    ],
)

cc_test(
    name = "accounts_dao_test",
    srcs = ["accounts_dao_test.cc"],
    deps = [
        ":account",
        ":accounts_dao",
        ":database",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
    ],
)

# TODO(move to pulse)
cc_library(
    name = "time",
    hdrs = ["time.h"],
    visibility = ["//visibility:public"],
    deps = ["@pulse//pulse/core:stringify"],
)
