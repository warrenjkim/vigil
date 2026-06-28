genrule(
    name = "iris_html_h",
    srcs = ["index.html"],
    outs = ["iris_html.h"],
    cmd = """(
        echo '#pragma once'
        echo 'namespace iris {'
        echo 'inline constexpr char kIrisHtml[] = R"html('
        cat $(location index.html)
        echo ')html";'
        echo '}  // namespace iris'
    ) > $@""",
)

cc_library(
    name = "iris_html",
    hdrs = ["iris_html.h"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "accounts_new_html_h",
    srcs = ["accounts/new.html"],
    outs = ["accounts_new_html.h"],
    cmd = """(
        echo '#pragma once'
        echo 'namespace iris {'
        echo 'inline constexpr char kAccountsNewHtml[] = R"html('
        cat $(location accounts/new.html)
        echo ')html";'
        echo '}  // namespace iris'
    ) > $@""",
)

cc_library(
    name = "accounts_new_html",
    hdrs = ["accounts_new_html.h"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "dashboard_html_h",
    srcs = ["dashboard.html"],
    outs = ["dashboard_html.h"],
    cmd = """(
        echo '#pragma once'
        echo 'namespace iris {'
        echo 'inline constexpr char kDashboardHtml[] = R"html('
        cat $(location dashboard.html)
        echo ')html";'
        echo '}  // namespace iris'
    ) > $@""",
)

cc_library(
    name = "dashboard_html",
    hdrs = ["dashboard_html.h"],
    visibility = ["//visibility:public"],
)
