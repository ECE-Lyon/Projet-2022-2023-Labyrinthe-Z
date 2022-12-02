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
        SDL_Log("Erreur init > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Labyrinthe-Z", 0, 0, 1980, 1080, SDL_WINDOW_MAXIMIZED);
    if( window == NULL ){
        SDL_Log("Erreur create_window > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);        
    }

    rendu_image = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if( rendu_image == NULL ){
        SDL_Log("Erreur render_background > %s\n",SDL_GetError());
        clean(window,NULL,NULL);
        exit(EXIT_FAILURE);        
    }    

    image = SDL_LoadBMP("src/test.bmp");
    if( image == NULL ){
        SDL_Log("Erreur load_image > %s\n",SDL_GetError());
        clean(window,rendu_image,NULL);
        exit(EXIT_FAILURE);
    }

    texture_image = SDL_CreateTextureFromSurface(rendu_image,image);
    SDL_FreeSurface(image);
    if( texture_image == NULL ){
        SDL_Log("Erreur creation texture > %s\n",SDL_GetError());
        clean(window,rendu_image,NULL);
        exit(EXIT_FAILURE);        
    }

    if( SDL_QueryTexture(texture_image, NULL, NULL, &rect_image.w, &rect_image.h) ){
        SDL_Log("Erreur querytexture > %s\n",SDL_GetError());
        clean(window, rendu_image, texture_image);
        exit(EXIT_FAILURE);         
    }

    rect_image.x = (1920 - rect_image.w) / 2;       // REVOIR POUR CENTRER
    rect_image.y = (1080 - rect_image.h) / 2; 


    if( SDL_RenderCopy(rendu_image, texture_image, NULL, &rect_image) ){
        SDL_Log("Erreur rendercopy > %s\n",SDL_GetError());
        clean(window, rendu_image, texture_image);
        exit(EXIT_FAILURE); 
    }
    
    SDL_RenderPresent(rendu_image);
    SDL_Delay(5000);

    return 0;
}
