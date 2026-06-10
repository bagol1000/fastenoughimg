# Performance

Benchmarked on a 1920×1080 image, averaged over 100 runs.
Compared against [OpenCV](https://opencv.org/) 4.x.

| Function              | fastenoughimg | OpenCV  | Result        |
|-----------------------|---------------|---------|---------------|
| `invert`              | 0.50 ms       | 0.38 ms | comparable    |
| `adjust_brightness`   | 0.85 ms       | 1.29 ms | **faster**    |
| `rgb_to_gray`         | 0.89 ms       | 0.19 ms | slower        |
| `histogram_eq`        | 1.63 ms       | 0.42 ms | slower        |
| `threshold_otsu`      | 1.33 ms       | 1.29 ms | comparable    |
| `edge_detect`         | 6.03 ms       | 6.01 ms | comparable    |
| `convolve (5x5)`      | 10.16 ms      | 5.12 ms | slower        |
| `blur_gauss`          | 47.75 ms      | 0.81 ms | slower        |

## Why is fastenoughimg slower?

**rgb_to_gray**, **histogram_eq** and **convolve** are all slower for the same reason: OpenCV uses SIMD instructions to process multiple pixels per clock cycle, while fastenoughimg processes one pixel at a time and even the usage of OpenMP does not stand a chance against SIMD. The SIMD implementation in these cases is unfortunately not trivial.
  
**blur_gauss** - OpenCV uses an IIR (Infinite Impulse Response) filter approximation instead of the exact Gaussian convolution and  runs in O(n) time regardless of the value of sigma. The fastenoughimg version is accurate :)



