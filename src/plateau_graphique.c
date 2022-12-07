#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "SDL_ttf.h"

// gcc src/plateau_graphique.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

#define MENU_BUTTON_W 820
#define MENU_BUTTON_H 90
#define MENU_BUTTON_BORDER 12

int getRandomInt(int min, int max);

void ResetRender(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void printButton(SDL_Renderer *renderer, SDL_Surface *image, SDL_Texture *texture_button, SDL_Rect rect_button, const char* file);
void AfficheButton(SDL_Renderer *renderer,SDL_Surface *image, SDL_Texture *texture_button,SDL_Rect rect_button, const char* file, int colorButton[3], int px);
void AfficheMenu(SDL_Renderer *renderer,const char* file1, const char* file2, int etatSelection);

void AffichePlateau(SDL_Renderer *renderer,  SDL_Rect rect_plateau, SDL_Rect rect_plateau2, SDL_Rect rect_plateau3);
void printTuileItem(SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item, int i, int j);
void RandomTuileItem(SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item, int i, int j);
void AfficheTuileItem( SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item);
void AffichePlateauTuileItem(SDL_Renderer *renderer, SDL_Surface *image_tuile, SDL_Surface *image_item, SDL_Texture *texture_tuile, SDL_Texture *texture_item, SDL_Rect rect_tuile, SDL_Rect rect_item, SDL_Rect rect_plateau, SDL_Rect rect_plateau2, SDL_Rect rect_plateau3);
void printDebugGrid(SDL_Renderer *renderer);

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
  char tuile, item; // on utilise des char pour économiser la mémoire (1 char = 1 octet alors que 1 int = 4 octets)
}Case;

typedef struct{
    int posX, posY, itemFound;
}PlayerDATA;

Case SDLplateau[7][7] = { 1,0,   0,0,   7,1,   0,0,   7,2,   0,0,   4,0,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          8,3,   0,0,   8,4,   0,0,   7,5,   0,0,   6,6,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          8,7,   0,0,   5,8,   0,0,   6,9,   0,0,   6,10,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          2,0,   0,0,   5,11,  0,0,   5,12,  0,0,   3,0
};

Case tuileRestante;

PlayerDATA playerData[4] = { 0,0,0,   0,6,0,   6,0,0,   6,6,0 };

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

int colorButtonSelected[3] = {216, 192, 168};
int colorButtonNotSelected[3] = {229, 204, 178};

SDL_Rect rect_button_1 = {(1920-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (1080/2)-(3*MENU_BUTTON_BORDER)-MENU_BUTTON_H, 820+MENU_BUTTON_BORDER*2, 90+MENU_BUTTON_BORDER*2};
SDL_Rect rect_button_2 = {(1920-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (1080/2)+MENU_BUTTON_BORDER, 820+MENU_BUTTON_BORDER*2, 90+MENU_BUTTON_BORDER*2};

int main(int argc, char **argv)
{
    
    SDL_Window *window = NULL;

    SDL_Renderer *jeu = NULL;

//------------------------------------------------//

    SDL_Surface *image_tuile = NULL;
    SDL_Surface *image_item = NULL;

    SDL_Surface *image_exit = NULL;

//------------------------------------------------//

    SDL_Texture *texture_exit = NULL;

    SDL_Texture *texture_tuile = NULL;
    SDL_Texture *texture_item = NULL;

//------------------------------------------------//

    //SDL_Rect rect_exit = {(1920-300)/2, (1080-100)/2+125, 300, 100};

    SDL_Rect rect_plateau = {(1920-522)/2, (1080-522)/2, 522, 522};
    SDL_Rect rect_plateau2 = {771, 297 , 18, 486};
    SDL_Rect rect_plateau3 = {717, 351 , 486, 18};

    SDL_Rect rect_tuile = {717, 297, 54, 54};
    SDL_Rect rect_item = {736, 316, 16 , 16};

//------------------------------------------------//

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

    jeu = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if( jeu == NULL ){
        SDL_Log("Erreur render > %s\n",SDL_GetError());
        clean(window,NULL,NULL);
        exit(EXIT_FAILURE);        
    }

    ResetRender(jeu,255, 233, 210, 255);
    AfficheMenu(jeu, "images/button/newgame.bmp", "images/button/exitgame.bmp", 0);

    SDL_bool launched = SDL_TRUE;
    SDL_bool launched_game = SDL_FALSE;

    int x,y;

    while( launched ){
        //printDebugGrid(jeu);

        SDL_Event event;

        while( SDL_PollEvent(&event) ){


            switch( event.type ){
            case SDL_KEYDOWN:
                
                switch ( event.key.keysym.sym ){
                case SDLK_ESCAPE:
                    if(launched_game == SDL_FALSE) launched = SDL_FALSE; //ferme la fenêtre
                    else {
                        ResetRender(jeu,255, 233, 210, 255);
                        AfficheMenu(jeu, "images/button/newgame.bmp", "images/button/exitgame.bmp", 0);
                        launched_game = SDL_FALSE;
                    }
                    continue;
                case SDLK_0:
                    AffichePlateauTuileItem(jeu, 
                        image_tuile, image_item, 
                        texture_tuile, texture_item, 
                        rect_tuile, rect_item,rect_plateau, rect_plateau2, rect_plateau3);
                default:
                    continue;
                }

            case SDL_MOUSEMOTION:

                if( launched_game == SDL_TRUE ){
                    continue;
                }else{
                    x = event.motion.x;
                    y = event.motion.y;
                    if(x >= rect_button_1.x && x <= rect_button_1.x + rect_button_1.w && y >= rect_button_1.y && y <= rect_button_1.y + rect_button_1.h){
                        ResetRender(jeu,255, 233, 210, 255);
                        AfficheMenu(jeu, "images/button/newgame.bmp", "images/button/exitgame.bmp", 1);
                    } else if (x >= rect_button_2.x && x <= rect_button_2.x + rect_button_2.w && y >= rect_button_2.y && y <= rect_button_2.y + rect_button_2.h){
                        ResetRender(jeu,255, 233, 210, 255);
                        AfficheMenu(jeu, "images/button/newgame.bmp", "images/button/exitgame.bmp", 2);
                    } else{
                        ResetRender(jeu,255, 233, 210, 255);
                        AfficheMenu(jeu, "images/button/newgame.bmp", "images/button/exitgame.bmp", 0);
                    }
                    continue;
                }

            case SDL_MOUSEBUTTONDOWN:

                if ( launched_game == SDL_FALSE){

                    x = event.motion.x;
                    y = event.motion.y;

                    if(x >= rect_button_1.x && x <= rect_button_1.x + rect_button_1.w && y >= rect_button_1.y && y <= rect_button_1.y + rect_button_1.h){      
                        AffichePlateauTuileItem(jeu, 
                                image_tuile, image_item, 
                                texture_tuile, texture_item, 
                                rect_tuile, rect_item,rect_plateau, rect_plateau2, rect_plateau3);
                        launched_game = SDL_TRUE;
                    } else if(x >= rect_button_2.x && x <= rect_button_2.x + rect_button_2.w && y >= rect_button_2.y && y <= rect_button_2.y + rect_button_2.h){      
                        launched = SDL_FALSE; // ferme la fenêtre
                    }

                }break;  

            case SDL_QUIT:
                launched = SDL_FALSE;
                break;

            default:
                break;
            }

        }

    }

    SDL_DestroyTexture(texture_tuile);
    SDL_DestroyTexture(texture_item);
    
    SDL_DestroyRenderer(jeu);

    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}

void ResetRender(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a){

    SDL_SetRenderDrawColor(renderer, 255, 233, 210, 255);
    SDL_RenderClear(renderer);

}


void printButton(SDL_Renderer *renderer, SDL_Surface *image, SDL_Texture *texture_button, SDL_Rect rect_button, const char* file){

    image = SDL_LoadBMP(file);
    texture_button = SDL_CreateTextureFromSurface(renderer, image);

    SDL_FreeSurface(image);
    SDL_QueryTexture(texture_button, NULL, NULL, &rect_button.w, &rect_button.h);
    SDL_RenderCopy(renderer, texture_button, NULL, &rect_button);

}

void AfficheButton(SDL_Renderer *renderer,SDL_Surface *image, SDL_Texture *texture_button,SDL_Rect rect_button, const char* file, int colorButton[3], int px){

    SDL_SetRenderDrawColor(renderer, colorButton[0], colorButton[1], colorButton[2], 255);

    SDL_RenderFillRect(renderer,&rect_button);

    rect_button.x += px;
    rect_button.y += px;
    rect_button.h -= 2*px;
    rect_button.w -= 2*px;

    printButton(renderer, image, texture_button, rect_button, file);

}

void AfficheMenu(SDL_Renderer *renderer,const char* file1, const char* file2, int etatSelection){

    SDL_Surface *image = NULL;
    SDL_Texture *texture_button = NULL;

    if (etatSelection == 1){
        AfficheButton(renderer, image, texture_button, rect_button_1, file1, colorButtonSelected, MENU_BUTTON_BORDER);
    } else {
        AfficheButton(renderer, image, texture_button, rect_button_1, file1, colorButtonNotSelected, MENU_BUTTON_BORDER);
    }

    if (etatSelection == 2){
        AfficheButton(renderer, image, texture_button, rect_button_2, file2, colorButtonSelected, MENU_BUTTON_BORDER);
    } else {
        AfficheButton(renderer, image, texture_button, rect_button_2, file2, colorButtonNotSelected, MENU_BUTTON_BORDER);
    }

    SDL_RenderPresent(renderer);

}

void AffichePlateau(SDL_Renderer *renderer,  SDL_Rect rect_plateau, SDL_Rect rect_plateau2, SDL_Rect rect_plateau3){

    SDL_SetRenderDrawColor(renderer, 229, 204, 178, 255);
    for( int i=0 ; i<18 ; i++){
        SDL_RenderDrawRect(renderer,&rect_plateau);
        rect_plateau.h -= 2;
        rect_plateau.w -= 2;
        rect_plateau.x = (1920-(rect_plateau.w))/2;
        rect_plateau.y = (1080-(rect_plateau.h))/2;
    }

    for( int i=0 ; i<6 ; i++){
        SDL_RenderFillRect(renderer,&rect_plateau2);
        rect_plateau2.x += 4*18;
    }

    for( int i=0 ; i<6 ; i++){
        SDL_RenderFillRect(renderer,&rect_plateau3);
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

    ResetRender(renderer, 255, 233, 210, 255);
    AffichePlateau(renderer, rect_plateau, rect_plateau2, rect_plateau3);
    AfficheTuileItem(renderer, image_tuile, image_item, texture_tuile, texture_item, rect_tuile, rect_item);  
    SDL_RenderPresent(renderer);  

}

void printDebugGrid(SDL_Renderer *renderer){
    SDL_SetRenderDrawColor(renderer,0,0,0, SDL_ALPHA_OPAQUE);
    for(int i = 0; i<=1080; i+=60){
        SDL_RenderDrawLine(renderer,0,i,1920,i);
    }
    SDL_SetRenderDrawColor(renderer,255,0,0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer,0,540,1920,540);

    SDL_SetRenderDrawColor(renderer,0,0,0, SDL_ALPHA_OPAQUE);
    for(int i = 0; i<=1920; i+=60){
        SDL_RenderDrawLine(renderer,i,0,i,1080);
    }
    SDL_SetRenderDrawColor(renderer,255,0,0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer,960,0,960,1080);
    
    SDL_RenderPresent(renderer);
}

int checkDeplacement(int direction, int player){
    switch(direction){
        case 0: // le joueur va en haut
        if (playerData[player].posY > 0){ // si le joueur n'est pas tout en haut
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile;
            int tuileSuivante = SDLplateau[playerData[player].posX][playerData[player].posY-1].tuile;
            if (tuileActuelle == 1 || tuileActuelle == 2 || tuileActuelle == 4 || tuileActuelle == 5 || tuileActuelle == 7 || tuileActuelle == 8){ // si la tuile à un passage en haut
                if (tuileSuivante == 0 || tuileSuivante == 3 || tuileSuivante == 5 || tuileSuivante == 6 || tuileSuivante == 7 || tuileSuivante == 8){ // si la tuile suivante a un passage en bas
                    return 1;
                }else return 0;
            }else return 0;
        }
        case 1: // le joueur va à droite
        if (playerData[player].posX < 6){ // si le joueur n'est pas tout à droite
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile;
            int tuileSuivante = SDLplateau[playerData[player].posX+1][playerData[player].posY].tuile;
            if (tuileActuelle == 0 || tuileActuelle == 1 || tuileActuelle == 4 || tuileActuelle == 6 || tuileActuelle == 7 || tuileActuelle == 9){ // si la tuile à un passage en haut
                if (tuileSuivante == 2 || tuileSuivante == 3 || tuileSuivante == 4 || tuileSuivante == 5 || tuileSuivante == 6 || tuileSuivante == 9){ // si la tuile suivante a un passage en bas
                    return 1;
                }else return 0;
            }else return 0;
        }
        
    }
}

int getRandomInt(int min, int max){
  int i = (rand() % (max-min+1)) + min;
  return i;
}