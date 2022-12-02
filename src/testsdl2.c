#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

void clean(SDL_Window *w, SDL_Renderer *r, SDL_Texture *t){
    
    if(t)
        SDL_DestroyTexture(t);
    if(r)
        SDL_DestroyRenderer(r);
    if(w)
        SDL_DestroyWindow(w);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    
    SDL_Window *window = NULL;
    SDL_Renderer *rendu_image = NULL;
    SDL_Renderer *rendu_background = NULL;
    SDL_Surface *image = NULL;
    SDL_Texture *texture_image = NULL;
    SDL_Rect rect_image = {0,0,257,261};

    if(SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Erreur > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Labyrinthe-Z", 0, 0, 1980, 1080, SDL_WINDOW_MAXIMIZED);
    if( window == NULL ){
        SDL_Log("Erreur > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);        
    }

    rendu_background = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if( rendu_background == NULL ){
        SDL_Log("Erreur > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);        
    }    

    return 0;
}
