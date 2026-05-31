cc_library(
    name = "sqlite_traits",
    visibility = ["//visibility:public"],
    deps = [
        ":column_extractor",
        ":parameter_binder",
        ":types",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "parameter_binder",
    hdrs = ["parameter_binder.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":types",
        "@sqlite3",
    ],
)

cc_library(
    name = "column_extractor",
    hdrs = ["column_extractor.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":types",
        "@sqlite3",
    ],
)
