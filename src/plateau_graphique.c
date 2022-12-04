#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

// gcc src/plateau_graphique.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2

void clean(SDL_Window *w, SDL_Renderer *r, SDL_Texture *t){
    
    if(t)
        SDL_DestroyTexture(t);
    if(r)
        SDL_DestroyRenderer(r);
    if(w)
        SDL_DestroyWindow(w);
    SDL_Quit();
}

void SDL_SetPlateauVide(SDL_Renderer *rendu,  SDL_Rect rect_plateau, SDL_Rect rect_plateau2, SDL_Rect rect_plateau3){

    SDL_SetRenderDrawColor(rendu, 229, 204, 178, 255);
    for( int i=0 ; i<18 ; i++){
        SDL_RenderDrawRect(rendu,&rect_plateau);
        rect_plateau.h -= 2;
        rect_plateau.w -= 2;
        rect_plateau.x = (1920-(rect_plateau.w))/2;
        rect_plateau.y = (1080-(rect_plateau.h))/2;
    }

    for( int i=0 ; i<6 ; i++){
        SDL_RenderFillRect(rendu,&rect_plateau2);
        rect_plateau2.x += 4*18;
    }

    for( int i=0 ; i<6 ; i++){
        SDL_RenderFillRect(rendu,&rect_plateau3);
        rect_plateau3.y += 4*18;
    }    

}

typedef struct{ 
  char  tuile, item; // on utilise des char pour économiser la mémoire (1 char = 1 octet alors que 1 int = 4 octets)
}Case;

Case SDLplateau[7][7] = { 1,1,   0,0,   7,2,   0,0,   7,3,   0,0,   4,4,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          8,5,   0,0,   8,6,   0,0,   7,7,   0,0,   6,8,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          8,9,   0,0,   5,10,  0,0,   6,11,  0,0,   6,12,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          2,13,  0,0,   5,14,  0,0,   5,15,  0,0,   3,16
};

char nbTuileRestant[4] = {6,6,10,12}; // 6 tuiles T avec trésor // 6 tuiles L avec trésor // 10 tuiles L vides // 12 tuiles I vides

const char chemin_tuile[10][19] = {"images/TuileL1.bmp",
                                   "images/TuileL2.bmp",          
                                   "images/TuileL3.bmp",                   
                                   "images/TuileL4.bmp",                   
                                   "images/TuileT1.bmp",                   
                                   "images/TuileT2.bmp",
                                   "images/TuileT3.bmp",                   
                                   "images/TuileT4.bmp",
                                   "images/TuileI1.bmp",
                                   "images/TuileI2.bmp"};

void printTuile(SDL_Renderer *renderer, SDL_Surface *image, SDL_Texture *texture, SDL_Rect rect, int i, int j){
    
    if ( (SDLplateau[i][j].tuile) == 0 ){
    
        while(1){
            
            int rand = getRandomInt(0,3);
            if(nbTuileRestant[rand]){

                switch ( rand )
                {
                case 0:
                    SDLplateau[i][j].tuile = getRandomInt(5,8);
                    nbTuileRestant[0] -= 1;
                    break;
                case 1:
                    SDLplateau[i][j].tuile = getRandomInt(1,4);
                    nbTuileRestant[1] -= 1;
                    break;

                case 2:
                    SDLplateau[i][j].tuile = getRandomInt(1,4);
                    nbTuileRestant[2] -= 1;
                    break;

                case 3:
                    SDLplateau[i][j].tuile = getRandomInt(9,10);
                    nbTuileRestant[3] -= 1;
                    break;
                }
            break;
            }
        
        }
    }
        
    image = SDL_LoadBMP(chemin_tuile[(SDLplateau[i][j].tuile)-1]); 
    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void AffichePlateauTuile( SDL_Renderer *renderer, SDL_Surface *image, SDL_Texture *texture, SDL_Rect rect){

    char nbTuileRestant[4] = {6,6,10,12}; // 6 tuiles T avec trésor // 6 tuiles L avec trésor // 10 tuiles L vides // 12 tuiles I vides

    srand(time(NULL));

    for( int i=0 ; i<7 ; i++ ){
        rect.x = 717;
        for( int j=0 ; j<7 ; j++){
            printTuile(renderer, image, texture, rect, i, j);
            rect.x += 4*18;
        }
        rect.y += 4*18;
    }
}

int main(int argc, char **argv)
{
    
    SDL_Window *window = NULL;
    SDL_Renderer *rendu = NULL;

    SDL_Surface *image_tuile = NULL;
    SDL_Texture *texture_tuile = NULL;

    SDL_Rect rect_plateau = {(1920-522)/2, (1080-522)/2, 522, 522};
    SDL_Rect rect_plateau2 = {771, 297 , 18, 486};
    SDL_Rect rect_plateau3 = {717, 351 , 486, 18};
    SDL_Rect rect_tuile = {717, 297, 54, 54};

    if(SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Erreur init > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Labyrinthe-Z", 0, 0, 1980, 1080, SDL_WINDOW_FULLSCREEN);
    if( window == NULL ){
        SDL_Log("Erreur create_window > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);        
    }

    rendu = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if( rendu == NULL ){
        SDL_Log("Erreur render > %s\n",SDL_GetError());
        clean(window,NULL,NULL);
        exit(EXIT_FAILURE);        
    }

    SDL_SetRenderDrawColor(rendu, 255, 233, 210, 255);
    SDL_RenderClear(rendu);

    SDL_SetPlateauVide(rendu, rect_plateau, rect_plateau2, rect_plateau3);

    SDL_bool launched = SDL_TRUE;

    while( launched ){

        SDL_Event event;

        while( SDL_PollEvent(&event) ){

            switch( event.type ){
            case SDL_KEYDOWN:
                
                switch ( event.key.keysym.sym ){
                case SDLK_ESCAPE:
                    launched = SDL_FALSE;
                    break;
                default:
                    continue;
                }

            case SDL_QUIT:
                launched = SDL_FALSE;
                break;

            default:
                // FONCTION POUR AFFICHER LA TUILE

                AffichePlateauTuile(rendu, image_tuile, texture_tuile, rect_tuile);

                // FONCTION POUR AFFICHER LA TUILE

                SDL_RenderPresent(rendu);

                break;
            }

        }

    }

    SDL_DestroyTexture(texture_tuile);
    SDL_DestroyRenderer(rendu);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
