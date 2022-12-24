// gcc src/plateau_graphique.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "SDL_ttf.h"
#include <SDL_mixer.h>
#include "bmp_manip.h"

#define MENU_BUTTON_W 820 // taille des images des boutons du menu
#define MENU_BUTTON_H 90
#define MENU_BUTTON_BORDER 12

typedef struct{
    Uint8 r, g, b, a;
}Color;

typedef struct{ 
    uint8_t tuile, item; // uint8 suffit car on a seulement 10 types de tuiles différentes et 24 items
}Case;

typedef struct{
    uint8_t posX, posY, itemFound; //pareil pour le uint8 (la position va de 0 à 7 et le nombre d'item jusqu'a 6)
}PlayerDATA;

typedef struct{
    size_t itemSize, tuileSize, borderSize;
}InfoDisplay;

int getRandomInt(int min, int max);

void ResetRender(SDL_Renderer *renderer, Color color);
void AfficheButton(SDL_Renderer *renderer,SDL_Rect rect_button, const char* file, Color color, int px);
void fenetreMenu(SDL_Renderer *renderer);
void SearchTuile();
void AffichePlateau(SDL_Renderer *renderer);
void RandomPlateau();
void AfficheTuileItem( SDL_Renderer *renderer);
void AffichePlateauTuileItem(SDL_Renderer *renderer, SDL_Rect rect_tuileRestante, SDL_Rect rect_itemRestant, SDL_Rect magnet_lock[12]);
void printDebugGrid(SDL_Renderer *renderer);
int movePlayer(int player, int direction);
void printImage(SDL_Renderer *renderer, SDL_Rect rect_image, const char *chemin_image);
void afficherPlateau(SDL_Renderer *renderer);
void resetPlateau();
void melangerTab(int* tab, size_t tailleTab);
void setGUIsize(uint8_t size);
void removeTempImages(void);
void printMagnetLockRect(SDL_Renderer *renderer ,SDL_Rect magnet_lock[12]);

void clean(SDL_Window *w, SDL_Renderer *r, SDL_Texture *t){   
    if(t)
        SDL_DestroyTexture(t);
    if(r)
        SDL_DestroyRenderer(r);
    if(w)
        SDL_DestroyWindow(w);
    SDL_Quit();

    removeTempImages();
}

InfoDisplay infoDisplay;

Case SDLplateau[7][7] = { 1,0,   0,0,   7,1,   0,0,   7,2,   0,0,   4,0,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          8,3,   0,0,   8,4,   0,0,   7,5,   0,0,   6,6,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          8,7,   0,0,   5,8,   0,0,   6,9,   0,0,   6,10,
                          0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,
                          2,0,   0,0,   5,11,  0,0,   5,12,  0,0,   3,0};

Case tuileRestante = {0,0};

PlayerDATA playerData[4] = { 0,0,0,   0,6,0,   6,0,0,   6,6,0 };

char nbTuileRestant[4] = {6,6,10,12}; // 6 tuiles T avec trésor // 6 tuiles L avec trésor // 10 tuiles L vides // 12 tuiles I vides

char nbItemRestant[24] = {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1};

Color Background = {255, 233, 210, 255};
Color ButtonSelected = {216, 192, 168, 255};
Color ButtonNotSelected = {229, 204, 178, 255};

int colorButtonNotSelected[3] = {229, 204, 178};

int main(int argc, char **argv){

    SDL_Window *window = NULL;
    SDL_Renderer *jeu = NULL;

    if(SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Erreur init > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);
    }

    SDL_DisplayMode Screen;
    if(SDL_GetDesktopDisplayMode(0, &Screen)){
        SDL_Log("Erreur init > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);
    }

    // ajustement de la taille des images pour correspondre au mieux à la taille de l'écran
    if (Screen.w < 720 || Screen.h < 480) setGUIsize(0);
    else if(Screen.w < 1280 || Screen.h < 720) setGUIsize(1);
    else if (Screen.w < 1920 || Screen.h < 1080) setGUIsize(2);
    else if (Screen.w < 2560 || Screen.h < 1440) setGUIsize(3);
    else if (Screen.w < 3840 || Screen.h < 2160) setGUIsize(4);
    else  setGUIsize(5);

    window = SDL_CreateWindow("Labyrinthe-Z", 0, 0, Screen.w, Screen.h, SDL_WINDOW_FULLSCREEN);
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

    clean(window, jeu, NULL);

    return EXIT_SUCCESS;
}

void fenetreMenu(SDL_Renderer *renderer){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    SDL_Surface *image = NULL;
    SDL_Texture *texture_button = NULL;

    SDL_Rect rect_button_1 = {(Screen.w-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (Screen.h/2)-(3*MENU_BUTTON_BORDER)-MENU_BUTTON_H, MENU_BUTTON_W+MENU_BUTTON_BORDER*2, MENU_BUTTON_H+MENU_BUTTON_BORDER*2};
    SDL_Rect rect_button_2 = {(Screen.w-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (Screen.h/2)+MENU_BUTTON_BORDER, MENU_BUTTON_W+MENU_BUTTON_BORDER*2, MENU_BUTTON_H+MENU_BUTTON_BORDER*2};

    SDL_bool windowOpen = SDL_TRUE;

    int cursorX, cursorY;

    uint8_t cursorInButton = 0;

    ResetRender(renderer, Background);
    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
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
                default:
                    continue;
                }
            case SDL_MOUSEMOTION:
                cursorX = event.motion.x;
                cursorY = event.motion.y;
                if(cursorX >= rect_button_1.x && cursorX <= rect_button_1.x + rect_button_1.w && cursorY >= rect_button_1.y && cursorY <= rect_button_1.y + rect_button_1.h){
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 1;
                } else if (cursorX >= rect_button_2.x && cursorX <= rect_button_2.x + rect_button_2.w && cursorY >= rect_button_2.y && cursorY <= rect_button_2.y + rect_button_2.h){
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 2;
                } else{
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 0;
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(cursorInButton == 1){
                    resetPlateau();                   
                    RandomPlateau();
                    afficherPlateau(renderer);
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    SDL_RenderPresent(renderer);
                } else if(cursorInButton == 2){      
                    windowOpen = SDL_FALSE; // ferme la fenêtre
                }
                break;  
            default:
                continue;
            } 
        }   
    }
}

void afficherPlateau(SDL_Renderer *renderer){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);
    
    int cursorX, cursorY;
    SDL_bool windowOpen = SDL_TRUE;

    size_t sizePlateau = (7*infoDisplay.tuileSize + 8*infoDisplay.borderSize);
    size_t posItem = (infoDisplay.tuileSize-infoDisplay.itemSize)/2;

    double plateauX = (Screen.w-sizePlateau)/2;
    double plateauY = (Screen.h-sizePlateau)/2;

    SDL_Rect rect_tuileRestante = {plateauX+4*infoDisplay.borderSize+3*infoDisplay.tuileSize, plateauY-infoDisplay.tuileSize-infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize};
    SDL_Rect rect_itemRestant = {rect_tuileRestante.x+posItem, rect_tuileRestante.y+posItem, infoDisplay.itemSize, infoDisplay.itemSize};

    SDL_Rect magnet_lock[12] = {plateauX+2*infoDisplay.borderSize+infoDisplay.tuileSize, plateauY-infoDisplay.tuileSize-infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX+4*infoDisplay.borderSize+3*infoDisplay.tuileSize, plateauY-infoDisplay.tuileSize-infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX+6*infoDisplay.borderSize+5*infoDisplay.tuileSize, plateauY-infoDisplay.tuileSize-infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX+sizePlateau+infoDisplay.borderSize, plateauY+infoDisplay.tuileSize+2*infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX+sizePlateau+infoDisplay.borderSize, plateauY+3*infoDisplay.tuileSize+4*infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX+sizePlateau+infoDisplay.borderSize, plateauY+5*infoDisplay.tuileSize+6*infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX+6*infoDisplay.borderSize+5*infoDisplay.tuileSize, plateauY+sizePlateau+infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX+4*infoDisplay.borderSize+3*infoDisplay.tuileSize, plateauY+sizePlateau+infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX+2*infoDisplay.borderSize+infoDisplay.tuileSize, plateauY+sizePlateau+infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX-infoDisplay.tuileSize-infoDisplay.borderSize, plateauY+5*infoDisplay.tuileSize+6*infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX-infoDisplay.tuileSize-infoDisplay.borderSize, plateauY+3*infoDisplay.tuileSize+4*infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                plateauX-infoDisplay.tuileSize-infoDisplay.borderSize, plateauY+infoDisplay.tuileSize+2*infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize};

    int posTuileRestante = 1;

    AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock);

    while( windowOpen ){
        
        SDL_Event event;
        SDL_bool mouseState;

        while( SDL_PollEvent(&event) ){

            switch( event.type ){
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ){
                case SDLK_ESCAPE:
                    return;
                    break;
                case SDLK_UP:
                    movePlayer(0, 0);
                    AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock);
                    break;
                case SDLK_RIGHT:
                    movePlayer(0, 1);
                    AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock);
                    break;
                case SDLK_DOWN:
                    movePlayer(0, 2);
                    AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock);
                    break;
                case SDLK_LEFT:
                    movePlayer(0, 3);
                    AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock);
                    break;
                default:
                    continue;
                }

            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT) {

                    cursorX = event.motion.x;
                    cursorY = event.motion.y;

                    if( cursorX >= rect_tuileRestante.x &&
                        cursorX <= rect_tuileRestante.x + rect_tuileRestante.w &&
                        cursorY >= rect_tuileRestante.y &&
                        cursorY <= rect_tuileRestante.y + rect_tuileRestante.h)
                    {

                        do {
                            if(SDL_PollEvent(&event)){
                                cursorX = event.motion.x;
                                cursorY = event.motion.y;

                                rect_tuileRestante.x = cursorX-infoDisplay.tuileSize/2; rect_tuileRestante.y = cursorY-infoDisplay.tuileSize/2;
                                rect_itemRestant.x = rect_tuileRestante.x+posItem ; rect_itemRestant.y = rect_tuileRestante.y+posItem;
                                AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock);

                            }
                        }while(event.type != SDL_MOUSEBUTTONUP);

                        //on quitte le while donc le bouton est relaché

                        for(int i = 0; i < 12; i++){
                            if( cursorX >= (magnet_lock[i].x - infoDisplay.tuileSize/2) &&
                                cursorX <= (magnet_lock[i].x + magnet_lock[i].w + infoDisplay.tuileSize/2) &&
                                cursorY >= (magnet_lock[i].y - infoDisplay.tuileSize/2) &&
                                cursorY <= (magnet_lock[i].y + magnet_lock[i].h + infoDisplay.tuileSize/2))
                            {
                                rect_tuileRestante.x = magnet_lock[i].x; rect_tuileRestante.y = magnet_lock[i].y;
                                rect_itemRestant.x = magnet_lock[i].x+posItem; rect_itemRestant.y = magnet_lock[i].y+posItem;
                                AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock);
                                posTuileRestante = i;
                                break;
                            }
                            else if(i == 11){
                                rect_tuileRestante.x = magnet_lock[posTuileRestante].x; rect_tuileRestante.y = magnet_lock[posTuileRestante].y;
                                rect_itemRestant.x = magnet_lock[posTuileRestante].x+posItem; rect_itemRestant.y = magnet_lock[posTuileRestante].y+posItem;
                                AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock);
                            }
                        }
                    }                               
                }             
                break;
            default:
                continue;
            }
        }
    }
}

void DeplaceTuile(SDL_Renderer *renderer, SDL_Rect rect_tuileRestante, SDL_Rect rect_itemRestant){

    char cheminItem[33];
    char cheminTuile[35];
    sprintf(cheminItem, "images/formated/item/item%d.bmp", tuileRestante.item);
    sprintf(cheminTuile, "images/formated/tuiles/Tuile%d.bmp", tuileRestante.tuile);
    printImage(renderer,rect_tuileRestante, cheminTuile);
    if( tuileRestante.item != 0) printImage(renderer,rect_itemRestant,cheminItem);

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

void AfficheButton(SDL_Renderer *renderer, SDL_Rect rect_button, const char* file, Color color, int px){

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_RenderFillRect(renderer,&rect_button);

    rect_button.x += px;
    rect_button.y += px;
    rect_button.h -= 2*px;
    rect_button.w -= 2*px;

    printImage(renderer, rect_button, file);
}

void AffichePlateau(SDL_Renderer *renderer){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    size_t sizePlateau = (7*infoDisplay.tuileSize + 8*infoDisplay.borderSize);

    SDL_Rect rect_plateau = {(Screen.w-sizePlateau)/2, (Screen.h-sizePlateau)/2, sizePlateau, sizePlateau};
    SDL_Rect rect_plateau2 = {(Screen.w-sizePlateau)/2+infoDisplay.borderSize+infoDisplay.tuileSize, (Screen.h-sizePlateau)/2+infoDisplay.borderSize , infoDisplay.borderSize, sizePlateau-2*infoDisplay.borderSize};
    SDL_Rect rect_plateau3 = {(Screen.w-sizePlateau)/2+infoDisplay.borderSize, (Screen.h-sizePlateau)/2+infoDisplay.borderSize+infoDisplay.tuileSize , sizePlateau-2*infoDisplay.borderSize, infoDisplay.borderSize};

    SDL_SetRenderDrawColor(renderer, 229, 204, 178, 255);
    for( int i=0 ; i<infoDisplay.borderSize ; i++){
        SDL_RenderDrawRect(renderer,&rect_plateau);
        rect_plateau.h -= 2;
        rect_plateau.w -= 2;
        rect_plateau.x = (Screen.w-(rect_plateau.w))/2;
        rect_plateau.y = (Screen.h-(rect_plateau.h))/2;
    }

    for( int i=0 ; i<6 ; i++){
        SDL_RenderFillRect(renderer,&rect_plateau2);
        rect_plateau2.x += infoDisplay.tuileSize+infoDisplay.borderSize;
    }

    for( int i=0 ; i<6 ; i++){
        SDL_RenderFillRect(renderer,&rect_plateau3);
        rect_plateau3.y += infoDisplay.tuileSize+infoDisplay.borderSize;
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

void RandomPlateau(){

    srand(time(NULL));

    int arrayTypeTuile[34] = {0,0,0,0,0,0, 1,1,1,1,1,1, 2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,3,3,3,3};
    int arrayItem[12] = {13,14,15,16,17,18,19,20,21,22,23,24};

    melangerTab(arrayTypeTuile, 34);
    melangerTab(arrayItem, 12);

    int indiceRemplissageTuile = 0;
    int indiceRemplissageItem = 0;

    for(int i = 0; i < 7; i++){ 
    for(int j = 0; j < 7; j++){
        if(i%2 == 1 || j%2 == 1){
            if(arrayTypeTuile[indiceRemplissageTuile] < 2){
                SDLplateau[i][j].item = arrayItem[indiceRemplissageItem];
                indiceRemplissageItem++;
            }else SDLplateau[i][j].item = 0;
            switch(arrayTypeTuile[indiceRemplissageTuile]){
                case 0:
                    SDLplateau[i][j].tuile = getRandomInt(5,8); break;
                case 1:
                case 2:
                    SDLplateau[i][j].tuile = getRandomInt(1,4); break;
                case 3:
                    SDLplateau[i][j].tuile = getRandomInt(9,10); break;
            }
            indiceRemplissageTuile++;
        }
    }}
    if(arrayTypeTuile[33] < 2) tuileRestante.item = arrayItem[11];
    switch(arrayTypeTuile[33]){
        case 0:
            tuileRestante.tuile = getRandomInt(5,8); break;
        case 1:
        case 2:
            tuileRestante.tuile = getRandomInt(1,4); break;
        case 3:
            tuileRestante.tuile = getRandomInt(9,10); break;
    }
}

void AfficheTuileItem(SDL_Renderer *renderer){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    size_t sizePlateau = (7*infoDisplay.tuileSize + 8*infoDisplay.borderSize);
    size_t posItem = (infoDisplay.tuileSize-infoDisplay.itemSize)/2;

    SDL_Rect rect_tuile = {(Screen.w-sizePlateau)/2+infoDisplay.borderSize, (Screen.h-sizePlateau)/2+infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize};
    SDL_Rect rect_item = {(Screen.w-sizePlateau)/2+infoDisplay.borderSize+posItem, (Screen.h-sizePlateau)/2+infoDisplay.borderSize+posItem, infoDisplay.itemSize , infoDisplay.itemSize};

    for( int i=0 ; i<7 ; i++ ){
        rect_tuile.x = (Screen.w-sizePlateau)/2+infoDisplay.borderSize;
        rect_item.x = (Screen.w-sizePlateau)/2+infoDisplay.borderSize+posItem;
        char cheminSkinPlayer[35];
        char cheminItem[32];
        char cheminTuile[35];
        for( int j=0 ; j<7 ; j++){
            for(int k = 0; k < 5; k++){
                sprintf(cheminTuile, "images/formated/tuiles/Tuile%d.bmp", SDLplateau[i][j].tuile);
                if(k == 4){
                    sprintf(cheminItem, "images/formated/item/item%d.bmp", SDLplateau[i][j].item);
                    printImage(renderer,rect_tuile, cheminTuile);
                    printImage(renderer,rect_item, cheminItem);
                }
                else if(playerData[k].posX == i && playerData[k].posY == j){
                    sprintf(cheminSkinPlayer, "images/formated/skin/player_%d.bmp", k+1);
                    printImage(renderer,rect_tuile, cheminTuile);
                    printImage(renderer,rect_item, cheminSkinPlayer);
                    break;
                }
            }
            rect_tuile.x += infoDisplay.tuileSize+infoDisplay.borderSize;
            rect_item.x += infoDisplay.tuileSize+infoDisplay.borderSize;
        }
        rect_tuile.y += infoDisplay.tuileSize+infoDisplay.borderSize;
        rect_item.y += infoDisplay.tuileSize+infoDisplay.borderSize;
    }
}

void AffichePlateauTuileItem(SDL_Renderer *renderer, SDL_Rect rect_tuileRestante, SDL_Rect rect_itemRestant, SDL_Rect magnet_lock[12]){

    ResetRender(renderer, Background);
    //printDebugGrid(renderer);
    printMagnetLockRect(renderer, magnet_lock);
    AffichePlateau(renderer);
    AfficheTuileItem(renderer);
    DeplaceTuile(renderer, rect_tuileRestante, rect_itemRestant); 
    SDL_RenderPresent(renderer);  

}

void printMagnetLockRect(SDL_Renderer *renderer, SDL_Rect magnet_lock[12]){
    SDL_SetRenderDrawColor(renderer,0,0,0, SDL_ALPHA_OPAQUE);
    for(int i = 0; i < 12; i++){
        SDL_RenderDrawRect(renderer, &magnet_lock[i]);
    }
}

void printDebugGrid(SDL_Renderer *renderer){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    SDL_SetRenderDrawColor(renderer,0,0,0, SDL_ALPHA_OPAQUE);
    for(int i = 0; i<=Screen.h; i+=60){
        SDL_RenderDrawLine(renderer,0,i,Screen.w,i);
    }
    SDL_SetRenderDrawColor(renderer,255,0,0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer,0,Screen.h/2,Screen.w,Screen.h/2);

    SDL_SetRenderDrawColor(renderer,0,0,0, SDL_ALPHA_OPAQUE);
    for(int i = 0; i<=Screen.w; i+=60){
        SDL_RenderDrawLine(renderer,i,0,i,Screen.h);
    }
    SDL_SetRenderDrawColor(renderer,255,0,0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer,Screen.w/2,0,Screen.w/2,Screen.h);
}

int checkDeplacement(int player, int direction){
    switch(direction){
        case 0: // le joueur va en haut
        if (playerData[player].posX > 0){ // si le joueur n'est pas tout en haut
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile-1;
            int tuileSuivante = SDLplateau[playerData[player].posX-1][playerData[player].posY].tuile-1;
            if (tuileActuelle == 1 || tuileActuelle == 2 || tuileActuelle == 4 || tuileActuelle == 5 || tuileActuelle == 7 || tuileActuelle == 8){ // si la tuile à un passage en haut
                if (tuileSuivante == 0 || tuileSuivante == 3 || tuileSuivante == 5 || tuileSuivante == 6 || tuileSuivante == 7 || tuileSuivante == 8){ // si la tuile suivante a un passage en bas
                    return 1;
                }else return 0;
            }else return 0;
        }
        break;
        case 1: // le joueur va à droite
        if (playerData[player].posY < 6){ // si le joueur n'est pas tout à droite
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile-1;
            int tuileSuivante = SDLplateau[playerData[player].posX][playerData[player].posY+1].tuile-1;
            if (tuileActuelle == 0 || tuileActuelle == 1 || tuileActuelle == 4 || tuileActuelle == 6 || tuileActuelle == 7 || tuileActuelle == 9){ // si la tuile à un passage en haut
                if (tuileSuivante == 2 || tuileSuivante == 3 || tuileSuivante == 4 || tuileSuivante == 5 || tuileSuivante == 6 || tuileSuivante == 9){ // si la tuile suivante a un passage en bas
                    return 1;
                }else return 0;
            }else return 0;
        }
        break;
        case 2:
        if (playerData[player].posX < 6){ // si le joueur n'est pas tout en bas
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile-1;
            int tuileSuivante = SDLplateau[playerData[player].posX+1][playerData[player].posY].tuile-1;
            if (tuileActuelle == 0 || tuileActuelle == 3 || tuileActuelle == 5  || tuileActuelle == 6 || tuileActuelle == 7 || tuileActuelle == 8){ // si la tuile à un passage en haut
                if (tuileSuivante == 1 || tuileSuivante == 2 || tuileSuivante == 4 || tuileSuivante == 5 || tuileSuivante == 7 || tuileSuivante == 8){ // si la tuile suivante a un passage en bas
                    return 1;
                }else return 0;
            }else return 0;
        }
        break;
        case 3: //le joueur va à gauche
        if (playerData[player].posY > 0){
            int tuileActuelle = SDLplateau[playerData[player].posX][playerData[player].posY].tuile-1;
            int tuileSuivante = SDLplateau[playerData[player].posX][playerData[player].posY-1].tuile-1;
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

void resetPlateau(){

    PlayerDATA tempPlayerData[4] = { 0,0,0,   0,6,0,   6,0,0,   6,6,0 };

    char tempArray[24] = {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1};
    char tempArray2[4] = {6,6,10,12};

    for(int i = 1; i<6; i+=2){
        for(int j = 1; j<6; j+=2){
            SDLplateau[i][j].tuile = 0;
            SDLplateau[i][j].item = 0;
        }
        tuileRestante.tuile = 0;
        tuileRestante.item = 0;
    }
    //SDL_memcpy(&rect_tuileRestante, &tempRectTuileRestante, sizeof(SDL_Rect));
    //SDL_memcpy(&rect_itemRestant, &tempRectItemRestant, sizeof(SDL_Rect));
    SDL_memcpy(&playerData, &tempPlayerData, sizeof(PlayerDATA)*4);
    SDL_memcpy(&nbItemRestant, &tempArray, sizeof(char)*24);
    SDL_memcpy(&nbTuileRestant, &tempArray2, sizeof(char)*4);
}

void melangerTab(int* tab, size_t tailleTab){
    int j; //position avec laquelle notre valeur va être échangée
    uint8_t buffer;
    for(int i = 0; i < tailleTab; i++){ //on parcours le tableau pour échanger la valeur d'une case avec une autre
        j = getRandomInt(0,tailleTab-1); //on génère l'indice de la case avec laquelle notre valeur est échangée
        buffer = tab[j]; //on mémorise la valeur pour ne pas la perdre
        tab[j] = tab[i]; //on met i dans j
        tab[i] = buffer; //on met j dans i
    }
}

void setGUIsize(uint8_t size){
    float facteurResize;
    switch(size){
        case 0:
            facteurResize = 0.5f;
            break;
        case 1:
            facteurResize = 0.75f;
            break;
        case 2:
            facteurResize = 1.0f;
            break;
        case 3:
            facteurResize = 1.5f;
            break;
        case 4:
            facteurResize = 2.0f;
            break;
        case 5:
            facteurResize = 3.0f;
            break;                       
    }

    infoDisplay.itemSize = 16*facteurResize;
    infoDisplay.tuileSize = 54*facteurResize;
    infoDisplay.borderSize = 18*facteurResize;

    char cheminImage[100];
    char cheminImageRedim[100];

    // redimentionnement des items
    for(int i = 1; i <= 24; i++){
        sprintf(cheminImage, "images/default/item/item%d.bmp", i);

        SDL_Surface *image = SDL_LoadBMP(cheminImage);
        if (image == NULL) {
            printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }

        SDL_Surface *image_redim = redimImage(image, facteurResize);
        if (image_redim == NULL) {
            printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }

        sprintf(cheminImageRedim, "images/formated/item/item%d.bmp", i);

        // Enregistrement de l'image redimensionnée
        SDL_SaveBMP(image_redim, cheminImageRedim);
    }

    // redimentionnement des joueurs 
    for(int i = 1; i <= 4; i++){
        sprintf(cheminImage, "images/default/skin/player_%d.bmp", i);

        SDL_Surface *image = SDL_LoadBMP(cheminImage);
        if (image == NULL) {
            printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }

        SDL_Surface *image_redim = redimImage(image, facteurResize);
        if (image_redim == NULL) {
            printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }

        sprintf(cheminImageRedim, "images/formated/skin/player_%d.bmp", i);

        // Enregistrement de l'image redimensionnée
        SDL_SaveBMP(image_redim, cheminImageRedim);
    }

    // redimentionnement des tuiles 
    for(int i = 1; i <= 10; i++){
        sprintf(cheminImage, "images/default/tuiles/Tuile%d.bmp", i);

        SDL_Surface *image = SDL_LoadBMP(cheminImage);
        if (image == NULL) {
            printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }

        SDL_Surface *image_redim = redimImage(image, facteurResize);
        if (image_redim == NULL) {
            printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }

        sprintf(cheminImageRedim, "images/formated/tuiles/Tuile%d.bmp", i);

        // Enregistrement de l'image redimensionnée
        SDL_SaveBMP(image_redim, cheminImageRedim);
    }

    // redimentionnement des boutons 
    for(int i = 1; i <= 2; i++){
        sprintf(cheminImage, "images/default/button/button%d.bmp", i);

        SDL_Surface *image = SDL_LoadBMP(cheminImage);
        if (image == NULL) {
            printf("Erreur lors du chargement de l'image : %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }

        sprintf(cheminImageRedim, "images/formated/button/button%d.bmp", i);

        // Enregistrement de l'image redimensionnée
        SDL_SaveBMP(image, cheminImageRedim);
    }
}

void removeTempImages(void){
    char cheminImage[100];
    for(int i = 1; i <= 24; i++){
        sprintf(cheminImage, "images/formated/item/item%d.bmp", i);
        remove(cheminImage);
    }
    for(int i = 1; i <= 4; i++){
        sprintf(cheminImage, "images/formated/skin/player_%d.bmp", i);
        remove(cheminImage);
    }
    for(int i = 1; i <= 10; i++){
        sprintf(cheminImage, "images/formated/tuiles/Tuile%d.bmp", i);
        remove(cheminImage);
    }
    for(int i = 1; i <= 2; i++){
        sprintf(cheminImage, "images/formated/button/button%d.bmp", i);
        remove(cheminImage);
    }
}