genrule(
    name = "iris_html_h",
    srcs = [":index.html"],
    outs = ["iris_html.h"],
    cmd = """(
        echo '#pragma once'
        echo 'namespace iris {'
        echo 'inline constexpr char kIrisHtml[] = R"html('
        cat $(location :index.html)
        echo ')html";'
        echo '}  // namespace iris'
    ) > $@""",
)

cc_library(
    name = "iris_html",
    hdrs = ["iris_html.h"],
    visibility = ["//visibility:public"],
)
