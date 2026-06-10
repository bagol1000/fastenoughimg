# Changelog

All important changes to this project will be presented in this file.

## [0.1.3] - 2026-06-10

#### C
- deleted '#pragma omp' parallel computations from 'invert' function

#### Python / Cython
- added tests to the package functions - 'test_fimg.py' file
- updated 'setup.py' file
- added two condition checkers, added missing nogil in 'adjust_brightness' function and deleted nogil in 'invert' function in 'fastenoughimg.pyx' file
- added 'benchmarks.py' file 

#### Other
- added 'LICENSE' file 
- updated '.gitignore' file
- added tests on different operating systems using github workflows
- updated 'README.md'
- added github docs workflow
- created sphinx documentation and project page on github

## [0.1.2] - 2026-06-09

#### C
- all functions changed: algorithms optimization and OpenMP prallel computations applied
- now every function but 'threshold_otsu' uses OpenMP
-

#### Python / Cython
- Cython wrapper file updated with 'nogil' usage and 'np.ascontiguousarray()'
- 'setup.py' updated with new compilation flags

#### Other
- 'Makefile' updated with '-fopenmp' '-lm' and 'lgomp'
- 'README.md' file added

## [0.1.1] - 2026-06-08

#### C
- 'threshold_otsu' - Otsu's method for image shades categorization - binary image as output
- 'edge_detect' - edge detection function
- 'convolve' - general convolution function
- 'sharpen' - sharpening filter as a special case of 'convolve' function
- 'blur_gauss' - Gaussian blur function
- 'fastenoughimg.h' header file updated

#### Fixes
- fixed error in 'threshold_otsu' - return statement placed incorrectly

## [0.1.0] - 2026-06-07

#### C
- 'invert' - pixel negation function (negative image)
- 'adjust_brightness' - image brightness change
- 'rgb_to_gray' - RGB colors to grayscale conversion
- 'histogram_eq' - shade distribution equalization
- 'fastenoughimg.h' - header with function declarations

#### Python / Cython
- Cython wrapper added
- 'pyproject.toml', 'setup.py', '__init__.py' configuration files added

#### Other
- '.gitignore' and 'Makefile' files added



