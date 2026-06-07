#include "fastenoughimg.h"

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
