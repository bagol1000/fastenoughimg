import numpy as np
cimport numpy as np

cdef extern from "<stdint.h>":
    ctypedef unsigned char uint8_t

cdef extern from "../src/fastenoughimg.h":
    void invert(const uint8_t* src, uint8_t* dest, int width, int height, int channels);

    void adjust_brightness(const uint8_t* src, uint8_t* dest, int width, int height, int channels, int delta);

    void rgb_to_gray(const uint8_t* src, uint8_t* dest, int width, int height);

    void histogram_eq(const uint8_t* src, uint8_t* dest, int width, int height);

    int threshold_otsu(const uint8_t* src, uint8_t* dest, int width, int height);

    void edge_detect(const uint8_t* src, uint8_t* dest, int width, int height);

    void convolve(const uint8_t* src, uint8_t* dest, int width, int height, const float* kernel, int k_width, int k_height);

    void sharpen(const uint8_t* src, uint8_t* dest, int width, int height);

    void blur_gauss(const uint8_t* src, uint8_t* dest, int width, int height, double sigma);

def pyinvert(np.ndarray[np.uint8_t, ndim = 3] img):
    img = np.ascontiguousarray(img)
    cdef np.ndarray[np.uint8_t, ndim = 3] out = np.empty((img.shape[0], img.shape[1], img.shape[2]), dtype = np.uint8)
    with nogil:
        invert(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0], img.shape[2])
    return out

def pyadjust_brightness(np.ndarray[np.uint8_t, ndim = 3] img, int delta):
    img = np.ascontiguousarray(img)
    cdef np.ndarray[np.uint8_t, ndim = 3] out = np.empty((img.shape[0], img.shape[1], img.shape[2]), dtype = np.uint8)
    adjust_brightness(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0], img.shape[2], delta)
    return out

def pyrgb_to_gray(np.ndarray[np.uint8_t, ndim = 3] img):
    img = np.ascontiguousarray(img)
    cdef np.ndarray[np.uint8_t, ndim = 2] out = np.empty((img.shape[0], img.shape[1]), dtype = np.uint8)
    with nogil:
        rgb_to_gray(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0])
    return out

def pyhistogram_eq(np.ndarray[np.uint8_t, ndim = 2] img):
    img = np.ascontiguousarray(img)
    cdef np.ndarray[np.uint8_t, ndim = 2] out = np.empty((img.shape[0], img.shape[1]), dtype = np.uint8)
    with nogil:
        histogram_eq(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0])
    return out

def pythreshold_otsu(np.ndarray[np.uint8_t, ndim = 2] img):
    img = np.ascontiguousarray(img)
    cdef np.ndarray[np.uint8_t, ndim = 2] out = np.empty((img.shape[0], img.shape[1]), dtype = np.uint8)
    cdef int threshold_otsu
    with nogil:
        threshold = threshold_otsu(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0])
    return out, threshold

def pyedge_detect(np.ndarray[np.uint8_t, ndim = 2] img):
    img = np.ascontiguousarray(img)
    cdef np.ndarray[np.uint8_t, ndim = 2] out = np.empty((img.shape[0], img.shape[1]), dtype = np.uint8)
    with nogil:
        edge_detect(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0])
    return out

def pyconvolve():
    img = np.ascontiguousarray(img)

def pysharpen():
    img = np.ascontiguousarray(img)

def pyblur_gauss():
    img = np.ascontiguousarray(img)