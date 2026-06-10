import pytest
import numpy as np
import fastenoughimg as fimg
import random

RUNS = 10

def make_gray(val, h=16, w=16):
    """Creates a gray-scaled picture filled with the defined value"""
    return np.full((h, w), val, dtype=np.uint8)

def make_rgb(val, h=16, w=16):
    """Creates an RGB picture filled with the defined value"""
    return np.full((h, w, 3), val, dtype=np.uint8)

#invert function tests

def test_invert():
    """invert(x) must return 255 - x for every pixel"""
    img = np.arange(256, dtype=np.uint8).reshape(16, 16, 1)
    img = np.repeat(img, 3, axis=2)
    assert np.all(fimg.pyinvert(img) == 255 - img)

@pytest.mark.parametrize("run", range(RUNS))
def test_double_invert(run):
    """Double invertion must return the same image"""
    img = np.random.randint(0, 256, (16, 16, 3), dtype=np.uint8)
    assert np.all(fimg.pyinvert(fimg.pyinvert(img)) == img)

def test_invert_white():
    assert np.all(fimg.pyinvert(make_rgb(255)) == 0)

def test_invert_black():
    assert np.all(fimg.pyinvert(make_rgb(0)) == 255)

def test_invert_shape():
    """Output shape must match the input shape"""
    img = make_rgb(128)
    assert fimg.pyinvert(img).shape == img.shape

#adjust_brightness function tests

def test_brightness():
    """Defined brightness change should add the specified value to every pixel"""
    assert np.all(fimg.pyadjust_brightness(make_rgb(128), 50) == 178)

@pytest.mark.parametrize("run", range(RUNS))
def test_brightness_zero(run):
    """Zero brightness change does not change the picture"""
    img = np.random.randint(0, 256, (16, 16, 3), dtype=np.uint8)
    assert np.all(fimg.pyadjust_brightness(img, 0) == img)

def test_brightness_overflow():
    """If the brightness value is too high (pixel + brightness value > 255) the final value must be 255"""
    assert np.all(fimg.pyadjust_brightness(make_rgb(200), 60) == 255)

def test_brightness_overflow_low():
    """If the brightness value is too low (pixel + brightness value < 0) the final value must be 0"""
    assert np.all(fimg.pyadjust_brightness(make_rgb(60), -100) == 0)

def test_brightness_shape():
    """Output shape must match the input shape"""
    img = make_rgb(128)
    assert fimg.pyadjust_brightness(img, 5).shape == img.shape

@pytest.mark.parametrize("run", range(RUNS))
def test_brightness_random(run):
    """Adds a random number as a brightness change to a random image"""
    image_val = random.randint(0, 255)
    br_val = random.randint(-128, 128)
    correct = max(0, min(255, image_val + br_val))
    assert np.all(fimg.pyadjust_brightness(make_rgb(image_val), br_val) == correct)

#rgb_to_gray function tests

def test_gray_black():
    """Black image must convert to black grayscale"""
    assert np.all(fimg.pyrgb_to_gray(make_rgb(0)) == 0)

def test_gray_white():
    """White image must convert to white grayscale"""
    assert np.all(fimg.pyrgb_to_gray(make_rgb(255)) == 255)

def test_gray_red_pixel():
    """Pure red pixel must be changed to 0.299 * 255 ≈ 76,2 (BT.601)"""
    img = np.zeros((1, 1, 3), dtype=np.uint8)
    img[0][0][0] = 255  # R=255, G=0, B=0
    result = int(fimg.pyrgb_to_gray(img)[0][0])
    assert 76 <= result <= 77

def test_gray_output_shape():
    """Output must be 2D, not 3D as input"""
    assert fimg.pyrgb_to_gray(make_rgb(128, 10, 20)).shape == (10, 20)

#histogram_eq function tests

def test_histogram_eq_uniform():
    """A uniform image must remain unchanged"""
    img = make_gray(128)
    assert np.all(fimg.pyhistogram_eq(img) == img)

def test_histogram_eq_shape():
    """Output shape must match the input shape"""
    assert fimg.pyhistogram_eq(make_gray(128, 10, 20)).shape == (10, 20)

def test_histogram_eq_range():
    """After equalization the pixel range must increase or stay the same"""
    img = np.zeros((10, 20), dtype=np.uint8)
    img[:, :10] = 50
    img[:, 10:] = 200
    out = fimg.pyhistogram_eq(img)
    assert int(out.max()) - int(out.min()) >= int(img.max()) - int(img.min())

#threshold_otsu function tests

def test_otsu_binary():
    """Output must contain only 0 and 255 pixels"""
    img = np.random.randint(0, 256, (20, 20), dtype=np.uint8)
    out, _ = fimg.pythreshold_otsu(img)
    assert np.all((out == 0) | (out == 255))

def test_otsu_threshold_range():
    """Threshold value must be a number in between 0 and 255"""
    img = np.random.randint(0, 256, (20, 20), dtype=np.uint8)
    _, t = fimg.pythreshold_otsu(img)
    assert 0 <= t <= 255

def test_otsu_bimodal():
    """For a two-shaded image the threshold must be a value that is between the two pixel values"""
    img = np.zeros((10, 20), dtype=np.uint8)
    img[:, :10] = 100
    img[:, 10:] = 200
    _, t = fimg.pythreshold_otsu(img)
    assert 100 <= t < 200

def test_otsu_shape():
    """Output shape must match the input shape"""
    img = make_gray(128, 10, 20)
    out, _ = fimg.pythreshold_otsu(img)
    assert out.shape == (10, 20)

#edge_detect function tests

def test_edge_uniform():
    """Uniform image has no edges"""
    assert np.all(fimg.pyedge_detect(make_gray(128)) == 0)

def test_edge_line():
    """A sharp line must return a nonzero result"""
    img = np.zeros((20, 20), dtype=np.uint8)
    img[:, 10:] = 255
    out = fimg.pyedge_detect(img)
    assert out[:, 9:11].max() > 0

def test_edge_shape():
    """Output shape must match the input shape"""
    assert fimg.pyedge_detect(make_gray(100, 10, 20)).shape == (10, 20)

#convolve function tests

def test_convolve_identity():
    """Identity kernel must leave the image unchanged"""
    img = np.random.randint(0, 256, (20, 20), dtype=np.uint8)
    kernel = np.array([[0, 0, 0], [0, 1, 0], [0, 0, 0]], dtype=np.float32)
    assert np.all(fimg.pyconvolve(img, kernel) == img)

def test_convolve_mean_uniform():
    """The mean filter on a uniform image must return the same image"""
    kernel = np.ones((3, 3), dtype=np.float32) / 9.0
    assert np.all(fimg.pyconvolve(make_gray(128), kernel) == 128)

def test_convolve_shape():
    """Output shape must match the input shape"""
    kernel = np.array([[1, 2, -1], [1, 1, 1], [0, 1, 0]], dtype=np.float32)
    assert fimg.pyconvolve(make_gray(128, 10, 20), kernel).shape == (10, 20)

def test_convolve_kernel_size():
    """The kernel must not be larger than the image"""
    img = make_gray(128, 4, 4)
    kernel = np.ones((8, 8), dtype=np.float32)
    with pytest.raises(ValueError):
        fimg.pyconvolve(img, kernel)

#sharpen function tests are not necessery as the function is a special case of the convolve function

#blur_gauss function tests

def test_blur_uniform():
    """Uniform image must stay unchanged"""
    assert np.all(fimg.pyblur_gauss(make_gray(128), 1.0) == 128)

def test_blur_noise():
    """Blurring must reduce the noise of the image"""
    img = np.random.randint(0, 256, (128, 128), dtype=np.uint8)
    assert float(fimg.pyblur_gauss(img, 2.0).std()) < float(img.std())

def test_blur_negative_sigma():
    """Negative sigma can not be a correct input"""
    with pytest.raises(ValueError):
        fimg.pyblur_gauss(make_gray(128), -1.0)
    
def test_blur_shape():
    """Output shape must match the input shape"""
    assert fimg.pyblur_gauss(make_gray(128, 10, 20), 1.0).shape == (10, 20)
