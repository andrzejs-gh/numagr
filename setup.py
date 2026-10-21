from setuptools import setup, Extension
import sys

with open("README.md", "r", encoding = "utf-8") as readme:
	long_description = readme.read()

src = [
		"src/numagr.c",
		"src/floats_hash_table.c",
		"src/floats_array.c"
]

extra_comp_flags = [
	"-O3",
	"-fno-fast-math",
]

define_macros = []

if sys.platform == "win32":
    extra_comp_flags = ["/O2"]
    extra_link_args = []
    define_macros.append(("NOMINMAX", "1"))
else:
    extra_comp_flags = ["-O3", "-fno-fast-math"]
    extra_link_args = ["-lm"]

setup(
    name = "numagr",
    version = "0.9",
    author = "andrzejs",
    url = "https://github.com/andrzejs-gh/numagr",
    description = "Python C extension offering efficient statistical methods for numerical data.",
    long_description = long_description,
    long_description_content_type = "text/markdown",
    ext_modules = [Extension("numagr",
						sources = src,
						define_macros = define_macros,
						extra_compile_args = extra_comp_flags,
						extra_link_args = extra_link_args
					)
		]
)
