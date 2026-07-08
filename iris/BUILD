load(":embed.bzl", "embed_file")

[
    embed_file(
        name = f.replace("/", "_").replace(".", "_"),
        src = f,
    )
    for f in glob(
        include = [
            "**/*.html",
            "**/*.js",
            "**/*.css",
        ],
        allow_empty = True,
        exclude = [],
    )
]
