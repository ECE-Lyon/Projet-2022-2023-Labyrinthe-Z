#include "library.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

// gcc src/plateau_graphique.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2

/*void sdl_ellipse(SDL_Renderer* r, int x0, int y0, int radiusX, int radiusY) // FAIS UN CERCLE
{
    float pi  = 3.14159265358979323846264338327950288419716939937510;
    float pih = pi / 2.0; //half of pi

    //drew  28 lines with   4x4  circle with precision of 150 0ms
    //drew 132 lines with  25x14 circle with precision of 150 0ms
    //drew 152 lines with 100x50 circle with precision of 150 3ms
    const int prec = 27; // precision value; value of 1 will draw a diamond, 27 makes pretty smooth circles.
    float theta = 0;     // angle that will be increased each loop

    //starting point
    int x  = (float)radiusX * cos(theta);//start point
    int y  = (float)radiusY * sin(theta);//start point
    int x1 = x;
    int y1 = y;

    //repeat until theta >= 90;
    float step = pih/(float)prec; // amount to add to theta each time (degrees)
    for(theta=step;  theta <= pih;  theta+=step)//step through only a 90 arc (1 quadrant)
    {
        //get new point location
        x1 = (float)radiusX * cosf(theta) + 0.5; //new point (+.5 is a quick rounding method)
        y1 = (float)radiusY * sinf(theta) + 0.5; //new point (+.5 is a quick rounding method)

        //draw line from previous point to new point, ONLY if point incremented
        if( (x != x1) || (y != y1) )//only draw if coordinate changed
        {
            //SDL_RenderDrawLine(r, x0 + x, y0 - y,    x0 + x1, y0 - y1 );//quadrant TR
            //SDL_RenderDrawLine(r, x0 - x, y0 - y,    x0 - x1, y0 - y1 );//quadrant TL
            //SDL_RenderDrawLine(r, x0 - x, y0 + y,    x0 - x1, y0 + y1 );//quadrant BL
            SDL_RenderDrawLine(r, x0 + x, y0 + y,    x0 + x1, y0 + y1 );//quadrant BR
        }
        //save previous points
        x = x1;//save new previous point
        y = y1;//save new previous point
    }
    //arc did not finish because of rounding, so finish the arc
    if(x!=0)
    {
        x=0;
        //SDL_RenderDrawLine(r, x0 + x, y0 - y,    x0 + x1, y0 - y1 );//quadrant TR
        //SDL_RenderDrawLine(r, x0 - x, y0 - y,    x0 - x1, y0 - y1 );//quadrant TL
        //SDL_RenderDrawLine(r, x0 - x, y0 + y,    x0 - x1, y0 + y1 );//quadrant BL
        SDL_RenderDrawLine(r, x0 + x, y0 + y,    x0 + x1, y0 + y1 );//quadrant BR
    }
}*/

void clean(SDL_Window *w, SDL_Renderer *r, SDL_Texture *t){
    
    if(t)
        SDL_DestroyTexture(t);
    if(r)
        SDL_DestroyRenderer(r);
    if(w)
        SDL_DestroyWindow(w);
    SDL_Quit();
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

void AffichePlateau(SDL_Renderer *rendu,  SDL_Rect rect_plateau, SDL_Rect rect_plateau2, SDL_Rect rect_plateau3){

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


void printTuileItem(SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item, int i, int j){

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

void RandomTuileItem(SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item, int i, int j){
    
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
        
    printTuileItem(renderer, image_tuile, image_item, texture_tuile, texture_item, rect_tuile, rect_item,i, j);

}

void AfficheTuileItem( SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item){

    srand(time(NULL));

    for( int i=0 ; i<7 ; i++ ){
        rect_tuile.x = 717;
        rect_item.x = 736;
        for( int j=0 ; j<7 ; j++){
            RandomTuileItem(renderer, image_tuile, image_item, texture_tuile, texture_item, rect_tuile, rect_item,i, j);
            rect_tuile.x += 4*18;
            rect_item.x += 4*18;
        }
        rect_tuile.y += 4*18;
        rect_item.y += 4*18;
    }
}

void AffichePlateauTuileItem(SDL_Renderer *renderer, 
                             SDL_Surface *image_tuile, SDL_Surface *image_item, 
                             SDL_Texture *texture_tuile, SDL_Texture *texture_item, 
                             SDL_Rect rect_tuile, SDL_Rect rect_item, SDL_Rect rect_plateau, SDL_Rect rect_plateau2, SDL_Rect rect_plateau3){

    AffichePlateau(renderer, rect_plateau, rect_plateau2, rect_plateau3);
    AfficheTuileItem(renderer, image_tuile, image_item, texture_tuile, texture_item, rect_tuile, rect_item);    

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

    rendu = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if( rendu == NULL ){
        SDL_Log("Erreur render > %s\n",SDL_GetError());
        clean(window,NULL,NULL);
        exit(EXIT_FAILURE);        
    }

    SDL_SetRenderDrawColor(rendu, 255, 233, 210, 255);
    SDL_RenderClear(rendu);

    AffichePlateauTuileItem(rendu, 
                            image_tuile, image_item, 
                            texture_tuile, texture_item, 
                            rect_tuile, rect_item,rect_plateau, rect_plateau2, rect_plateau3);

    //sdl_ellipse(rendu, 100, 100, 45,45); // TEST CERCLE

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
