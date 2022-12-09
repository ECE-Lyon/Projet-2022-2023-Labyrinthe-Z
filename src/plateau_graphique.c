#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "SDL_ttf.h"
#include <SDL_mixer.h>

// gcc src/plateau_graphique.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

#define MENU_BUTTON_W 820
#define MENU_BUTTON_H 90
#define MENU_BUTTON_BORDER 12

typedef struct{
    Uint8 r, g, b, a;
}Color;

int getRandomInt(int min, int max);

void ResetRender(SDL_Renderer *renderer, Color color);
void AfficheButton(SDL_Renderer *renderer,SDL_Surface *image, SDL_Texture *texture_button,SDL_Rect rect_button, const char* file, Color color, int px);
void fenetreMenu(SDL_Renderer *renderer);
void SearchTuile();
void AffichePlateau(SDL_Renderer *renderer);
void RandomPlateau();
void AfficheTuileItem( SDL_Renderer *renderer);
void AffichePlateauTuileItem(SDL_Renderer *renderer);
void printDebugGrid(SDL_Renderer *renderer);
int movePlayer(int player, int direction);
void printImage(SDL_Renderer *renderer, SDL_Rect rect_image, const char chemin_image[28]);
void afficherPlateau(SDL_Renderer *renderer);

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

Case tuileRestante = {0,0};

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

Color Background = {255, 233, 210, 255};
Color ButtonSelected = {216, 192, 168, 255};
Color ButtonNotSelected = {229, 204, 178, 255};

int colorButtonNotSelected[3] = {229, 204, 178};

SDL_Rect rect_button_1 = {(1920-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (1080/2)-(3*MENU_BUTTON_BORDER)-MENU_BUTTON_H, 820+MENU_BUTTON_BORDER*2, 90+MENU_BUTTON_BORDER*2};
SDL_Rect rect_button_2 = {(1920-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (1080/2)+MENU_BUTTON_BORDER, 820+MENU_BUTTON_BORDER*2, 90+MENU_BUTTON_BORDER*2};

int main(int argc, char **argv)
{
    
    SDL_Window *window = NULL;
    SDL_Renderer *jeu = NULL;

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

    fenetreMenu(jeu);

    /*SDL_bool windowOpen = SDL_TRUE;
    SDL_bool launched_game = SDL_FALSE;

    int x,y;

    while( windowOpen ){
        
        SDL_Event event;

        while( SDL_PollEvent(&event) ){

            switch( event.type ){
            case SDL_KEYDOWN:
                
                switch ( event.key.keysym.sym ){
                case SDLK_ESCAPE:
                    if(launched_game == SDL_FALSE) windowOpen = SDL_FALSE; //ferme la fenêtre
                    else {
                        ResetRender(jeu, Background);
                        AfficheMenu(jeu, 0);
                        launched_game = SDL_FALSE;
                    }
                    continue;
                case SDLK_0:
                    AffichePlateauTuileItem(jeu,launched_game);
                    break;
                case SDLK_UP:
                    movePlayer(0, 0);
                    AffichePlateauTuileItem(jeu,launched_game);
                    break;
                case SDLK_RIGHT:
                    movePlayer(0, 1);
                    AffichePlateauTuileItem(jeu,launched_game);
                    break;
                case SDLK_DOWN:
                    movePlayer(0, 2);
                    AffichePlateauTuileItem(jeu,launched_game);
                    break;
                case SDLK_LEFT:
                    movePlayer(0, 3);
                    AffichePlateauTuileItem(jeu,launched_game);
                    break;
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
                        ResetRender(jeu, Background);
                        AfficheMenu(jeu, 1);
                    } else if (x >= rect_button_2.x && x <= rect_button_2.x + rect_button_2.w && y >= rect_button_2.y && y <= rect_button_2.y + rect_button_2.h){
                        ResetRender(jeu, Background);
                        AfficheMenu(jeu, 2);
                    } else{
                        ResetRender(jeu, Background);
                        AfficheMenu(jeu, 0);
                    }
                    continue;
                }

            case SDL_MOUSEBUTTONDOWN:

                if ( launched_game == SDL_FALSE ){

                    x = event.motion.x;
                    y = event.motion.y;

                    if(x >= rect_button_1.x && x <= rect_button_1.x + rect_button_1.w && y >= rect_button_1.y && y <= rect_button_1.y + rect_button_1.h){                          
                        AffichePlateauTuileItem(jeu,launched_game);
                        launched_game = SDL_TRUE;
                    } else if(x >= rect_button_2.x && x <= rect_button_2.x + rect_button_2.w && y >= rect_button_2.y && y <= rect_button_2.y + rect_button_2.h){      
                        windowOpen = SDL_FALSE; // ferme la fenêtre
                    }

                }break;  

            case SDL_QUIT:
                windowOpen = SDL_FALSE;
                break;

            default:
                break;
            }

        }
    
    }*/
    
    SDL_DestroyRenderer(jeu);

    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}

void fenetreMenu(SDL_Renderer *renderer){

    SDL_Surface *image = NULL;
    SDL_Texture *texture_button = NULL;

    SDL_Rect rect_button_1 = {(1920-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (1080/2)-(3*MENU_BUTTON_BORDER)-MENU_BUTTON_H, 820+MENU_BUTTON_BORDER*2, 90+MENU_BUTTON_BORDER*2};
    SDL_Rect rect_button_2 = {(1920-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (1080/2)+MENU_BUTTON_BORDER, 820+MENU_BUTTON_BORDER*2, 90+MENU_BUTTON_BORDER*2};

    SDL_bool windowOpen = SDL_TRUE;

    int cursorX, cursorY;

    uint8_t cursorInButton = 0;

    ResetRender(renderer, Background);
    AfficheButton(renderer, image, texture_button, rect_button_1, "images/button/newgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
    AfficheButton(renderer, image, texture_button, rect_button_2, "images/button/exitgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
    SDL_RenderPresent(renderer); 

    while( windowOpen ){
        
        SDL_Event event;

        while( SDL_PollEvent(&event) ){

            switch( event.type ){
            case SDL_KEYDOWN:               
                switch ( event.key.keysym.sym ){
                case SDLK_ESCAPE:
                    windowOpen = SDL_FALSE; //ferme la fenêtre
                    break;
                case SDLK_0:
                    break;
                default:
                    continue;
                }
            case SDL_MOUSEMOTION:
                cursorX = event.motion.x;
                cursorY = event.motion.y;
                if(cursorX >= rect_button_1.x && cursorX <= rect_button_1.x + rect_button_1.w && cursorY >= rect_button_1.y && cursorY <= rect_button_1.y + rect_button_1.h){
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, image, texture_button, rect_button_1, "images/button/newgame.bmp", ButtonSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, image, texture_button, rect_button_2, "images/button/exitgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 1;
                } else if (cursorX >= rect_button_2.x && cursorX <= rect_button_2.x + rect_button_2.w && cursorY >= rect_button_2.y && cursorY <= rect_button_2.y + rect_button_2.h){
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, image, texture_button, rect_button_1, "images/button/newgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, image, texture_button, rect_button_2, "images/button/exitgame.bmp", ButtonSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 2;
                } else{
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, image, texture_button, rect_button_1, "images/button/newgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, image, texture_button, rect_button_2, "images/button/exitgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 0;
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(cursorInButton == 1){                       
                    RandomPlateau();
                    afficherPlateau(renderer);
                } else if(cursorInButton == 2){      
                    windowOpen = SDL_FALSE; // ferme la fenêtre
                }
                break;  
            case SDL_QUIT:
                windowOpen = SDL_FALSE;
                break;
            default:
                continue;
            } 
        }   
    }
}

void afficherPlateau(SDL_Renderer *renderer){
    SDL_bool windowOpen = SDL_TRUE;

    AffichePlateauTuileItem(renderer);

    while( windowOpen ){
        
        SDL_Event event;

        while( SDL_PollEvent(&event) ){

            switch( event.type ){
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ){
                case SDLK_UP:
                    printf("haut");
                    movePlayer(0, 0);
                    AffichePlateauTuileItem(renderer);
                    break;
                case SDLK_RIGHT:
                    movePlayer(0, 1);
                    AffichePlateauTuileItem(renderer);
                    break;
                case SDLK_DOWN:
                    movePlayer(0, 2);
                    AffichePlateauTuileItem(renderer);
                    break;
                case SDLK_LEFT:
                    movePlayer(0, 3);
                    AffichePlateauTuileItem(renderer);
                    break;
                default:
                    continue;
                }
            case SDL_QUIT:
                windowOpen = SDL_FALSE;
                break;
            default:
                continue;
            }
        }
    }
}

void SearchTuile(){

    for( int i=0 ; i<3 ; i++ ){
        if(nbTuileRestant[i]){
            switch ( i ){

            case 0: // TUILE T item
                tuileRestante.tuile = 5; 
                for( int j=12 ; j<24 ; j++ ){
                    if( nbItemRestant[j] ){
                        tuileRestante.item = j+1;
                        break;
                    }
                }
            break;
            case 1: // TUILE L item
                tuileRestante.tuile = 1;
                for( int j=12 ; j<24 ; j++ ){
                    if( nbItemRestant[j] ){
                        tuileRestante.item = j+1;
                        break;
                    }
                }
            break;
            case 2: // TUILE L vide
                tuileRestante.tuile = 1;
                tuileRestante.item = 0;
            break;
            case 3: // TUILE I VIDE
                tuileRestante.tuile = 9;
                tuileRestante.item = 0;
            break;
            }
            break;
        }
    }
    //printf("%d %d\n", tuileRestante.tuile, tuileRestante.item);

}

void ResetRender(SDL_Renderer * renderer, Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void printButton(SDL_Renderer *renderer, SDL_Surface *image, SDL_Texture *texture_button, SDL_Rect rect_button, const char* file){

    image = SDL_LoadBMP(file);
    texture_button = SDL_CreateTextureFromSurface(renderer, image);

    SDL_FreeSurface(image);
    SDL_QueryTexture(texture_button, NULL, NULL, &rect_button.w, &rect_button.h);
    SDL_RenderCopy(renderer, texture_button, NULL, &rect_button);

}

void AfficheButton(SDL_Renderer *renderer,SDL_Surface *image, SDL_Texture *texture_button,SDL_Rect rect_button, const char* file, Color color, int px){

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_RenderFillRect(renderer,&rect_button);

    rect_button.x += px;
    rect_button.y += px;
    rect_button.h -= 2*px;
    rect_button.w -= 2*px;

    printImage(renderer, rect_button, file);
}

void AffichePlateau(SDL_Renderer *renderer){

    SDL_Rect rect_plateau = {(1920-522)/2, (1080-522)/2, 522, 522};
    SDL_Rect rect_plateau2 = {771, 297 , 18, 486};
    SDL_Rect rect_plateau3 = {717, 351 , 486, 18};

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

void printImage(SDL_Renderer *renderer, SDL_Rect rect_image, const char *chemin_image){

    SDL_Surface *surface_image = NULL;
    SDL_Texture *texture_image = NULL;

    surface_image = SDL_LoadBMP(chemin_image);
    texture_image = SDL_CreateTextureFromSurface(renderer, surface_image);
    SDL_FreeSurface(surface_image);
    SDL_QueryTexture(texture_image, NULL, NULL, &rect_image.w, &rect_image.h);
    SDL_RenderCopy(renderer, texture_image, NULL, &rect_image);

    SDL_DestroyTexture(texture_image);
}

void printPlayer(SDL_Renderer *renderer, SDL_Rect rect_player, int i, int j){

    SDL_Surface *image_player = NULL;
    SDL_Texture *texture_player = NULL;

    image_player = SDL_LoadBMP("images/items16px/player_1.bmp"); 
    texture_player = SDL_CreateTextureFromSurface(renderer, image_player);
    SDL_FreeSurface(image_player);
    SDL_QueryTexture(texture_player, NULL, NULL, &rect_player.w, &rect_player.h);
    SDL_RenderCopy(renderer, texture_player, NULL, &rect_player);

    SDL_DestroyTexture(texture_player);
}

void RandomPlateau(){

    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 7; j++){
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
        }
    }
    
    

    SearchTuile();

}

void AfficheTuileItem(SDL_Renderer *renderer){

    SDL_Rect rect_tuile = {717, 297, 54, 54};
    SDL_Rect rect_item = {736, 316, 16 , 16};

    for( int i=0 ; i<7 ; i++ ){
        rect_tuile.x = 717;
        rect_item.x = 736;
        for( int j=0 ; j<7 ; j++){
            if (playerData[0].posX == i && playerData[0].posY == j){
                printImage(renderer,rect_tuile, chemin_tuile[(SDLplateau[i][j].tuile)-1]);
                printImage(renderer,rect_item, "images/skin16px/player_1.bmp");
            } else if (playerData[1].posX == i && playerData[1].posY == j){
                printImage(renderer,rect_tuile, chemin_tuile[(SDLplateau[i][j].tuile)-1]);
                printImage(renderer,rect_item, "images/skin16px/player_2.bmp");
            } else if (playerData[2].posX == i && playerData[2].posY == j){
                printImage(renderer,rect_tuile, chemin_tuile[(SDLplateau[i][j].tuile)-1]);
                printImage(renderer,rect_item, "images/skin16px/player_3.bmp");
            } else if (playerData[3].posX == i && playerData[3].posY == j){
                printImage(renderer,rect_tuile, chemin_tuile[(SDLplateau[i][j].tuile)-1]);
                printImage(renderer,rect_item, "images/skin16px/player_4.bmp");
            } else {
                printImage(renderer,rect_tuile, chemin_tuile[(SDLplateau[i][j].tuile)-1]);
                printImage(renderer,rect_item, chemin_item[(SDLplateau[i][j].item)-1]);
            }
            rect_tuile.x += 4*18;
            rect_item.x += 4*18;
        }
        rect_tuile.y += 4*18;
        rect_item.y += 4*18;
    }
}

void AffichePlateauTuileItem(SDL_Renderer *renderer){

    SDL_Rect rect_tuileRestante = {(1920-54)/2, 100, 54, 54};
    SDL_Rect rect_itemRestant = {(1920-16)/2, 100-17, 16, 16};

    ResetRender(renderer, Background);
    printImage(renderer,rect_tuileRestante,chemin_tuile[tuileRestante.tuile]);
    if( tuileRestante.item ) printImage(renderer,rect_itemRestant,chemin_item[tuileRestante.item]);
    AffichePlateau(renderer);
    AfficheTuileItem(renderer);  
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

int checkDeplacement(int player, int direction){
    switch(direction){
        case 0: // le joueur va en haut
        if (playerData[player].posX != 0){ // si le joueur n'est pas tout en haut
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile -1;
            int tuileSuivante = SDLplateau[playerData[player].posX-1][playerData[player].posY].tuile -1;
            if (tuileActuelle == 1 || tuileActuelle == 2 || tuileActuelle == 4 || tuileActuelle == 5 || tuileActuelle == 7 || tuileActuelle == 8){ // si la tuile à un passage en haut
                if (tuileSuivante == 0 || tuileSuivante == 3 || tuileSuivante == 5 || tuileSuivante == 6 || tuileSuivante == 7 || tuileSuivante == 8){ // si la tuile suivante a un passage en bas
                    return 1;
                }else return 0;
            }else return 0;
        }
        break;
        case 1: // le joueur va à droite
        if (playerData[player].posY != 6){ // si le joueur n'est pas tout à droite
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile -1;
            int tuileSuivante = SDLplateau[playerData[player].posX][playerData[player].posY+1].tuile -1;
            if (tuileActuelle == 0 || tuileActuelle == 1 || tuileActuelle == 4 || tuileActuelle == 6 || tuileActuelle == 7 || tuileActuelle == 9){ // si la tuile à un passage en haut
                if (tuileSuivante == 2 || tuileSuivante == 3 || tuileSuivante == 4 || tuileSuivante == 5 || tuileSuivante == 6 || tuileSuivante == 9){ // si la tuile suivante a un passage en bas
                    return 1;
                }else return 0;
            }else return 0;
        }
        break;
        case 2:
        if (playerData[player].posX != 6){ // si le joueur n'est pas tout en bas
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile -1;
            int tuileSuivante = SDLplateau[playerData[player].posX+1][playerData[player].posY].tuile -1;
            if (tuileActuelle == 0 || tuileActuelle == 3 || tuileActuelle == 5  || tuileActuelle == 6 || tuileActuelle == 7 || tuileActuelle == 8){ // si la tuile à un passage en haut
                if (tuileSuivante == 1 || tuileSuivante == 2 || tuileSuivante == 4 || tuileSuivante == 5 || tuileSuivante == 7 || tuileSuivante == 8){ // si la tuile suivante a un passage en bas
                    return 1;
                }else return 0;
            }else return 0;
        }
        break;
        case 3: //le joueur va à gauche
        if (playerData[player].posY != 0){
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile -1;
            int tuileSuivante = SDLplateau[playerData[player].posX][playerData[player].posY-1].tuile -1;
            if ( tuileActuelle == 2 || tuileActuelle == 3 || tuileActuelle == 4 || tuileActuelle == 5 || tuileActuelle == 6 || tuileActuelle == 9){
                if(tuileSuivante ==  0 || tuileSuivante == 1 || tuileSuivante == 4 || tuileSuivante == 6 || tuileSuivante == 7 || tuileSuivante == 9){
                    return 1;
                }else return 0;
            }else return 0;
        }
        break;       
    }
}

int movePlayer(int player, int direction){
  if(checkDeplacement(player, direction) == 1){
    switch (direction) {
      case 0: playerData[player].posX -= 1; break;
      case 1: playerData[player].posY += 1; break;
      case 2: playerData[player].posX += 1; break;
      case 3: playerData[player].posY -= 1; break;
      case 4: break;
    }
    return 0;
  } else return 1;
}

int getRandomInt(int min, int max){
  int i = (rand() % (max-min+1)) + min;
  return i;
}