import setuptools
import numpy as np
from Cython.Build import cythonize
import platform

if platform.system() == 'Darwin': #macOS
    extra_compile_args = ["-O3", "-Xpreprocessor", "-fopenmp"]
    extra_link_args = ["-lomp", "-lm"]
else:
    extra_compile_args = ["-O3", "-march=native", "-fopenmp"]
    extra_link_args = ["-lgomp", "-lm"]

ext = setuptools.Extension(
    "fastenoughimg.fastenoughimg", sources=["fastenoughimg/fastenoughimg.pyx", "src/fastenoughimg.c"],
    include_dirs=[np.get_include()],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
)

setuptools.setup(   
    name="fastenoughimg",
    packages=setuptools.find_packages(),
    ext_modules=cythonize([ext], language_level=3),
)
