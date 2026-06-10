# API Reference

## pyinvert

```python
pyinvert(img)
```

Inverts the colors of an RGB image. Returns 255 - x for every
pixel.

- img: numpy array, shape (height, width, 3), dtype uint8
- returns: inverted image, same shape

---
## pyadjust_brightness

pyadjust_brightness(img, delta)
Adjusts brightness by adding delta to every pixel, scaled to [0,
255].

- img: numpy array, shape (height, width, 3), dtype uint8
- delta: integer, positive increases brightness, negative
decreases
- returns: adjusted image, same shape

---
## pyrgb_to_gray

pyrgb_to_gray(img)
Converts RGB image to a grayscale one.

- img: numpy array, shape (height, width, 3), dtype uint8
- returns: grayscale image, shape (height, width)

---
## pyhistogram_eq

pyhistogram_eq(img)
Equalizes the histogram of a grayscale image to improve contrast.

- img: numpy array, shape (height, width), dtype uint8
- returns: equalized image, same shape

---
## pythreshold_otsu

pythreshold_otsu(img)
Applies Otsu's thresholding — finds the optimal threshold to
separate pixels into two groups.

- img: numpy array, shape (height, width), dtype uint8
- returns: tuple (black_white_image, threshold) where black_white_image has only 0 and 255 values

---
## pyedge_detect

pyedge_detect(img)
Detects edges using the Sobel method.

- img: numpy array, shape (height, width), dtype uint8
- returns: with-edge image, same shape

---
## pyconvolve

pyconvolve(img, kernel)
Applies a convolution with a defined kernel.

- img: numpy array, shape (height, width), dtype uint8
- kernel: numpy array, dtype float32, must be smaller than image
- returns: convolved image, same shape

---
## pysharpen

pysharpen(img)
Sharpens the image - a special case of pyconvolve function.

- img: numpy array, shape (height, width), dtype uint8
- returns: sharpened image, same shape

---
## pyblur_gauss

pyblur_gauss(img, sigma)
Applies Gaussian blur with separable 1D passes.

- img: numpy array, shape (height, width), dtype uint8
- sigma: float, must be positive (determines the blur strenght)
- returns: blurred image, same shape
