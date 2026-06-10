import time
import numpy as np
import cv2
import fastenoughimg as fimg

#funkcja do mierzenia czasu w milisekundach
def measure(function, *args,):
    function(*args)  #załadowanie funkcji
    start = time.perf_counter()
    for _ in range(100):
        function(*args)
    return (time.perf_counter() - start) / 100 * 1000

#testowy obraz 1920x1080 i parametry
img_rgb = np.random.randint(0, 256, (1080, 1920, 3), dtype=np.uint8)
img_gray = np.random.randint(0, 256, (1080, 1920), dtype=np.uint8)
kernel = np.ones((5, 5), dtype=np.float32) / 25.0
sigma = 2.0

print(f"{'Function'} {'fastenoughimg'} {'OpenCV'}")
print("-" * 52)

#invert
t_start = measure(fimg.pyinvert, img_rgb)
t_end = measure(cv2.bitwise_not, img_rgb)
print(f"{'invert'} {t_start:.2f}ms {t_end:.2f}ms")

#adjust_brightness
t_start = measure(fimg.pyadjust_brightness, img_rgb, 50)
t_end = measure(cv2.convertScaleAbs, img_rgb, None, 1, 50)
print(f"{'adjust_brightness'} {t_start:.2f}ms {t_end:.2f}ms")

#rgb_to_gray
t_start = measure(fimg.pyrgb_to_gray, img_rgb)
t_end = measure(cv2.cvtColor, img_rgb, cv2.COLOR_RGB2GRAY)
print(f"{'rgb_to_gray'} {t_start:.2f}ms {t_end:.2f}ms")

#histogram_eq
t_start = measure(fimg.pyhistogram_eq, img_gray)
t_end = measure(cv2.equalizeHist, img_gray)
print(f"{'histogram_eq'} {t_start:.2f}ms {t_end:.2f}ms")

#threshold_otsu
t_start = measure(fimg.pythreshold_otsu, img_gray)
t_end = measure(cv2.threshold, img_gray, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
print(f"{'threshold_otsu'} {t_start:.2f}ms {t_end:.2f}ms")

#edge_detect
def cv_sobel(img):
    gx = cv2.Sobel(img, cv2.CV_16S, 1, 0)
    gy = cv2.Sobel(img, cv2.CV_16S, 0, 1)
    return cv2.addWeighted(cv2.convertScaleAbs(gx), 0.5, cv2.convertScaleAbs(gy), 0.5, 0)

t_start = measure(fimg.pyedge_detect, img_gray)
t_end = measure(cv_sobel, img_gray)
print(f"{'edge_detect'} {t_start:.2f}ms {t_end:.2f}ms")

#convolve
t_start = measure(fimg.pyconvolve, img_gray, kernel)
t_end = measure(cv2.filter2D, img_gray, -1, kernel)
print(f"{'convolve (5x5)'} {t_start:.2f}ms {t_end:.2f}ms")

#blur_gauss
t_start = measure(fimg.pyblur_gauss, img_gray, sigma)
t_end = measure(cv2.GaussianBlur, img_gray, (0, 0), sigma)
print(f"{'blur_gauss'} {t_start:.2f}ms {t_end:.2f}ms")
