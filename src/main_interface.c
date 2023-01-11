// gcc src/main_interface.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <SDL.h>
#include "SDL_ttf.h"
#include <SDL_mixer.h>
#include "bmp_manip.h"

#define MENU_BUTTON_BORDER 4

typedef struct{
    Uint8 r, g, b, a;
}Color;

typedef struct{ 
    uint8_t tuile, item; // uint8 suffit car on a seulement 10 types de tuiles différentes et 24 items
}Case;

typedef struct{
    uint8_t posX, posY;
    int itemFound; //pareil pour le uint8 (la position va de 0 à 7 et le nombre d'item jusqu'a 6)
}PlayerDATA;

typedef struct{
    int tab[12];
}PlayerCARD;

typedef struct{
    size_t itemSize, tuileSize, borderSize, cadreSizeX, cadreSizeY, skinSizeX, skinSizeY, menuButtonX, menuButtonY, cadreTR, text_playerX, text_playerY, tickProgression;
}InfoDisplay;

typedef struct{
    SDL_Texture *BG[4], *cadre, *skin[4], *tuile[12], *item[24], *player[4], *mouse, *cadreTuileRestante, *tick[20], *item32[24], *text_player[4];
}TextureJeu;

typedef struct{
    SDL_Texture *button[2], *buttonPressed[2], *mouse;
}TextureMenu;

int getRandomInt(int min, int max);

void RandomCard(int nbCards);
void ResetRender(SDL_Renderer *renderer, Color color);
void AfficheButton(SDL_Renderer *renderer,SDL_Rect rect_button, const char* file, Color color, int px);
void fenetreMenu(SDL_Renderer *renderer, TextureMenu *menuTexture);
void SearchTuile();
void AffichePlateau(SDL_Renderer *renderer);
void RandomPlateau();
void AfficheTuileItem(SDL_Renderer *renderer, TextureJeu *gameTexture, struct timeval time);
void AffichePlateauTuileItem(SDL_Renderer *renderer, SDL_Rect rect_tuileRestante, SDL_Rect rect_itemRestant, SDL_Rect magnet_lock[13], TextureJeu *gameTexture, int cursorX, int cursorY, SDL_Rect rect_TR, SDL_Rect rect_button[4], SDL_Rect rect_Tick[2], int playerTurn);
void printDebugGrid(SDL_Renderer *renderer);
int movePlayer(int player, int direction);
void printImage(SDL_Renderer *renderer, SDL_Rect rect_image, const char *chemin_image);
void afficherPlateau(SDL_Renderer *renderer, TextureJeu *gameTexture, int* cursorX, int* cursorY);
void resetPlateau();
void melangerTab(int* tab, size_t tailleTab);
float setGUIsize(uint8_t size);
void removeTempImages(void);
void printMagnetLockRect(SDL_Renderer *renderer, SDL_Rect magnet_lock[12], TextureJeu *gameTexture);
int pushTuile(int emplacement);
void afficherHUD(SDL_Renderer *renderer, TextureJeu *gameTexture, int cursorX, int cursorY, SDL_Rect rect_button[2], SDL_Rect rect_TR, SDL_Rect rect_Tick[2], int playerTurn);
void printBG(SDL_Renderer *renderer, TextureJeu *gameTexture);
void printImageFromSurface(SDL_Renderer *renderer, SDL_Surface *surface_image,SDL_Rect rect_image);
void unloadTexturesPlateau(SDL_Renderer *renderer ,TextureJeu *gameTexture);
TextureJeu loadGameTexture(SDL_Renderer *renderer);
TextureMenu loadMenuTexture(SDL_Renderer *renderer);
void delay(time_t pauseTime);
void rotateTuile(uint8_t* tuileRestante, int direction);
void createRectTick(SDL_Rect rect_Tick[2], int emplacement, SDL_Rect magnet_lock[13]);
void checkObjectif(int x, int y, int player);

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

PlayerDATA playerData[4] = { 0,0,1,   0,6,1,   6,0,1,   6,6,1 };

PlayerCARD playerCard[4];

int nbplayer = 4;

char nbTuileRestant[4] = {6,6,10,12}; // 6 tuiles T avec trésor // 6 tuiles L avec trésor // 10 tuiles L vides // 12 tuiles I vides

char nbItemRestant[24] = {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1};

int nbCardRestant[24] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

char itemTrouves[24] = {0};

Color Background = {255, 233, 210, 255};
Color ButtonSelected = {216, 192, 168, 255};
Color ButtonNotSelected = {229, 204, 178, 255};

int colorButtonNotSelected[3] = {229, 204, 178};

long current_frame = 0;

float facteurResize;

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
    if (Screen.w < 720 || Screen.h < 480) facteurResize = setGUIsize(0);
    else if(Screen.w < 1280 || Screen.h < 720) facteurResize = setGUIsize(1);
    else if (Screen.w < 1920 || Screen.h < 1080) facteurResize = setGUIsize(2);
    else if (Screen.w < 2560 || Screen.h < 1440) facteurResize = setGUIsize(3);
    else if (Screen.w < 3840 || Screen.h < 2160) facteurResize = setGUIsize(4);
    else facteurResize = setGUIsize(5);

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

    if (Mix_OpenAudio(96000, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0){
        SDL_Log("Erreur initialisation SDL_mixer : %s", Mix_GetError());
        SDL_Quit();
        return -1;
    }

    TextureMenu menuTexture = loadMenuTexture(jeu);
    fenetreMenu(jeu, &menuTexture);

    /*for ( int i=0 ; i<nbplayer ; i++ )
    {
        printf("\n");
        for (int j = 0; j < 12; j++)
        {
            printf("[%d]",playerCard[i].tab[j]);
        }
        
    }printf("\n");*/

    clean(window, jeu, NULL);

    return EXIT_SUCCESS;
}

TextureMenu loadMenuTexture(SDL_Renderer *renderer){
    TextureMenu menuTexture;

    SDL_Surface *surface = SDL_LoadBMP("images/default/HUD/mouse.bmp");
    handleSurfaceError(surface);
    menuTexture.mouse = SDL_CreateTextureFromSurface(renderer, surface);
    handleTextureError(menuTexture.mouse);
    SDL_FreeSurface(surface);

    return menuTexture;
}

void fenetreMenu(SDL_Renderer *renderer, TextureMenu *menuTexture){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    SDL_Surface *image = NULL;
    SDL_Texture *texture_button = NULL;

    SDL_Rect rect_button_1 = {(Screen.w-infoDisplay.menuButtonX)/2-MENU_BUTTON_BORDER, (Screen.h/2)-(3*MENU_BUTTON_BORDER)-infoDisplay.menuButtonY, infoDisplay.menuButtonX+MENU_BUTTON_BORDER*2, infoDisplay.menuButtonY+MENU_BUTTON_BORDER*2};
    SDL_Rect rect_button_2 = {(Screen.w-infoDisplay.menuButtonX)/2-MENU_BUTTON_BORDER, (Screen.h/2)+MENU_BUTTON_BORDER, infoDisplay.menuButtonX+MENU_BUTTON_BORDER*2, infoDisplay.menuButtonY+MENU_BUTTON_BORDER*2};

    SDL_bool windowOpen = SDL_TRUE;

    int cursorX, cursorY;

    uint8_t cursorInButton = 0;

    Uint8 volume = 50;

    Mix_Music *button = Mix_LoadMUS("Sound/Button.mp3");
    Mix_VolumeMusic(volume);

    SDL_ShowCursor(SDL_DISABLE);

    ResetRender(renderer, Background);
    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);

    while( windowOpen ){
        
        SDL_Event event;

        while( SDL_PollEvent(&event) ){

            cursorX = event.motion.x;
            cursorY = event.motion.y;

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
                if(cursorX >= rect_button_1.x && cursorX <= rect_button_1.x + rect_button_1.w && cursorY >= rect_button_1.y && cursorY <= rect_button_1.y + rect_button_1.h){
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/formated/button/button1_pressed.bmp", ButtonSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 1;
                } else if (cursorX >= rect_button_2.x && cursorX <= rect_button_2.x + rect_button_2.w && cursorY >= rect_button_2.y && cursorY <= rect_button_2.y + rect_button_2.h){
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/formated/button/button2_pressed.bmp", ButtonSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 2;
                } else{
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    cursorInButton = 0;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(cursorInButton == 1){
                    Mix_PlayMusic(button, 0);
                    resetPlateau();                 
                    RandomPlateau();
                    RandomCard(nbplayer);
                    TextureJeu gameTexture = loadGameTexture(renderer);
                    afficherPlateau(renderer, &gameTexture, &cursorX, &cursorY);
                      

                    unloadTexturesPlateau(renderer, &gameTexture);
                    ResetRender(renderer, Background);
                    AfficheButton(renderer, rect_button_1, "images/formated/button/button1.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    AfficheButton(renderer, rect_button_2, "images/formated/button/button2.bmp", ButtonNotSelected, MENU_BUTTON_BORDER);
                    //SDL_RenderPresent(renderer);
                } else if(cursorInButton == 2){      
                    windowOpen = SDL_FALSE; // ferme la fenêtre
                }
                break;
            case SDL_QUIT:
                windowOpen = SDL_FALSE; // ferme la fenêtre
                break;
            default:
                continue;
            }
            SDL_Rect rect_mouse = {cursorX, cursorY, 32, 32};
            printImageFromTexture(renderer, menuTexture->mouse, rect_mouse);
            SDL_RenderPresent(renderer);
        }   
    }
}


TextureJeu loadGameTexture(SDL_Renderer *renderer){
    TextureJeu gameTexture;

    // BACKGROUND

    for(int i = 0; i < 4; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/HUD/BG/2/%d.bmp", i+1);
        SDL_Surface *BG_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(BG_surface);
        gameTexture.BG[i] = SDL_CreateTextureFromSurface(renderer, BG_surface);
        handleTextureError(gameTexture.BG[0]);
        SDL_FreeSurface(BG_surface);
    }

    // CADRES DU HUD

    SDL_Surface *cadre_surface = SDL_LoadBMP("images/default/HUD/cadre2.bmp");
    handleSurfaceError(cadre_surface);
    gameTexture.cadre = SDL_CreateTextureFromSurface(renderer, cadre_surface);
    handleTextureError(gameTexture.cadre);
    SDL_FreeSurface(cadre_surface);

    // SKIN DANS LE HUD

    for(int i = 0; i < 4; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/skin/skin_%d.bmp", i+1);
        SDL_Surface *skin_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(skin_surface);
        gameTexture.skin[i] = SDL_CreateTextureFromSurface(renderer, skin_surface);
        handleTextureError(gameTexture.skin[i]);
        SDL_FreeSurface(skin_surface);
    }

    // TUILES

    for(int i = 0; i < 10; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/tuiles/Tuile%d.bmp", i+1);
        SDL_Surface *tuile_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(tuile_surface);
        gameTexture.tuile[i] = SDL_CreateTextureFromSurface(renderer, tuile_surface);
        handleTextureError(gameTexture.tuile[i]);
        SDL_FreeSurface(tuile_surface);
    }

    // ITEMS

    for(int i = 0; i < 24; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/item/item16px/item%d.bmp", i+1);
        SDL_Surface *item_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(item_surface);
        gameTexture.item[i] = SDL_CreateTextureFromSurface(renderer, item_surface);
        handleTextureError(gameTexture.item[i]);
        SDL_FreeSurface(item_surface);
    }

    // ITEMS 32

    for(int i = 0; i < 24; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/item/item32px/item%d.bmp", i+1);
        SDL_Surface *item32_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(item32_surface);
        gameTexture.item32[i] = SDL_CreateTextureFromSurface(renderer, item32_surface);
        handleTextureError(gameTexture.item32[i]);
        SDL_FreeSurface(item32_surface);
    }

    // PLAYERS

    for(int i = 0; i < 4; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/skin/player_%d.bmp", i+1);
        SDL_Surface *item_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(item_surface);
        gameTexture.player[i] = SDL_CreateTextureFromSurface(renderer, item_surface);
        handleTextureError(gameTexture.player[i]);
        SDL_FreeSurface(item_surface);
    }

    // CURSEUR

    SDL_Surface *mouse_surface = SDL_LoadBMP("images/default/HUD/mouse.bmp");
    handleSurfaceError(mouse_surface);
    gameTexture.mouse = SDL_CreateTextureFromSurface(renderer, mouse_surface);
    handleTextureError(gameTexture.mouse);
    SDL_FreeSurface(mouse_surface);

    //CADRE TUILE RESTANTE

    SDL_Surface *TR_surface = SDL_LoadBMP("images/default/HUD/cadreTuileRestante.bmp");
    handleSurfaceError(TR_surface);
    gameTexture.cadreTuileRestante = SDL_CreateTextureFromSurface(renderer, TR_surface);
    handleTextureError(gameTexture.cadreTuileRestante);
    SDL_FreeSurface(TR_surface);

    // TICK YES ET NO, BOUTONS ROTATION, CADRES JOUEUR ACTUEL, TICK DE PROGRESSION

    for(int i = 0; i < 20; i++){
        char cheminTick[100];
        sprintf(cheminTick, "images/default/HUD/tick%d.bmp", i+1);
        SDL_Surface *tick_surface = SDL_LoadBMP(cheminTick);
        handleSurfaceError(tick_surface);
        gameTexture.tick[i] = SDL_CreateTextureFromSurface(renderer, tick_surface);
        handleTextureError(gameTexture.tick[i]);
        SDL_FreeSurface(tick_surface);
    }

    // TEXTE PLAYER

    for(int i = 0; i < 4; i++){
        char cheminTextPlayer[100];
        sprintf(cheminTextPlayer, "images/default/HUD/textP%d.bmp", i+1);
        SDL_Surface *textPlayer_surface = SDL_LoadBMP(cheminTextPlayer);
        handleSurfaceError(textPlayer_surface);
        gameTexture.text_player[i] = SDL_CreateTextureFromSurface(renderer, textPlayer_surface);
        handleTextureError(gameTexture.text_player[i]);
        SDL_FreeSurface(textPlayer_surface);
    }

    return gameTexture;
}

void unloadTexturesPlateau(SDL_Renderer *renderer ,TextureJeu *gameTexture){

    for(int i = 0; i < sizeof(TextureJeu); i += sizeof(SDL_Texture *)){ // parcourt la struct de textures

        SDL_Texture* pSurTexture = (SDL_Texture *) ((int)gameTexture + i); // calcule la valeur du pointeur de la texture actuelle
        SDL_DestroyTexture(pSurTexture);

    }
}

void afficherPlateau(SDL_Renderer *renderer, TextureJeu *gameTexture, int* cursorX_, int* cursorY_){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    SDL_ShowCursor(SDL_DISABLE);
    
    int cursorX = *cursorX_, cursorY = *cursorY_;
    SDL_bool windowOpen = SDL_TRUE;

    size_t sizePlateau = (7*infoDisplay.tuileSize + 8*infoDisplay.borderSize);
    size_t posItem = (infoDisplay.tuileSize-infoDisplay.itemSize)/2;
    size_t spaceForHUD = (Screen.w-sizePlateau)/2-infoDisplay.borderSize/2-infoDisplay.tuileSize; 

    double plateauX = (Screen.w-sizePlateau)/2;
    double plateauY = (Screen.h-sizePlateau)/2;

    //SDL_Rect rect_tuileRestante = {plateauX+4*infoDisplay.borderSize+3*infoDisplay.tuileSize, plateauY-infoDisplay.tuileSize-infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize};
    SDL_Rect rect_tuileRestante = {Screen.w-(Screen.w-sizePlateau)/4,(Screen.h-infoDisplay.tuileSize)/2,infoDisplay.tuileSize, infoDisplay.tuileSize};
    SDL_Rect rect_itemRestant = {rect_tuileRestante.x+posItem, rect_tuileRestante.y+posItem, infoDisplay.itemSize, infoDisplay.itemSize};

    SDL_Rect magnet_lock[13] = {plateauX+2*infoDisplay.borderSize+infoDisplay.tuileSize, plateauY-infoDisplay.tuileSize-infoDisplay.borderSize, infoDisplay.tuileSize, 0,
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
                                plateauX-infoDisplay.tuileSize-infoDisplay.borderSize, plateauY+infoDisplay.tuileSize+2*infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize,
                                Screen.w-(Screen.w-sizePlateau)/4,(Screen.h-infoDisplay.tuileSize)/2,infoDisplay.tuileSize, infoDisplay.tuileSize};

    SDL_Rect rect_TR  = {Screen.w-(spaceForHUD+infoDisplay.cadreTR)/2, (Screen.h-infoDisplay.cadreTR)/2, infoDisplay.cadreTR, infoDisplay.cadreTR};

    SDL_Rect rect_button[4] = {rect_TR.x-infoDisplay.cadreTR-infoDisplay.borderSize*4, rect_TR.y, infoDisplay.cadreTR, infoDisplay.cadreTR,
                               rect_TR.x+infoDisplay.cadreTR+infoDisplay.borderSize*4, rect_TR.y, infoDisplay.cadreTR, infoDisplay.cadreTR,
                               rect_TR.x-infoDisplay.cadreTR-infoDisplay.borderSize*4, rect_TR.y+infoDisplay.cadreTR+infoDisplay.borderSize*2, infoDisplay.cadreTR*3+infoDisplay.borderSize*8, infoDisplay.cadreTR,
                               rect_TR.x-infoDisplay.cadreTR-infoDisplay.borderSize*4, rect_TR.y-infoDisplay.cadreTR-infoDisplay.borderSize*2, infoDisplay.cadreTR*3+infoDisplay.borderSize*8, infoDisplay.cadreTR};

    SDL_Rect rect_Tick[2];

    int posTuileRestante = 12;

    uint8_t playerTurn = 0;
    uint8_t gameState = 0;

    AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock, gameTexture, cursorX, cursorY, rect_TR, rect_button, rect_Tick, playerTurn);

    while( windowOpen ){
        
        SDL_Event event;

        while( SDL_PollEvent(&event) ){

            switch( event.type ){
            case SDL_MOUSEMOTION:
                cursorX = event.motion.x;
                cursorY = event.motion.y;
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ){
                case SDLK_ESCAPE:
                    *cursorX_ = cursorX;
                    *cursorY_ = cursorY;
                    return;
                    break;
                case SDLK_UP:
                case SDLK_z:
                    movePlayer(playerTurn, 0);
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    movePlayer(playerTurn, 1);
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    movePlayer(playerTurn, 2);
                    break;
                case SDLK_LEFT:
                case SDLK_q:
                    movePlayer(playerTurn, 3);
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
                        cursorY <= rect_tuileRestante.y + rect_tuileRestante.h && gameState == 0)
                    {
                        magnet_lock[0].h = 1; // active les cadres de magnet lock

                        do {
                            if(SDL_PollEvent(&event)){
                                cursorX = event.motion.x;
                                cursorY = event.motion.y;

                                rect_tuileRestante.x = cursorX-infoDisplay.tuileSize/2; rect_tuileRestante.y = cursorY-infoDisplay.tuileSize/2;
                                rect_itemRestant.x = rect_tuileRestante.x+posItem ; rect_itemRestant.y = rect_tuileRestante.y+posItem;
                                rect_Tick[0].x = 0;
                            }
                            AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock, gameTexture, cursorX, cursorY, rect_TR, rect_button, rect_Tick, playerTurn);
                        }while(event.type != SDL_MOUSEBUTTONUP);

                        //on quitte le while donc le bouton est relaché

                        for(int i = 0; i < 13; i++){
                            if( cursorX >= (magnet_lock[i].x - infoDisplay.tuileSize/2) &&
                                cursorX <= (magnet_lock[i].x + magnet_lock[i].w + infoDisplay.tuileSize/2) &&
                                cursorY >= (magnet_lock[i].y - infoDisplay.tuileSize/2) &&
                                cursorY <= (magnet_lock[i].y + magnet_lock[i].h + infoDisplay.tuileSize/2))
                            {
                                rect_tuileRestante.x = magnet_lock[i].x; rect_tuileRestante.y = magnet_lock[i].y;
                                rect_itemRestant.x = magnet_lock[i].x+posItem; rect_itemRestant.y = magnet_lock[i].y+posItem;
                                createRectTick(rect_Tick, i, magnet_lock);
                                posTuileRestante = i;
                                if (posTuileRestante == 12) {
                                    rect_Tick[0].x = 0;
                                    break;
                                }
                                if (posTuileRestante == 0) {
                                    magnet_lock[0].h = 0; // desactive les cadres de magnet lock
                                    break;
                                }
                                
                                break;
                            }
                            else if(i == 12){
                                rect_tuileRestante.x = magnet_lock[posTuileRestante].x; rect_tuileRestante.y = magnet_lock[posTuileRestante].y;
                                rect_itemRestant.x = magnet_lock[posTuileRestante].x+posItem; rect_itemRestant.y = magnet_lock[posTuileRestante].y+posItem;
                            }

                            magnet_lock[0].h = 0; // desactive les cadres de magnet lock

                            AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock, gameTexture, cursorX, cursorY, rect_TR, rect_button, rect_Tick, playerTurn);
                        }
                    }

                    // LES BOUTONS DE ROTATION

                    for(int i = 0; i < 2; i++){
                        if( cursorX >= rect_button[i].x &&
                            cursorX <= rect_button[i].x + rect_button[i].w &&
                            cursorY >= rect_button[i].y &&
                            cursorY <= rect_button[i].y + rect_button[i].h)
                        {
                            rotateTuile(&tuileRestante.tuile, i);
                        }
                    }

                    // LES BOUTONS YES ET NO

                    for(int i = 0; i < 2; i++){
                        if( cursorX >= rect_Tick[i].x &&
                            cursorX <= rect_Tick[i].x + rect_Tick[i].w &&
                            cursorY >= rect_Tick[i].y &&
                            cursorY <= rect_Tick[i].y + rect_Tick[i].h && gameState == 0)
                        {
                            if (i == 0){ // bouton NO
                                posTuileRestante = 12;
                                rect_tuileRestante.x = magnet_lock[12].x; rect_tuileRestante.y = magnet_lock[12].y;
                                rect_itemRestant.x = magnet_lock[12].x+posItem; rect_itemRestant.y = magnet_lock[12].y+posItem;
                                rect_Tick[0].x = 0; // cache les ticks
                            }else{ // bouton YES
                                posTuileRestante = pushTuile(posTuileRestante);
                                if(posTuileRestante >= 0){
                                    rect_tuileRestante.x = magnet_lock[posTuileRestante].x; rect_tuileRestante.y = magnet_lock[posTuileRestante].y;
                                    rect_itemRestant.x = magnet_lock[posTuileRestante].x+posItem; rect_itemRestant.y = magnet_lock[posTuileRestante].y+posItem;
                                    gameState = 1; //le joueur ne peut plus déplacer d'autres tuiles
                                } else printf("déplacement impossible"); // A REMPLACER PAR LE MESSAGE SUR LE PLATEAU
                                rect_Tick[0].x = 0; // cache les ticks
                            }
                        }
                    }

                    // LE BOUTON TOUR SUIVANT

                    if( cursorX >= rect_button[2].x &&
                            cursorX <= rect_button[2].x + rect_button[2].w &&
                            cursorY >= rect_button[2].y &&
                            cursorY <= rect_button[2].y + rect_button[2].h)
                        {
                            if(playerTurn < 3 ) playerTurn += 1;
                            else playerTurn = 0;

                            //on remet la tuile restante dans le cadre
                            posTuileRestante = 12;
                            rect_tuileRestante.x = magnet_lock[12].x; rect_tuileRestante.y = magnet_lock[12].y;
                            rect_itemRestant.x = magnet_lock[12].x+posItem; rect_itemRestant.y = magnet_lock[12].y+posItem;
                            rect_Tick[0].x = 0; // cache les ticks

                            //on permet au nouveau joeur de pousser les tuiles
                            gameState = 0;
                        }

                }             
                break;
            case SDL_QUIT:
                windowOpen = SDL_FALSE; // ferme la fenêtre
                *cursorX_ = cursorX;
                *cursorY_ = cursorY;
                break;
            default:
                continue;
            }
        }
        AffichePlateauTuileItem(renderer, rect_tuileRestante, rect_itemRestant, magnet_lock, gameTexture, cursorX, cursorY, rect_TR, rect_button, rect_Tick, playerTurn);
    }
}

void DeplaceTuile(SDL_Renderer *renderer, SDL_Rect rect_tuileRestante, SDL_Rect rect_itemRestant, TextureJeu *gameTexture){

    printImageFromTexture(renderer, gameTexture->tuile[tuileRestante.tuile-1], rect_tuileRestante);
    if( tuileRestante.item != 0) printImageFromTexture(renderer, gameTexture->item[tuileRestante.item-1], rect_itemRestant);

}

void ResetRender(SDL_Renderer * renderer, Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void afficherHUD(SDL_Renderer *renderer, TextureJeu *gameTexture, int cursorX, int cursorY, SDL_Rect rect_button[4], SDL_Rect rect_TR, SDL_Rect rect_Tick[2], int playerTurn){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    size_t sizePlateau = (7*infoDisplay.tuileSize + 8*infoDisplay.borderSize);

    size_t spaceForHUD = (Screen.w-sizePlateau)/2-infoDisplay.borderSize/2-infoDisplay.tuileSize; 

    // LES CADRES POUR LES JOUEURS 

    for(int i=0 ; i<nbplayer ; i++){
        SDL_Rect rect_cadre = {(spaceForHUD-infoDisplay.cadreSizeX)/2-infoDisplay.borderSize, (Screen.h-sizePlateau)/2+(infoDisplay.cadreSizeY+(double)(sizePlateau-nbplayer*infoDisplay.cadreSizeY)/nbplayer-1)*i, infoDisplay.cadreSizeX, infoDisplay.cadreSizeY};
        printImageFromTexture(renderer, gameTexture->cadre, rect_cadre);
        SDL_Rect rect_player = {rect_cadre.x+infoDisplay.cadreSizeY/4, rect_cadre.y+infoDisplay.cadreSizeY/4, infoDisplay.cadreSizeY/2, infoDisplay.cadreSizeY/2};
        printImageFromTexture(renderer, gameTexture->skin[i] ,rect_player);
        SDL_Rect rect_item32 = {rect_cadre.x+140*2.25, rect_cadre.y+25*2.25, infoDisplay.itemSize*2, infoDisplay.itemSize*2};
        printImageFromTexture(renderer, gameTexture->item32[playerCard[i].tab[playerData[i].itemFound]], rect_item32);
        SDL_Rect rect_textPLayer = {rect_cadre.x+96*facteurResize, rect_cadre.y+12*facteurResize, infoDisplay.text_playerX, infoDisplay.text_playerY};
        printImageFromTexture(renderer, gameTexture->text_player[i], rect_textPLayer);
        SDL_Rect rect_TickProgression = {rect_textPLayer.x+44*2.25, rect_textPLayer.y, 5, 18};
        
        for(int j=0 ; j<playerData[i].itemFound ; j++ ){
            printImageFromTexture(renderer, gameTexture->tick[19],rect_TickProgression);
            rect_TickProgression.x += 10;
        }

        for(int j=0 ; j<24/nbplayer - playerData[i].itemFound ; j++){
            printImageFromTexture(renderer, gameTexture->tick[18],rect_TickProgression);
            rect_TickProgression.x += 10;
        }
    }   

    // LE CADRE QUI DIT A QUI C'EST DE JOUER

    printImageFromTexture(renderer, gameTexture->cadreTuileRestante, rect_TR);

    // LES BOUTONS DE ROTATION DE TUILE, DE VALIDATION ET L'EMPLACEMENT POUR LA TUILE

    for(int i = 0; i < 3; i++){
        if(cursorX >= rect_button[i].x && cursorX <= rect_button[i].x + rect_button[i].w && cursorY >= rect_button[i].y && cursorY <= rect_button[i].y + rect_button[i].h) {
            printImageFromTexture(renderer, gameTexture->tick[i*2+5], rect_button[i]);
        }else printImageFromTexture(renderer, gameTexture->tick[i*2+4], rect_button[i]);
    }
    printImageFromTexture(renderer, gameTexture->tick[14+playerTurn], rect_button[3]);

    // L'AFFICHAGE DES BOUTON DE VALIDATION ET D'ANNULATION

    if(rect_Tick[0].x != 0){
        for(int i = 0; i < 2; i++){
            if(cursorX >= rect_Tick[i].x && cursorX <= rect_Tick[i].x + rect_Tick[i].w && cursorY >= rect_Tick[i].y && cursorY <= rect_Tick[i].y + rect_Tick[i].h) {
                printImageFromTexture(renderer, gameTexture->tick[i*2+1], rect_Tick[i]);
            }else printImageFromTexture(renderer, gameTexture->tick[i*2], rect_Tick[i]);
        }
    }
}

void printBG(SDL_Renderer *renderer, TextureJeu *gameTexture){
    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    double vitesseScroll = (double)Screen.w/(double)24000; //pour que la vitesse reste la même peu importe la taille d'écran

    for(int i = 0; i < 4; i++){
        SDL_Rect rect_BG1 = {(int)(-Screen.w+(int)((vitesseScroll*current_frame)*i)%Screen.w), 0, Screen.w, Screen.h};
        SDL_Rect rect_BG2 = {(int)(0+(vitesseScroll*current_frame)*i)%Screen.w, 0, Screen.w, Screen.h};

        printImageFromTexture(renderer, gameTexture->BG[i], rect_BG1);
        printImageFromTexture(renderer, gameTexture->BG[i], rect_BG2);
    }

    current_frame = (current_frame == (int)(Screen.w/vitesseScroll-1))? 0 : ++current_frame;
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

    //SDL_RenderFillRect(renderer,&rect_button);

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

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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

void AfficheTuileItem(SDL_Renderer *renderer, TextureJeu *gameTexture, struct timeval time){

    SDL_DisplayMode Screen;
    SDL_GetCurrentDisplayMode(0, &Screen);

    size_t sizePlateau = (7*infoDisplay.tuileSize + 8*infoDisplay.borderSize);
    size_t posItem = (infoDisplay.tuileSize-infoDisplay.itemSize)/2;

    SDL_Rect rect_tuile = {(Screen.w-sizePlateau)/2+infoDisplay.borderSize, (Screen.h-sizePlateau)/2+infoDisplay.borderSize, infoDisplay.tuileSize, infoDisplay.tuileSize};
    SDL_Rect rect_item = {(Screen.w-sizePlateau)/2+infoDisplay.borderSize+posItem, (Screen.h-sizePlateau)/2+infoDisplay.borderSize+posItem, infoDisplay.itemSize , infoDisplay.itemSize};

    int playerHere[4] = {-1,-1,-1,-1};
    int nbPlayerHere = 0;

    for( int i=0 ; i<7 ; i++ ){
        rect_tuile.x = (Screen.w-sizePlateau)/2+infoDisplay.borderSize;
        rect_item.x = (Screen.w-sizePlateau)/2+infoDisplay.borderSize+posItem;
        for( int j=0 ; j<7 ; j++){
            nbPlayerHere = 0;
            for(int k = 0; k < 4; k++){
                if(playerData[k].posX == i && playerData[k].posY == j){
                    playerHere[nbPlayerHere] = k;
                    nbPlayerHere++;
                    checkObjectif(i, j, k);
                }else playerHere[k] = -1;
            }
            if(nbPlayerHere != 0){
                printImageFromTexture(renderer, gameTexture->tuile[SDLplateau[i][j].tuile-1],rect_tuile);
                printImageFromTexture(renderer, gameTexture->player[playerHere[time.tv_sec%nbPlayerHere]],rect_item); // magie noire pour faire clignotter les joueurs si ils sont plusieurs sur la même case
            }else{
                printImageFromTexture(renderer, gameTexture->tuile[SDLplateau[i][j].tuile-1],rect_tuile);
                if(SDLplateau[i][j].item!=0 && itemTrouves[SDLplateau[i][j].item-1] == 0) printImageFromTexture(renderer, gameTexture->item[SDLplateau[i][j].item-1],rect_item);
            }

            rect_tuile.x += infoDisplay.tuileSize+infoDisplay.borderSize;
            rect_item.x += infoDisplay.tuileSize+infoDisplay.borderSize;
        }
        rect_tuile.y += infoDisplay.tuileSize+infoDisplay.borderSize;
        rect_item.y += infoDisplay.tuileSize+infoDisplay.borderSize;
    }
}


void AffichePlateauTuileItem(SDL_Renderer *renderer, SDL_Rect rect_tuileRestante, SDL_Rect rect_itemRestant, SDL_Rect magnet_lock[13], TextureJeu *gameTexture, int cursorX, int cursorY, SDL_Rect rect_TR, SDL_Rect rect_button[4], SDL_Rect rect_Tick[2], int playerTurn){

    int total_t;

    int microsecFac = 1000000;

    int fpsMax = 100;
    int freqMax = microsecFac/fpsMax;

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    ResetRender(renderer, Background);
    printBG(renderer, gameTexture);
    //printDebugGrid(renderer);
    afficherHUD(renderer, gameTexture, cursorX, cursorY, rect_button, rect_TR, rect_Tick, playerTurn);
    printMagnetLockRect(renderer, magnet_lock, gameTexture);
    AffichePlateau(renderer);
    AfficheTuileItem(renderer, gameTexture, start);
    DeplaceTuile(renderer, rect_tuileRestante, rect_itemRestant, gameTexture); 

    SDL_Rect rect_mouse = {cursorX, cursorY, 32, 32};
    printImageFromTexture(renderer, gameTexture->mouse, rect_mouse);

    gettimeofday(&stop, NULL);

    total_t = (stop.tv_sec - start.tv_sec) * microsecFac + stop.tv_usec - start.tv_usec;

    delay(freqMax-total_t);

    //printf("Frame time: %d us, ", total_t  );

    int fps = microsecFac / (freqMax+((total_t>freqMax)? total_t-freqMax : 0)); // 100 fps sauf si le frame time est supérieur à 10ms
    //printf("FPS = %d\n", fps);

    SDL_RenderPresent(renderer);
}

void printMagnetLockRect(SDL_Renderer *renderer, SDL_Rect magnet_lock[12], TextureJeu *gameTexture){
    if(magnet_lock[0].h == 0){
        return;
    }else magnet_lock->h = magnet_lock->w;

    SDL_SetRenderDrawColor(renderer,0,0,0, SDL_ALPHA_OPAQUE);
    for(int i = 0; i < 12; i++){
        //SDL_RenderDrawRect(renderer, &magnet_lock[i]);
        printImageFromTexture(renderer, gameTexture->tick[10+i/3], magnet_lock[i]);
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

int pushTuile(int emplacement){
    Case bufferTuile = tuileRestante;
    if(emplacement < 3){
        int pos = (emplacement % 3)*2 + 1;

        for(int i = 0; i < 4; i++){
            if(playerData[i].posY == pos){
                if(playerData[i].posX != 6)playerData[i].posX += 1;
                else playerData[i].posX = 0;
            }
        }

        tuileRestante = SDLplateau[6][pos]; // définition de la nouvelle tuile restante
        for(int i = 6; i > 0; i-=1){
            SDLplateau[i][pos] = SDLplateau[i-1][pos];
        }
        SDLplateau[0][pos] = bufferTuile; // tuile restante précédente
    }
    else if(emplacement >= 3 && emplacement < 6){
        int pos = (emplacement % 3)*2 + 1;
        
        for(int i = 0; i < 4; i++){
            if(playerData[i].posX == pos){
                if(playerData[i].posY != 0)playerData[i].posY -= 1;
                else playerData[i].posY = 6;
            }
        }

        tuileRestante = SDLplateau[pos][0]; // définition de la nouvelle tuile restante
        for(int i = 0; i < 6; i+=1){
            SDLplateau[pos][i] = SDLplateau[pos][i+1];
        }
        SDLplateau[pos][6] = bufferTuile; // tuile restante précédente
    }
    else if(emplacement >= 6 && emplacement < 9){
        int pos = (emplacement % 3)*2 + 1;
        pos = (pos == 1) ? 5 : (pos == 5) ? 1 : 3; // part de 1 3 5 pour avoir 5 3 1

        for(int i = 0; i < 4; i++){
            if(playerData[i].posY == pos){
                if(playerData[i].posX != 0)playerData[i].posX -= 1;
                else playerData[i].posX = 6;
            }
        }
        tuileRestante = SDLplateau[0][pos]; // définition de la nouvelle tuile restante
        for(int i = 0; i < 6; i+=1){
            SDLplateau[i][pos] = SDLplateau[i+1][pos];
        }
        SDLplateau[6][pos] = bufferTuile; // tuile restante précédente
    }
    else if(emplacement >= 9 && emplacement < 12){
        int pos = (emplacement % 3)*2 + 1;
        pos = (pos == 1) ? 5 : (pos == 5) ? 1 : 3; // part de 1 3 5 pour avoir 5 3 1

        for(int i = 0; i < 4; i++){
            if(playerData[i].posX == pos){
                if(playerData[i].posY != 6)playerData[i].posY += 1;
                else playerData[i].posY = 0;
            }
        }

        tuileRestante = SDLplateau[pos][6]; // définition de la nouvelle tuile restante
        for(int i = 6; i > 0; i-=1){
            SDLplateau[pos][i] = SDLplateau[pos][i-1];
        }
        SDLplateau[pos][0] = bufferTuile; // tuile restante précédente
    }
    return (emplacement == 0)? 8: (emplacement == 1)? 7: (emplacement == 2)? 6: 
           (emplacement == 3)? 11: (emplacement == 4)? 10: (emplacement == 5)? 9:
           (emplacement == 6)? 2: (emplacement == 7)? 1: (emplacement == 8)? 0:
           (emplacement == 9)? 5: (emplacement == 10)? 4: (emplacement == 11)? 3: -1;
}

float setGUIsize(uint8_t size){
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
            facteurResize = 3.0;
            break;                       
    }

    infoDisplay.itemSize = 16*facteurResize;
    infoDisplay.tuileSize = 54*facteurResize;
    infoDisplay.borderSize = 2*facteurResize;
    infoDisplay.cadreSizeX = 176*facteurResize*1.5;
    infoDisplay.cadreSizeY = 64*facteurResize*1.5;
    infoDisplay.skinSizeX = 64*facteurResize;
    infoDisplay.skinSizeY = 64*facteurResize;
    infoDisplay.cadreTR = 28*facteurResize*3;
    infoDisplay.text_playerX = 36*facteurResize*1.5;
    infoDisplay.text_playerY = 8*facteurResize*1.5;


    char cheminImage[100];
    char cheminImageRedim[100];

    // redimentionnement des boutons 
    for(int i = 1; i <= 2; i++){
        sprintf(cheminImage, "images/default/button/button%d.bmp", i);

        SDL_Surface *image = SDL_LoadBMP(cheminImage);
        handleSurfaceError(image);

        SDL_Surface *image_redim = SDL_CreateRGBSurface(0, image->w*facteurResize*3, image->h*facteurResize*3, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
        handleSurfaceError(image_redim);

        infoDisplay.menuButtonX = image_redim->w;
        infoDisplay.menuButtonY = image_redim->h;
        
        redimImage(image, image_redim);

        sprintf(cheminImageRedim, "images/formated/button/button%d.bmp", i);

        // Enregistrement de l'image redimensionnée
        SDL_SaveBMP(image_redim, cheminImageRedim);
    }

    for(int i = 1; i <= 2; i++){
        sprintf(cheminImage, "images/default/button/button%d_pressed.bmp", i);

        SDL_Surface *image = SDL_LoadBMP(cheminImage);
        handleSurfaceError(image);

        SDL_Surface *image_redim = SDL_CreateRGBSurface(0, image->w*facteurResize*3, image->h*facteurResize*3, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
        handleSurfaceError(image_redim);

        infoDisplay.menuButtonX = image_redim->w;
        infoDisplay.menuButtonY = image_redim->h;
        
        redimImage(image, image_redim);

        sprintf(cheminImageRedim, "images/formated/button/button%d_pressed.bmp", i);

        // Enregistrement de l'image redimensionnée
        SDL_SaveBMP(image_redim, cheminImageRedim);
    }

    return facteurResize;
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
    for(int i = 1; i <= 4; i++){
        sprintf(cheminImage, "images/formated/skin/skin_%d.bmp", i);
        remove(cheminImage);
    }
}

void delay(time_t pauseTime){
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    int total_t = 0;
    while(total_t < pauseTime){
       gettimeofday(&stop, NULL);
       total_t = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    }
}

void rotateTuile(uint8_t* tuileRestante, int direction){
    switch (direction)
    {
    case 0:
        if(*tuileRestante != 4 && *tuileRestante != 8 && *tuileRestante != 10){
            *tuileRestante += 1;
        }else *tuileRestante = (*tuileRestante == 4)? 1: (*tuileRestante == 8)? 5: (*tuileRestante == 10)? 9: -1;
        break;
    case 1:
        if(*tuileRestante != 1 && *tuileRestante != 5 && *tuileRestante != 9){
            *tuileRestante -= 1;
        }else *tuileRestante = (*tuileRestante == 1)? 4: (*tuileRestante == 5)? 8: (*tuileRestante == 9)? 10: -1;
        break;
    }
}

void createRectTick(SDL_Rect rect_Tick[2], int emplacement, SDL_Rect magnet_lock[13]){
    int decalageX1, decalageX2, decalageY1, decalageY2;

    size_t tickSize = (infoDisplay.tuileSize-3*infoDisplay.borderSize)/2;
    
    decalageX1 = (emplacement < 3)? infoDisplay.borderSize : 
                 (emplacement < 6)? infoDisplay.tuileSize+infoDisplay.borderSize*2 :
                 (emplacement < 9)? infoDisplay.borderSize : -(tickSize+infoDisplay.borderSize*2);

    decalageX2 = (emplacement < 3)? tickSize + infoDisplay.borderSize*2 : 
                 (emplacement < 6)? infoDisplay.tuileSize+infoDisplay.borderSize*2 :
                 (emplacement < 9)? tickSize + infoDisplay.borderSize*2 : -(tickSize+infoDisplay.borderSize*2);

    decalageY1 = (emplacement < 3)? -(tickSize+infoDisplay.borderSize*2) : 
                 (emplacement < 6)? infoDisplay.borderSize :
                 (emplacement < 9)? infoDisplay.tuileSize+infoDisplay.borderSize*2 : infoDisplay.borderSize;

    decalageY2 = (emplacement < 3)? -(tickSize+infoDisplay.borderSize*2) : 
                 (emplacement < 6)? tickSize + infoDisplay.borderSize*2 :
                 (emplacement < 9)? infoDisplay.tuileSize+infoDisplay.borderSize*2 : tickSize + infoDisplay.borderSize*2;

    rect_Tick[0].x = magnet_lock[emplacement].x+decalageX1; rect_Tick[0].y = magnet_lock[emplacement].y+decalageY1;
    rect_Tick[0].w = tickSize; rect_Tick[0].h = tickSize;

    rect_Tick[1].x = magnet_lock[emplacement].x+decalageX2; rect_Tick[1].y = magnet_lock[emplacement].y+decalageY2;
    rect_Tick[1].w = tickSize; rect_Tick[1].h = tickSize;
}

void RandomCard( int nbPlayer ){

    melangerTab(nbCardRestant, 24);

    int n=0, p=0;

    switch ( nbPlayer )
    {
    case 2:
        for( int i=0 ; i<24 ; i++ ){

            if( i==12 ){
                n++;
                p=0;
            }
            playerCard[n].tab[p] = nbCardRestant[i];
            p++;
        }
        break;
    case 3:
        for( int i=0 ; i<24 ; i++ ){

            if( i==8 || i==16 ){
                n++;
                p=0;
            }
            playerCard[n].tab[p] = nbCardRestant[i];
            p++;
        }
        break;
    case 4:
        for( int i=0 ; i<24 ; i++ ){

            if( i==6 || i==12 || i==18 ){
                n++;
                p=0;
            }
            playerCard[n].tab[p] = nbCardRestant[i];
            p++;
        }
        break;
    default:
        break;
    }   

    for(int i = 0; i < 48; i++){
        playerCard[i/12].tab[i%12] -= 1;
    }
}

void checkObjectif(int x, int y, int player){
    if(playerCard[player].tab[playerData[player].itemFound] == SDLplateau[x][y].item-1){
        playerData[player].itemFound += 1;
        itemTrouves[SDLplateau[x][y].item-1] = 1;
    }    
}