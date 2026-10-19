from setuptools import setup, Extension

src = [
		"src/numagr.c",
		"src/floats_hash_table.c",
		"src/floats_array.c"
]

extra_comp_flags = [
	"-O3",
	"-fno-fast-math",
	"-lm"
]

setup(
    name = "numagr",
    version = "0.9",
    author = "andrzejs",
    url = "https://github.com/andrzejs-gh/numagr",
    ext_modules = [Extension("numagr",
						sources = src,
						extra_compile_args = extra_comp_flags
					)
		]
)
