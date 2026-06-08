import setuptools
import numpy as np
from Cython.Build import cythonize

numpy_include_path = np.get_include()

setuptools.setup(
    name="fastenoughimg",
    packages=setuptools.find_packages(),
    include_dirs=[numpy_include_path],
    extra_compile_args=["-O3", "-march=native", "-fopenmp"],
    extra_link_args=["-lgomp", "-lm"],
    ext_modules=[
        cythonize("fastenoughimg/fastenoughimg.pyx", language_level = 3,),
    ]
)