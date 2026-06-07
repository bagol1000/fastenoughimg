import setuptools
import numpy as np
from Cython.Build import cythonize

numpy_include_path = np.get_include()

setuptools.setup(
    name="fastenoughimg",
    packages=setuptools.find_packages(),
    include_dirs=[numpy_include_path],
    #extra_compile_args=["-O3", "-DNDEBUG=1"],
    ext_modules=[
        cythonize("fastenoughimg/fastenoughimg.pyx", language_level = 3,),
    ]
)