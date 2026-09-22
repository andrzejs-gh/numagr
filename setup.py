from setuptools import setup, Extension

setup(

    name="test",
    ext_modules=[
		   Extension("hello", sources=["hello.c"]),
		   Extension("numagr", sources=["numagr.c"])
		]
)
