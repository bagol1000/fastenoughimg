#include "fastenoughimg.h"


////////// funkcje pomocnicze ///////////
/////////////////////////////////////////
//funkcja dowybierania poprawnych sąsiednich pikseli
int reflect(int idx, int size){
    if (idx < 0) return -idx;
    if (idx >= size) return 2 * (size - 1) - idx;
    return idx;
}
/////////////////////////////////////////

//negatyw obrazu'
void invert(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, int channels){
    int n = width * height * channels;
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

    uint8_t remap[256];
    for (int i = 0; i < 256; i++){
        int r = ((long)(cumsum[i] - cum_min) * 255) / (n - cum_min); //normalizacja
        remap[i] = r < 0 ? 0 : r;
    }

    for (int i = 0; i < n; i++) dest[i] = remap[src[i]];
}

//progowanie Otsu - wybor wartosci granicznej ktora najlepiej rozdziela piksele na dwie grupy (0 albo 255)
int threshold_otsu(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height){
    int shade_count[256] = {0};
    int n = width * height;
    for (int i = 0; i < n; i++) shade_count[src[i]]++;

    double var = -1.0;
    int treshold = 0;

    //liczymy liczbe pikseli w grupie < i w grupie > threshold
    //ich sumaryczną jasność a potem średnią
    for (int tr = 0; tr < 256; tr++){
        int dark_count = 0, bright_count = 0;
        long dark_sum = 0, bright_sum = 0;

        for (int i = 0; i <= tr; i++){
            dark_count += shade_count[i];
            dark_sum += (long)i * shade_count[i];
        }

        for (int i = tr + 1; i < 256; i++){
            bright_count += shade_count[i];
            bright_sum += (long)i * shade_count[i];
        }

        if (dark_count == 0 || bright_count == 0) continue;

        double mean_dark = (double)dark_sum / dark_count;
        double mean_bright = (double)bright_sum / bright_count;

        double v = (double)dark_count * bright_count * (mean_dark - mean_bright) * (mean_dark - mean_bright);

        if (v > var) {
            var = v;
            treshold = tr;
        }
    }
    for (int i = 0; i < n; i++) dest[i] = src[i] > treshold ? 255 : 0;
    return treshold;
}

//detekcja krawedzi metoda Sobela
void edge_detect(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height){
    const int kernel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    const int kernel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            int grad_x = 0, grad_y = 0;

            for (int k_row = 0; k_row < 3; k_row++){
                for (int k_col = 0; k_col < 3; k_col++){
                    int neighbour_row = reflect(row + k_row - 1, height);
                    int neighbour_col = reflect(col + k_col - 1, width);

                    int shade = src[neighbour_row * width + neighbour_col];
                    grad_x += kernel_x[k_row * 3 + k_col] * shade;
                    grad_y += kernel_y[k_row * 3 + k_col] * shade;
                }
            }
            int s = (int)sqrt(grad_x * grad_x + grad_y * grad_y);
            dest[row * width + col] = s > 255 ? 255 : s;
        }
    }
}

//obliczanie sredniej wazonej dla kazdego piksela na podstawie jego otoczenia oraz podanej macierzy
//moze byc wykorzystywane do wyostrznia, rozmywania itp.
void convolve(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, const float* restrict kernel, int k_width, int k_height){
    int radius_h = k_height / 2; //promien zasiegu od pixela
    int radius_w = k_width / 2;

    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            float pixel_sum = 0.0f;
            for (int k_row = 0; k_row < k_height; k_row++){
                for (int k_col = 0; k_col < k_width; k_col++){
                    int neighbour_row = reflect(row + k_row - radius_h, height);
                    int neighbour_col = reflect(col + k_col - radius_w, width);

                    pixel_sum += kernel[k_row * k_width + k_col] * src[neighbour_row * width + neighbour_col];
                }
            }
            int z = (int)(pixel_sum + 0.5f); //zaokraglenie w gore bo kompilator zawsze w dol
            dest[row * width + col] = z < 0 ? 0 : (z > 255 ? 255 : z);
        }
    }
}

void sharpen(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height){
    const float kernel[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
    convolve(src, dest, width, height, kernel, 3, 3);
}

void blur_gauss(const uint8_t* restrict src, uint8_t* restrict dest, int width, int height, double sigma){
    int r = (int)(3.0 * sigma + 0.5);//z zaokragleniem
    int kernel_size = 2 * r + 1;
    double* weights = (double*)malloc(kernel_size * sizeof(double));
    double weight_sum = 0.0;

    //wektor wag
    for (int i = 0; i < kernel_size; i++){
        weights[i] = exp(-(i - r) * (i - r) / (2.0 * sigma * sigma));
        weight_sum += weights[i];
    }

    for (int i = 0; i < kernel_size; i++) weights[i] /= weight_sum;

    double* temp = (double*)malloc(width * height * sizeof(double));

    //przejscie po wierszach
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            double pixel_sum = 0.0;
            for (int i = 0; i < kernel_size; i++){
                int neighbour_col = reflect(col + i - r, width);
                pixel_sum += weights[i] * src[row * width + neighbour_col];
            }
            temp[row * width + col] = pixel_sum;
        }
    }

    //przejscie po kolumnach
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            double pixel_sum = 0.0;
            for (int i = 0; i < kernel_size; i++){
                int neighbour_row = reflect(row + i - r, height);
                pixel_sum += weights[i] * temp[neighbour_row * width + col];
            }
            int x = (int)(pixel_sum + 0.5);
            dest[row * width + col] = x < 0 ? 0 : (x > 255 ? 255 : x);
        }
    }
    free(weights);
    free(temp);
}