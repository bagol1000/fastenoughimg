#ifndef SEEN_FASTIMG_H
#define SEEN_FASTIMG_H

#include <stdint.h> //potrzebna jest pewność co do rozmiarów okreslonych typów - pixel ma zawsze 8 bitów

// restrict - obiekty nie nakladaja sie w pamieci i pozwala to kompilatorowi przyspieszyc niektore obliczenia np. za pomoca SIMD
void invert(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, int channels);

void adjust_brightness(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, int channels, int delta);

void rgb_to_gray(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height);

void histogram_eq(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height);

int threshold_otsu(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height);

void edge_detect(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height);

void convolve(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, const float* restrict kernel, int k_width, int k_height);

void sharpen(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height);

void blur_gauss(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, double sigma);

#endif