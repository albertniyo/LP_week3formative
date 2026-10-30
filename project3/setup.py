from setuptools import setup, Extension

# CPython module - analysis
ext = Extension(
    "analysis",
    sources=["analysis.c"],
    extra_compile_args=["-O3", "-Wall"],
)

setup(
    name="analysis",
    version="1.0",
    description="Simple CPython stats extension",
    packages=[],
    py_modules=[],
    ext_modules=[ext],
)
