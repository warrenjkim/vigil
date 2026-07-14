def bundle(name, html = [], js = [], css = []):
    if type(html) == "string":
        html = [html]
    if type(js) == "string":
        js = [js]
    if type(css) == "string":
        css = [css]

    native.filegroup(
        name = name,
        srcs = html + js + css,
        visibility = ["//visibility:public"],
    )
