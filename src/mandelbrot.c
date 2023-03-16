#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <math.h>

#define WIDTH 1200
#define HEIGHT 900
#define MAX_ITERATIONS 10000
#define COLOR_DEPTH 200
#define GRADIENT_FADE 2.5

#define X1 -2.0
#define X2 1.0
#define Y1 -1.5
#define Y2 1.5

typedef struct {
    double x;
    double y;
} Complex;

void couleur(int* r, int* g, int* b, int index) {

    int index_resize = pow(MAX_ITERATIONS*(double)index, 1.0/GRADIENT_FADE);

    double index2 = index_resize % COLOR_DEPTH;

    if (index == MAX_ITERATIONS){
        *r = 0;
        *g = 0;
        *b = 0;
    }else if (index2 <= COLOR_DEPTH/3) {
        *r = 255;
        *g = (index2*(double)255/((double)COLOR_DEPTH/3));
        *b = 255-(index2*(double)255/((double)COLOR_DEPTH/3));
    } else if (index2 <= COLOR_DEPTH*2/3){
        *r = 255-(index2*(double)255/((double)COLOR_DEPTH/3));
        *g = 255;
        *b = (index2*(double)255/((double)COLOR_DEPTH/3));
    } else{
        *r = (index2*(double)255/((double)COLOR_DEPTH/3));
        *g = 255-(index2*(double)255/((double)COLOR_DEPTH/3));
        *b = 255;
    }
}

// Définition de la formule pour la suite
int suite(int n) {

    int tab[12] = {0,6,3,9,2,4,8,10,1,5,7,11};
    
    return tab[n];
}

int main(int argc, char **argv){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Mandelbrot Set",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
    bool running = true;
    bool done = false;

    bool screen[WIDTH][HEIGHT] = {0};

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        if (done) continue;
        for (int n = 0; n < 12; n++){
            int ligne = suite(n);
            for (int n2 = 0; n2 < 12; n2++){
                int ligne2 = suite(n2);
                for (int y = ligne + ligne2-12; y < HEIGHT; y+=12) {
                    for (int x = ligne2; x < WIDTH; x+=12) {
                        if (screen[x-1][y-1]) break;
                        Complex c = {
                            X1 + (X2-X1) * (double)x / (double)WIDTH,
                            Y1 + (Y2-Y1) * (double)y / (double)HEIGHT
                        };
                        Complex z = {0.0, 0.0};
                        int i = 0;
                        while (i < MAX_ITERATIONS) {
                            Complex tmp = {
                                z.x * z.x - z.y * z.y + c.x,
                                2.0 * z.x * z.y + c.y
                            };
                            z = tmp;
                            i++;
                            if (z.x * z.x + z.y * z.y > 4.0) break;
                        }

                        int r, g, b;

                        couleur(&r, &g, &b, i);

                        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                        SDL_RenderDrawPoint(renderer, x, y);
                        screen[x-1][y-1] = true;
                    }
                    for (int x = WIDTH-ligne2-1; x >= -11; x-=12) {
                        if (screen[x-1][y-1]) break;
                        Complex c = {
                            X1 + (X2-X1) * (double)x / (double)WIDTH,
                            Y1 + (Y2-Y1) * (double)y / (double)HEIGHT
                        };
                        Complex z = {0.0, 0.0};
                        int i = 0;
                        while (i < MAX_ITERATIONS) {
                            Complex tmp = {
                                z.x * z.x - z.y * z.y + c.x,
                                2.0 * z.x * z.y + c.y
                            };
                            z = tmp;
                            i++;
                            if (z.x * z.x + z.y * z.y > 4.0) break;
                        }

                        int r, g, b;

                        couleur(&r, &g, &b, i);

                        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                        SDL_RenderDrawPoint(renderer, x, y);
                        screen[x-1][y-1] = true;
                    }
                    SDL_RenderPresent(renderer);
                }
                printf("%0.1f%% \n", (float)(n*12+n2)/144.0f*100.0f);
            } 
        }
        
        done = true;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}