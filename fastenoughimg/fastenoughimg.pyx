import numpy as np
cimport numpy as np

cdef extern from "<stdint.h>":
    ctypedef unsigned char uint8_t

cdef extern from "../src/fastenoughimg.h":
    void invert(const uint8_t* src, uint8_t* dest, int width, int height, int channels);

    void adjust_brightness(const uint8_t* src, uint8_t* dest, int width, int height, int channels, int delta);

    void rgb_to_gray(const uint8_t* src, uint8_t* dest, int width, int height);

    void histogram_eq(const uint8_t* src, uint8_t* dest, int width, int height);

def pyinvert(np.ndarray[np.uint8_t, ndim = 3] img):
    cdef np.ndarray[np.uint8_t, ndim = 3] out = np.empty((img.shape[0], img.shape[1], img.shape[2]), dtype = np.uint8)
    invert(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0], img.shape[2])
    return out

def pyadjust_brightness(np.ndarray[np.uint8_t, ndim = 3] img, int delta):
    cdef np.ndarray[np.uint8_t, ndim = 3] out = np.empty((img.shape[0], img.shape[1], img.shape[2]), dtype = np.uint8)
    adjust_brightness(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0], img.shape[2], delta)
    return out

def pyrgb_to_gray(np.ndarray[np.uint8_t, ndim = 3] img):
    cdef np.ndarray[np.uint8_t, ndim = 2] out = np.empty((img.shape[0], img.shape[1]), dtype = np.uint8)
    rgb_to_gray(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0])
    return out

def pyhistogram_eq(np.ndarray[np.uint8_t, ndim = 2] img):
    cdef np.ndarray[np.uint8_t, ndim = 2] out = np.empty((img.shape[0], img.shape[1]), dtype = np.uint8)
    histogram_eq(<uint8_t*> img.data, <uint8_t*> out.data, img.shape[1], img.shape[0])
    return out