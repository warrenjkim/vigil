def _to_const_name(name):
    parts = name.split("_")
    return "k" + "".join([p.capitalize() for p in parts])

def embed_file(name, src):
    native.genrule(
        name = name + "_h",
        srcs = [src],
        outs = [name + ".h"],
        cmd = """(
            echo '#pragma once'
            echo 'namespace iris {'
            echo 'inline constexpr char """ + _to_const_name(name) + """[] = R"iris_embed('
            cat $(location """ + src + """)
            echo ')iris_embed";'
            echo '}  // namespace iris'
        ) > $@""",
    )
    native.cc_library(
        name = name,
        hdrs = [name + ".h"],
        visibility = ["//visibility:public"],
    )
