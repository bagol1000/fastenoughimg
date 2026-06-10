import setuptools
import numpy as np
from Cython.Build import cythonize

numpy_include_path = np.get_include()

setuptools.setup(
    name="fastenoughimg",
    packages=setuptools.find_packages(),
    ext_modules=
        cythonize(setuptools.Extension("fastenoughimg.fastenoughimg", sources=["fastenoughimg/fastenoughimg.pyx", "src/fastenoughimg.c"],
        include_dirs=[numpy_include_path],
        extra_compile_args=["-O3", "-march=native", "-fopenmp"],
        extra_link_args=["-lgomp", "-lm"],),
        language_level = 3,),
)
