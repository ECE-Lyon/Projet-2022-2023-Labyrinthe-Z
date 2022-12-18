// gcc src/plateau_graphique.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "SDL_ttf.h"
#include <SDL_mixer.h>

#define MENU_BUTTON_W 820 // taille des images des boutons du menu
#define MENU_BUTTON_H 90
#define MENU_BUTTON_BORDER 12

#define SCREEN_W 1920
#define SCREEN_H 1080

typedef struct{
    Uint8 r, g, b, a;
}Color;

int getRandomInt(int min, int max);

void ResetRender(SDL_Renderer *renderer, Color color);
void AfficheButton(SDL_Renderer *renderer,SDL_Rect rect_button, const char* file, Color color, int px);
void fenetreMenu(SDL_Renderer *renderer);
void SearchTuile();
void AffichePlateau(SDL_Renderer *renderer);
void RandomPlateau();
void AfficheTuileItem( SDL_Renderer *renderer);
void AffichePlateauTuileItem(SDL_Renderer *renderer);
void printDebugGrid(SDL_Renderer *renderer);
int movePlayer(int player, int direction);
void printImage(SDL_Renderer *renderer, SDL_Rect rect_image, const char *chemin_image);
void afficherPlateau(SDL_Renderer *renderer);
void resetPlateau();
void melangerTab(int* tab, size_t tailleTab);

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
    uint8_t tuile, item; // uint8 suffit car on a seulement 10 types de tuiles différentes et 24 items
}Case;

typedef struct{
    uint8_t posX, posY, itemFound; //pareil pour le uint8 (la position va de 0 à 7 et le nombre d'item jusqu'a 6)
}PlayerDATA;

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

SDL_Rect rect_tuileRestante = {(SCREEN_W-54)/2, 100, 54, 54};
SDL_Rect rect_itemRestant = {(SCREEN_W-16)/2, 100+19, 16, 16};

int main(int argc, char **argv){

    SDL_Window *window = NULL;
    SDL_Renderer *jeu = NULL;

    if(SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Erreur init > %s\n",SDL_GetError());
        clean(NULL,NULL,NULL);
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Labyrinthe-Z", 0, 0, SCREEN_W, SCREEN_H, SDL_WINDOW_FULLSCREEN);
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
    
    SDL_DestroyRenderer(jeu);

    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}

void fenetreMenu(SDL_Renderer *renderer){

    SDL_Surface *image = NULL;
    SDL_Texture *texture_button = NULL;

    SDL_Rect rect_button_1 = {(SCREEN_W-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (SCREEN_H/2)-(3*MENU_BUTTON_BORDER)-MENU_BUTTON_H, MENU_BUTTON_W+MENU_BUTTON_BORDER*2, MENU_BUTTON_H+MENU_BUTTON_BORDER*2};
    SDL_Rect rect_button_2 = {(SCREEN_W-MENU_BUTTON_W)/2-MENU_BUTTON_BORDER, (SCREEN_H/2)+MENU_BUTTON_BORDER, MENU_BUTTON_W+MENU_BUTTON_BORDER*2, MENU_BUTTON_H+MENU_BUTTON_BORDER*2};

    SDL_bool windowOpen = SDL_TRUE;

    int cursorX, cursorY;

    uint8_t cursorInButton = 0;

    ResetRender(renderer, Background);
    AfficheButton(renderer, rect_button_1, "images/button/newgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
    AfficheButton(renderer, rect_button_2, "images/button/exitgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
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
                    AfficheButton(renderer, rect_button_1, "images/button/newgame.bmp", ButtonSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/button/exitgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 1;
                } else if (cursorX >= rect_button_2.x && cursorX <= rect_button_2.x + rect_button_2.w && cursorY >= rect_button_2.y && cursorY <= rect_button_2.y + rect_button_2.h){
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/button/newgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/button/exitgame.bmp", ButtonSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 2;
                } else{
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/button/newgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/button/exitgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
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
                    AfficheButton(renderer, rect_button_1, "images/button/newgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/button/exitgame.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    SDL_RenderPresent(renderer);
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
    
    int cursorX, cursorY;
    SDL_bool windowOpen = SDL_TRUE;

    AffichePlateauTuileItem(renderer);

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

            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT) {

                    cursorX = event.motion.x;
                    cursorY = event.motion.y;

                    if( cursorX >= rect_tuileRestante.x &&
                        cursorX <= rect_tuileRestante.x + rect_tuileRestante.w &&
                        cursorY >= rect_tuileRestante.y &&
                        cursorY <= rect_tuileRestante.y + rect_tuileRestante.h)
                    {

                        SDL_PollEvent(&event);

                        while(event.type != SDL_MOUSEBUTTONUP){

                            if(SDL_PollEvent(&event)){
                                cursorX = event.motion.x;
                                cursorY = event.motion.y;

                                rect_tuileRestante.x = cursorX-27; rect_tuileRestante.y = cursorY-27;
                                rect_itemRestant.x = cursorX+19-27 ; rect_itemRestant.y = cursorY+19-27 ;
                                AffichePlateauTuileItem(renderer);
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

void DeplaceTuile(SDL_Renderer *renderer){

    char cheminItem[28];
    char cheminTuile[26];
    sprintf(cheminItem, "images/items16px/item%d.bmp", tuileRestante.item-1);
    sprintf(cheminTuile, "images/tuiles/Tuile%d.bmp", tuileRestante.tuile);
    printImage(renderer,rect_tuileRestante, cheminTuile);
    if( tuileRestante.item ) printImage(renderer,rect_itemRestant,cheminItem);

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

    SDL_Rect rect_plateau = {(SCREEN_W-522)/2, (SCREEN_H-522)/2, 522, 522};
    SDL_Rect rect_plateau2 = {771, 297 , 18, 486};
    SDL_Rect rect_plateau3 = {717, 351 , 486, 18};

    SDL_SetRenderDrawColor(renderer, 229, 204, 178, 255);
    for( int i=0 ; i<18 ; i++){
        SDL_RenderDrawRect(renderer,&rect_plateau);
        rect_plateau.h -= 2;
        rect_plateau.w -= 2;
        rect_plateau.x = (SCREEN_W-(rect_plateau.w))/2;
        rect_plateau.y = (SCREEN_H-(rect_plateau.h))/2;
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

    SDL_Rect rect_tuile = {717, 297, 54, 54};
    SDL_Rect rect_item = {736, 316, 16 , 16};

    for( int i=0 ; i<7 ; i++ ){
        rect_tuile.x = 717;
        rect_item.x = 736;
        char cheminSkinPlayer[30];
        char cheminItem[28];
        char cheminTuile[26];
        for( int j=0 ; j<7 ; j++){
            for(int k = 0; k < 5; k++){
                sprintf(cheminTuile, "images/tuiles/Tuile%d.bmp", SDLplateau[i][j].tuile);
                if(k == 4){
                    sprintf(cheminItem, "images/items16px/item%d.bmp", SDLplateau[i][j].item-1);
                    printImage(renderer,rect_tuile, cheminTuile);
                    printImage(renderer,rect_item, cheminItem);
                }
                else if(playerData[k].posX == i && playerData[k].posY == j){
                    sprintf(cheminSkinPlayer, "images/skin16px/player_%d.bmp", k+1);
                    printImage(renderer,rect_tuile, cheminTuile);
                    printImage(renderer,rect_item, cheminSkinPlayer);
                    break;
                }
            }
            rect_tuile.x += 4*18;
            rect_item.x += 4*18;
        }
        rect_tuile.y += 4*18;
        rect_item.y += 4*18;
    }
}

void AffichePlateauTuileItem(SDL_Renderer *renderer){

    ResetRender(renderer, Background);
    AffichePlateau(renderer);
    AfficheTuileItem(renderer);
    DeplaceTuile(renderer); 
    SDL_RenderPresent(renderer);  

}

void printDebugGrid(SDL_Renderer *renderer){
    SDL_SetRenderDrawColor(renderer,0,0,0, SDL_ALPHA_OPAQUE);
    for(int i = 0; i<=SCREEN_H; i+=60){
        SDL_RenderDrawLine(renderer,0,i,SCREEN_W,i);
    }
    SDL_SetRenderDrawColor(renderer,255,0,0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer,0,540,SCREEN_W,540);

    SDL_SetRenderDrawColor(renderer,0,0,0, SDL_ALPHA_OPAQUE);
    for(int i = 0; i<=SCREEN_W; i+=60){
        SDL_RenderDrawLine(renderer,i,0,i,SCREEN_H);
    }
    SDL_SetRenderDrawColor(renderer,255,0,0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer,960,0,960,SCREEN_H);
    
    SDL_RenderPresent(renderer);
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

    SDL_Rect tempRectTuileRestante = {(SCREEN_W-54)/2, 100, 54, 54};
    SDL_Rect tempRectItemRestant = {(SCREEN_W-16)/2, 100+19, 16, 16};

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
    SDL_memcpy(&rect_tuileRestante, &tempRectTuileRestante, sizeof(SDL_Rect));
    SDL_memcpy(&rect_itemRestant, &tempRectItemRestant, sizeof(SDL_Rect));
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