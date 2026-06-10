import setuptools
import numpy as np
from Cython.Build import cythonize
import platform
import subprocess

if platform.system() == 'Darwin':
    try:
        libomp_prefix = subprocess.check_output(['brew', '--prefix', 'libomp'], text=True).strip()   
        extra_compile_args = ["-O3", f"-I{libomp_prefix}/include", "-Xpreprocessor", "-fopenmp"]
        extra_link_args = [f"-L{libomp_prefix}/lib", "-lomp", "-lm"]
    except Exception:
        extra_compile_args = ["-O3"]
        extra_link_args = ["-lm"]
else:
    extra_compile_args = ["-O3", "-march=native", "-fopenmp"]
    extra_link_args = ["-lgomp", "-lm"]

ext = setuptools.Extension(
    "fastenoughimg.fastenoughimg",
    sources=["fastenoughimg/fastenoughimg.pyx", "src/fastenoughimg.c"],
    include_dirs=[np.get_include()],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
)

setuptools.setup(   
    name="fastenoughimg",
    packages=setuptools.find_packages(),
    ext_modules=cythonize([ext], language_level=3),
)
