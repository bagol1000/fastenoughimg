# fastenoughimg

Fastenoughimg is a Python package for image processing implemented in C with a Cython wrapper. The name for this package is not random as the implemented functions are fast enough for everyday usage :) 
Most of the algorithms use OpenMP parallelization, LUT-based operations, fixed-point arithmetic, and cache-friendly memory layouts.

Refer to the package **[documentation](https://bagol1000.github.io/fastenoughimg/)** for the full API reference.

---

## Features

  | Function              | Description                                    | Optimization                   |
  |-----------------------|------------------------------------------------|--------------------------------|
  | `pyinvert`            | Creates a negative image                       | OpenMP                         |
  | `pyadjust_brightness` | Brightness adjustment with clamping            | LUT, OpenMP                    |
  | `pyrgb_to_gray`       | RGB to grayscale (BT.601)                      | Fixed-point arithmetic, OpenMP |
  | `pyhistogram_eq`      | Histogram equalization                         | LUT, OpenMP                    |
  | `pythreshold_otsu`    | Otsu thresholding, returns threshold value     | Prefix sums                    |
  | `pyedge_detect`       | Sobel edge detection                           | Reflect padding, OpenMP        |
  | `pyconvolve`          | 2D convolution with arbitrary float kernel     | Reflect padding, OpenMP        |
  | `pysharpen`           | Image sharpening                               | Special case of `pyconvolve`   |
  | `pyblur_gauss`        | Gaussian blur                                  | Separable 1D passes            |


---

## Installation     

Requires: Python ≥ 3.11, GCC with OpenMP support, Cython, NumPy ≥
2.0.

```bash
git clone https://github.com/bagol1000/fastenoughimg.git
cd fastenoughimg
pip install -e .

---
Usage

import numpy as np
import fastenoughimg as fimg

# load image as numpy array, shape (H, W, 3), dtype uint8
img = np.array(...)

# color inversion
inverted = fimg.pyinvert(img)

# brightness adjustment (clamped to [0, 255])
brighter = fimg.pyadjust_brightness(img, delta=50)

# convert to grayscale — returns (H, W) array
gray = fimg.pyrgb_to_gray(img)

# histogram equalization
equalized = fimg.pyhistogram_eq(gray)

# Otsu thresholding — returns (binary image, threshold value)
binary, threshold = fimg.pythreshold_otsu(gray)

# Sobel edge detection
edges = fimg.pyedge_detect(gray)

# Gaussian blur (sigma controls blur strength)
blurred = fimg.pyblur_gauss(gray, sigma=2.0)

# general convolution with custom kernel
kernel = np.array([[0, -1,  0],
                 [-1,  5, -1],
                 [0, -1,  0]], dtype=np.float32)
sharpened = fimg.pyconvolve(gray, kernel)

---
Performance

Benchmarked on a 1920×1080 image, averaged over 100 runs.
Compared against OpenCV 4.x.

┌───────────────────┬───────────────┬─────────┬────────────┐
│     Function      │ fastenoughimg │ OpenCV  │   Result   │
├───────────────────┼───────────────┼─────────┼────────────┤
│ invert            │ 0.50 ms       │ 0.38 ms │ comparable │
├───────────────────┼───────────────┼─────────┼────────────┤
│ adjust_brightness │ 0.85 ms       │ 1.29 ms │ faster     │
├───────────────────┼───────────────┼─────────┼────────────┤
│ rgb_to_gray       │ 0.89 ms       │ 0.19 ms │ slower     │
├───────────────────┼───────────────┼─────────┼────────────┤
│ histogram_eq      │ 1.63 ms       │ 0.42 ms │ slower     │
├───────────────────┼───────────────┼─────────┼────────────┤
│ threshold_otsu    │ 1.33 ms       │ 1.29 ms │ comparable │
├───────────────────┼───────────────┼─────────┼────────────┤
│ edge_detect       │ 6.03 ms       │ 6.01 ms │ comparable │
├───────────────────┼───────────────┼─────────┼────────────┤
│ convolve (5x5)    │ 10.16 ms      │ 5.12 ms │ slower     │
├───────────────────┼───────────────┼─────────┼────────────┤
│ blur_gauss        │ 47.75 ms      │ 0.81 ms │ slower     │
└───────────────────┴───────────────┴─────────┴────────────┘

OpenCV is a 20-year-old library with multiple SIMD instructions.
Functions where fastenoughimg is slower mostly rely on SIMD
vectorizatio (rgb_to_gray, histogram_eq, convolve) or IIR approximations (blur_gauss) that OpenCV uses internally.

---
License

Copyright (C) 2026 Adam Bagiński

This program is free software licensed under the
GNU General Public License v3.0 — see LICENSE for details.
```
