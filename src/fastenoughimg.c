#include "fastenoughimg.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

////////// funkcje pomocnicze ///////////
//////////////////////////////////////////////////////////////////////////////////

//funkcja do rozszerzania obrazu (na brzegach mamy odbicia), aby nie sprawdzac obrazu z poprzednią funkcją reflect - czyli eliminacja if-ów
static uint8_t* pad(const uint8_t* restrict src, int width, int height, int pad_x, int pad_y){
    int p_width = width + 2 * pad_x;
    int p_height = height + 2 * pad_y;
    uint8_t* padded = (uint8_t*)malloc(p_width * p_height);

    for (int row = 0; row < p_height; row++){
        for (int col = 0; col < p_width; col++){
            int org_row = row - pad_y; //org = original
            int org_col = col - pad_x;

            if (org_row < 0) org_row = -org_row;
            else if (org_row >= height) org_row = 2 * (height - 1) - org_row; //Hmax - (org_row - Hmax) = 2Hmax - org_row [Hmax = (height - 1)]

            if (org_col < 0) org_col = -org_col;
            else if (org_col >= width) org_col = 2 * (width - 1) - org_col;

            padded[row * p_width + col] = src[org_row * width + org_col];
        }
    }
    return padded;
}

static float* pad_f(const float* restrict src, int width, int height, int pad_x, int pad_y){
    int p_width = width + 2 * pad_x;
    int p_height = height + 2 * pad_y;
    float* padded = (float*)malloc(p_width * p_height * sizeof(float));

    for (int row = 0; row < p_height; row++){
        for (int col = 0; col < p_width; col++){
            int org_row = row - pad_y; //org = original
            int org_col = col - pad_x;

            if (org_row < 0) org_row = -org_row;
            else if (org_row >= height) org_row = 2 * (height - 1) - org_row; //Hmax - (org_row - Hmax) = 2Hmax - org_row [Hmax = (height - 1)]

            if (org_col < 0) org_col = -org_col;
            else if (org_col >= width) org_col = 2 * (width - 1) - org_col;

            padded[row * p_width + col] = src[org_row * width + org_col];
        }
    }
    return padded;
}

//transpozycja z blokowaniem - pod cache
static void transpose(const float* restrict src, float* restrict out, int width, int height){
    const int C = 32; //pod cache

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < height; i += C){
        for (int j = 0; j < width; j += C){
            int i_end = (i + C < height) ? i + C : height;
            int j_end = (j + C < width) ? j + C : width;
            for (int ii = i; ii < i_end; ii++){
                for (int jj = j; jj < j_end; jj++){
                    out[jj * height + ii] = src[ii * width + jj];
                }
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////

//negatyw obrazu'
void invert(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, int channels){
    int n = width * height * channels;
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) dest[i] = 255 - src[i];
}

//zmiana jasnosci obrazu
//uzycie map_tab - dzieki temu nie sprawdzamy warunku <0 i >255 dla kazdego piksela osobno tylko mamy gotowa 'mapę'
void adjust_brightness(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, int channels, int delta){
    uint8_t map_tab[256];
    
    int n = width * height * channels;

    for (int i = 0; i < 256; i++){
        int c = i + delta;
        map_tab[i] = c < 0 ? 0 : (c > 255 ? 255 : c);
    }

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) dest[i] = map_tab[src[i]];
}

//konwersja z RGB na skale szarosci wg normy BT.601
//zamiast mnoznikow 0.299 dla RED, 0.587 dla GREEN oraz 0.114 dla BLUE
//uzyto tych wartosci pomnozonych przez 2^16, aby wynik przesunac o 16 bitow (podzielic przez 2^16)
//taka operacja jest szybsza i dokladniejsza niz mnozenie przez liczby zmiennoprzecinkowe
void rgb_to_gray(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height){
    const int cr = 19595; //0.299 * 65536 itd.
    const int cg = 38470;
    const int cb = 7471;

    int n = width * height;

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++){
        uint8_t r = src[3 * i];
        uint8_t g = src[3 * i + 1];
        uint8_t b = src[3 * i + 2];
        dest[i] = (uint8_t)((cr * r + cg * g + cb * b) >> 16);
    }
}

//tzw wyrownanie histogramu - optymalizacja kontrastu na obrazie - przeskalowanie odcieni szarosci
void histogram_eq(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height){
    int shade_count[256] = {0}; //nie calloc bo znany rozmiar i latwiej a przy okazji nie jest wolniej

    int n = width * height;

    for (int i = 0; i < n; i++) shade_count[src[i]]++;

    int cumsum[256]; // :)
    cumsum[0] = shade_count[0];
    for (int i = 1; i < 256; i++) cumsum[i] = cumsum[i - 1] + shade_count[i]; //mozna powiedziec dystrybuanta

    int cum_min = 0;
    for (int i = 0; i < 256; i++){
        if(cumsum[i] > 0){
            cum_min = cumsum[i];
            break;
        }
    }

    if (n == cum_min){//obraz o jednorodnym kolorze
        memcpy(dest, src, n);
        return;
    }

    uint8_t remap[256];
    for (int i = 0; i < 256; i++){
        int r = ((long)(cumsum[i] - cum_min) * 255) / (n - cum_min); //normalizacja
        remap[i] = r < 0 ? 0 : r;
    }

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) dest[i] = remap[src[i]];
}

//progowanie Otsu - wybor wartosci granicznej ktora najlepiej rozdziela piksele na dwie grupy (0 albo 255)
int threshold_otsu(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height){
    int shade_count[256] = {0};
    int n = width * height;
    for (int i = 0; i < n; i++) shade_count[src[i]]++;

    double var = -1.0;
    int threshold = 0;
    int dark_count = 0, bright_count = 0;
    long dark_sum = 0;

    long total = 0;
    for (int i = 0; i < 256; i++) total += (long)i * shade_count[i];


    //liczymy liczbe pikseli w grupie < i w grupie > threshold
    //ich sumaryczną jasność a potem średnią
    for (int tr = 0; tr < 256; tr++){

        dark_count += shade_count[tr];
        bright_count = n - dark_count;
        dark_sum += (long)tr * shade_count[tr];

        if (dark_count == 0 || bright_count == 0) continue;

        double mean_dark = (double)dark_sum / dark_count;
        double mean_bright = (double)(total - dark_sum) / bright_count;

        double v = (double)dark_count * bright_count * (mean_dark - mean_bright) * (mean_dark - mean_bright);

        if (v > var) {
            var = v;
            threshold = tr;
        }
    }
    for (int i = 0; i < n; i++) dest[i] = src[i] > threshold ? 255 : 0;
    return threshold;
}

//detekcja krawedzi metoda Sobela za pomoca badania zmianny jasnosci
void edge_detect(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height){
    const int kernel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    const int kernel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

    int pad_radius = 1; //promien kernela
    uint8_t* padded = pad(src, width, height, pad_radius, pad_radius);
    int p_width = width + 2 * pad_radius;

    #pragma omp parallel for schedule(static)
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            int grad_x = 0, grad_y = 0;

            for (int k_row = 0; k_row < 3; k_row++){
                for (int k_col = 0; k_col < 3; k_col++){
                    int shade = padded[(row + k_row) * p_width + (col + k_col)];
                    grad_x += kernel_x[k_row * 3 + k_col] * shade;
                    grad_y += kernel_y[k_row * 3 + k_col] * shade;
                }
            }
            int s = abs(grad_x) + abs(grad_y); //zamiast pierwiastka ktory jest niekorzystny obliczeniowo (metryka manhattan) - czesto uzywane zastapienie wydajnościowe
            dest[row * width + col] = s > 255 ? 255 : s;
        }
    }
    free(padded);
}

//obliczanie sredniej wazonej dla kazdego piksela na podstawie jego otoczenia oraz podanej macierzy
//moze byc wykorzystywane do wyostrznia, rozmywania itp.
void convolve(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, const float* restrict kernel, int k_width, int k_height){
    int radius_h = k_height / 2; //promien zasiegu od pixela
    int radius_w = k_width / 2;

    uint8_t* padded = pad(src, width, height, radius_w, radius_h);
    int p_width = width + 2 * radius_w; 

    #pragma omp parallel for schedule(static)
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            float pixel_sum = 0.0f;
            for (int k_row = 0; k_row < k_height; k_row++){
                for (int k_col = 0; k_col < k_width; k_col++){
                    pixel_sum += kernel[k_row * k_width + k_col] * padded[(row + k_row) * p_width + (col + k_col)];
                }
            }
            int z = (int)(pixel_sum + 0.5f); //zaokraglenie w gore bo kompilator zawsze w dol
            dest[row * width + col] = z < 0 ? 0 : (z > 255 ? 255 : z);
        }
    }
    free(padded);
}

//specyficzny przypadek funkcji convolve ale jak uzytkownik chce wyostrzyc obraz to raczej nie bedzie szukal specjalnego jądra pod to
void sharpen(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height){
    const float kernel[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
    convolve(src, dest, width, height, kernel, 3, 3);
}

void blur_gauss(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, double sigma){
    int r = (int)(3.0 * sigma + 0.5);//z zaokragleniem
    int kernel_size = 2 * r + 1;

    float* weights = (float*)malloc(kernel_size * sizeof(float));
    float weight_sum = 0.0f;

    //wektor wag
    for (int i = 0; i < kernel_size; i++){
        weights[i] = expf(-(i - r) * (i - r) / (2.0f * sigma * sigma));
        weight_sum += weights[i];
    }

    for (int i = 0; i < kernel_size; i++) weights[i] /= weight_sum; //normalizacja

    float* temp = (float*)malloc(width * height * sizeof(float));

    //przejscie po wierszach
    uint8_t* padded_src = pad(src, width, height, r, 0);
    int p_width = width + 2 * r;

    #pragma omp parallel for schedule(static)
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            float pixel_sum = 0.0f;
            for (int i = 0; i < kernel_size; i++){
                pixel_sum += weights[i] * padded_src[row * p_width + (col + i)];
            }
            temp[row * width + col] = pixel_sum;
        }
    }

    float* tempT = (float*)malloc(width * height * sizeof(float));
    transpose(temp, tempT, width, height);
    
    float* padded_tempT = pad_f(tempT, height, width, r, 0);
    int pT_width = height + 2 * r;

    //przejscie po kolumnach
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < width; row++){
        for (int col = 0; col < height; col++){
            float pixel_sum = 0.0f;
            for (int i = 0; i < kernel_size; i++){
                pixel_sum += weights[i] * padded_tempT[row * pT_width + (col + i)];
            }
            temp[row * height + col] = pixel_sum;
        }
    }

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < height; i += 32){
        for (int j = 0; j < width; j += 32){
            int i_end = (i + 32 < height) ? i + 32 : height;
            int j_end = (j + 32 < width) ? j + 32 : width;
            for (int ii = i; ii < i_end; ii++){
                for (int jj = j; jj < j_end; jj++){
                    int x = (int)(temp[jj * height + ii] + 0.5f);
                    dest[ii * width + jj] = x < 0 ? 0 : (x > 255 ? 255 : x);
                }
            }
        }
    }

    free(weights);
    free(temp);
    free(padded_src);
    free(tempT);
    free(padded_tempT);
}