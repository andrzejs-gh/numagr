from setuptools import setup, Extension

setup(

    name="test",
    ext_modules=[
		   Extension("hello", sources=["hello.c"], extra_compile_args=["-O3"]),
		   Extension("numagr", sources=["numagr.c"], extra_compile_args=["-O3"])
		]
)
