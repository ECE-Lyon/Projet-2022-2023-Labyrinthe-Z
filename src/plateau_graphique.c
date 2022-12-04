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

Case SDLplateau[7][7] = { 1,0,   0,0,   7,1,   0,0,   7,2,   0,0,   4,0,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          8,3,   0,0,   8,4,   0,0,   7,5,   0,0,   6,6,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          8,7,   0,0,   5,8,   0,0,   6,9,   0,0,   6,10,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          2,0,   0,0,   5,11,  0,0,   5,12,  0,0,   3,0
};

char nbTuileRestant[4] = {6,6,10,12}; // 6 tuiles T avec trésor // 6 tuiles L avec trésor // 10 tuiles L vides // 12 tuiles I vides

char nbItemRestant[24] = {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1};

const char chemin_tuile[10][26] = {"images/tuiles/TuileL1.bmp",
                                   "images/tuiles/TuileL2.bmp",          
                                   "images/tuiles/TuileL3.bmp",                   
                                   "images/tuiles/TuileL4.bmp",                   
                                   "images/tuiles/TuileT1.bmp",                   
                                   "images/tuiles/TuileT2.bmp",
                                   "images/tuiles/TuileT3.bmp",                   
                                   "images/tuiles/TuileT4.bmp",
                                   "images/tuiles/TuileI1.bmp",
                                   "images/tuiles/TuileI2.bmp"};

const char chemin_item[24][28] = {"images/items16px/iitem1.bmp", // MAGMA
                                  "images/items16px/iitem2.bmp", // COOKIE
                                  "images/items16px/iitem3.bmp", // EMERAUDE
                                  "images/items16px/iitem4.bmp", // PASTEQUE
                                  "images/items16px/iitem5.bmp", // LIVRE
                                  "images/items16px/iitem6.bmp", // POMME
                                  "images/items16px/iitem7.bmp", // CAROTTE
                                  "images/items16px/iitem8.bmp", // DIAMANT
                                  "images/items16px/iitem9.bmp", // PEARL
                                  "images/items16px/item10.bmp", // NEMO
                                  "images/items16px/item11.bmp", // NETHERSTAR            
                                  "images/items16px/item12.bmp", // DISQUE
                                  "images/items16px/item13.bmp", // FEUILLE
                                  "images/items16px/item14.bmp", // LASSO
                                  "images/items16px/item15.bmp", // BERRIES
                                  "images/items16px/item16.bmp", // POTION
                                  "images/items16px/item17.bmp", // XP BOTTLE
                                  "images/items16px/item18.bmp", // PAPIER
                                  "images/items16px/item19.bmp", // POISSON BOULE
                                  "images/items16px/item20.bmp", // OEUF
                                  "images/items16px/item21.bmp", // FLECHE
                                  "images/items16px/item22.bmp", // TARTE
                                  "images/items16px/item23.bmp", // BOUSSOLE
                                  "images/items16px/item24.bmp"};// POULET

void printTuile(SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item, int i, int j){
    
    if ( (SDLplateau[i][j].tuile) == 0 ){
    
        while(1){
            
            int rand = getRandomInt(0,3);
            if(nbTuileRestant[rand]){

                switch ( rand )
                {
                case 0:

                    SDLplateau[i][j].tuile = getRandomInt(5,8);
                    while(1){

                        int rand_item = getRandomInt(1,24);
                        if(nbItemRestant[rand_item-1]){
                            SDLplateau[i][j].item = rand_item;
                            nbItemRestant[rand_item-1] = 0;
                        break;
                        }
                    }
                    
                    nbTuileRestant[0] -= 1;
                    break;
                case 1:

                    SDLplateau[i][j].tuile = getRandomInt(1,4);
                    while(1){

                        int rand_item = getRandomInt(1,24);
                        if(nbItemRestant[rand_item-1]){
                            SDLplateau[i][j].item = rand_item;
                            nbItemRestant[rand_item-1] = 0;
                        break;
                        }
                    }
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
        
    image_tuile = SDL_LoadBMP(chemin_tuile[(SDLplateau[i][j].tuile)-1]); 
    texture_tuile = SDL_CreateTextureFromSurface(renderer, image_tuile);
    SDL_FreeSurface(image_tuile);
    SDL_QueryTexture(texture_tuile, NULL, NULL, &rect_tuile.w, &rect_tuile.h);
    SDL_RenderCopy(renderer, texture_tuile, NULL, &rect_tuile);

    image_item = SDL_LoadBMP(chemin_item[(SDLplateau[i][j].item)-1]);
    texture_item = SDL_CreateTextureFromSurface(renderer, image_item);
    SDL_FreeSurface(image_item);
    SDL_QueryTexture(texture_item, NULL, NULL, &rect_item.w, &rect_item.h);
    SDL_RenderCopy(renderer, texture_item, NULL, &rect_item);

}

void AffichePlateauTuile( SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item){

    srand(time(NULL));

    for( int i=0 ; i<7 ; i++ ){
        rect_tuile.x = 717;
        rect_item.x = 736;
        for( int j=0 ; j<7 ; j++){
            printTuile(renderer, image_tuile, image_item, texture_tuile, texture_item, rect_tuile, rect_item,i, j);
            rect_tuile.x += 4*18;
            rect_item.x += 4*18;
        }
        rect_tuile.y += 4*18;
        rect_item.y += 4*18;
    }
}

int main(int argc, char **argv)
{
    
    SDL_Window *window = NULL;
    SDL_Renderer *rendu = NULL;

    SDL_Surface *image_tuile = NULL;
    SDL_Surface *image_item = NULL;

    SDL_Texture *texture_tuile = NULL;
    SDL_Texture *texture_item = NULL;

    SDL_Rect rect_plateau = {(1920-522)/2, (1080-522)/2, 522, 522};
    SDL_Rect rect_plateau2 = {771, 297 , 18, 486};
    SDL_Rect rect_plateau3 = {717, 351 , 486, 18};
    SDL_Rect rect_tuile = {717, 297, 54, 54};
    SDL_Rect rect_item = {736, 316, 16 , 16};

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

    AffichePlateauTuile(rendu, image_tuile, image_item, texture_tuile, texture_item, rect_tuile, rect_item);

    SDL_RenderPresent(rendu);

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
