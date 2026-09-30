from setuptools import setup, Extension

setup(

    name="numagr",
    ext_modules=[
		   Extension("numagr", sources=["numagr.c"], extra_compile_args=["-O3", 
										 "-fno-fast-math"])
		]
)
