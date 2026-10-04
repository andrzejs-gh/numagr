from setuptools import setup, Extension

src = [
		"numagr.c",
		"floats_hash_table.c",
		"floats_array.c"
]

extra_comp_flags = [
	"-O3",
	"-fno-fast-math"
]

setup(
    name = "numagr",
    ext_modules = [Extension("numagr",
						sources = src,
						extra_compile_args = extra_comp_flags
					)
		]
)
