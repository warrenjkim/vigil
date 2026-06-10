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
        "//vigil/db/sqlite_traits:column_extractor",
        "//vigil/db/sqlite_traits:parameter_binder",
        "//vigil/db/sqlite_traits:types",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
        "@pulse//pulse/strings:cat",
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

cc_library(
    name = "transaction",
    hdrs = ["transaction.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":time",
        "@pulse//pulse/core:enum_macros",
        "@pulse//pulse/core:stringify",
        "@pulse//pulse/strings:cat",
    ],
)

cc_library(
    name = "transactions_dao",
    srcs = ["transactions_dao.cc"],
    hdrs = ["transactions_dao.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":database",
        ":time",
        ":transaction",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:stringify",
    ],
)

cc_test(
    name = "transactions_dao_test",
    srcs = ["transactions_dao_test.cc"],
    deps = [
        ":account",
        ":accounts_dao",
        ":database",
        ":transaction",
        ":transactions_dao",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
    ],
)

cc_library(
    name = "holding",
    hdrs = ["holding.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@pulse//pulse/core:stringify",
        "@pulse//pulse/strings:cat",
    ],
)

cc_library(
    name = "holdings_dao",
    srcs = ["holdings_dao.cc"],
    hdrs = ["holdings_dao.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":database",
        ":holding",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
    ],
)

cc_test(
    name = "holdings_dao_test",
    srcs = ["holdings_dao_test.cc"],
    deps = [
        ":account",
        ":accounts_dao",
        ":database",
        ":holding",
        ":holdings_dao",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
    ],
)

cc_library(
    name = "trade",
    hdrs = ["trade.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":time",
        "@pulse//pulse/core:enum_macros",
        "@pulse//pulse/core:stringify",
        "@pulse//pulse/strings:cat",
    ],
)

cc_library(
    name = "trades_dao",
    srcs = ["trades_dao.cc"],
    hdrs = ["trades_dao.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":database",
        ":time",
        ":trade",
        "@pulse//pulse/core:result",
        "@pulse//pulse/core:stringify",
    ],
)

cc_test(
    name = "trades_dao_test",
    srcs = ["trades_dao_test.cc"],
    deps = [
        ":account",
        ":accounts_dao",
        ":database",
        ":trade",
        ":trades_dao",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "@pulse//pulse/core:error",
        "@pulse//pulse/core:result",
    ],
)
