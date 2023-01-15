#ifndef textures_
#define textures_

#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include "bmp_manip.h"

typedef struct{
    SDL_Texture *BG[4], *cadre, *skin[4], *tuile[12], *item[24], *player[4], *mouse, *cadreTuileRestante, *tick[20], *item32[24], *text_player[4];
}TextureJeu;

typedef struct{
    SDL_Texture *button[2], *buttonPressed[2], *mouse;
}TextureMenu;

typedef struct{
    SDL_Texture *cadre, *mouse, *tick[10], *playerText[4], *BG[20];
}TextureSelectionJeu;

typedef struct{
    SDL_Texture *cadre[3], *cadrePlayer[2];
}TextureFinJeu;

typedef struct{
    size_t itemSize, tuileSize, borderSize, cadreSizeX, cadreSizeY, skinSizeX, skinSizeY, menuButtonX, menuButtonY, cadreTR, text_playerX, text_playerY, tickProgression;
}InfoDisplay;

TextureJeu loadGameTexture(SDL_Renderer *renderer, int backgroundTheme);
TextureMenu loadMenuTexture(SDL_Renderer *renderer);
TextureSelectionJeu loadTextureSelectionJeu(SDL_Renderer *renderer);
TextureFinJeu loadTextureFinJeu(SDL_Renderer *renderer);
void unloadTexturesPlateau(SDL_Renderer *renderer ,TextureJeu *gameTexture);
float setGUIsize(uint8_t size, InfoDisplay *infoDisplay);

TextureMenu loadMenuTexture(SDL_Renderer *renderer){
    TextureMenu menuTexture;

    SDL_Surface *surface = SDL_LoadBMP("images/default/HUD/mouse.bmp");
    handleSurfaceError(surface);
    menuTexture.mouse = SDL_CreateTextureFromSurface(renderer, surface);
    handleTextureError(menuTexture.mouse);
    SDL_FreeSurface(surface);

    return menuTexture;
}

TextureSelectionJeu loadTextureSelectionJeu(SDL_Renderer *renderer){
    TextureSelectionJeu textureSelectionJeu;

    SDL_Surface *cadre_surface = SDL_LoadBMP("images/default/HUD/cadre_selection.bmp");
    handleSurfaceError(cadre_surface);
    textureSelectionJeu.cadre = SDL_CreateTextureFromSurface(renderer, cadre_surface);
    handleTextureError(textureSelectionJeu.cadre);
    SDL_FreeSurface(cadre_surface);

    // CURSEUR

    SDL_Surface *mouse_surface = SDL_LoadBMP("images/default/HUD/mouse.bmp");
    handleSurfaceError(mouse_surface);
    textureSelectionJeu.mouse = SDL_CreateTextureFromSurface(renderer, mouse_surface);
    handleTextureError(textureSelectionJeu.mouse);
    SDL_FreeSurface(mouse_surface);

    // TICK

    for(int i = 0; i < 10; i++){
        char cheminImage[100];
        int indiceTick = (i<2)? 21: (i<6)? 21: -5; 
        sprintf(cheminImage, "images/default/HUD/tick%d.bmp", i+indiceTick);
        SDL_Surface *tick_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(tick_surface);
        textureSelectionJeu.tick[i] = SDL_CreateTextureFromSurface(renderer, tick_surface);
        handleTextureError(textureSelectionJeu.tick[i]);
        SDL_FreeSurface(tick_surface);
    }

    for(int i = 0; i < 4; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/HUD/textP%d.bmp", i+5);
        SDL_Surface *playerText_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(playerText_surface);
        textureSelectionJeu.playerText[i] = SDL_CreateTextureFromSurface(renderer, playerText_surface);
        handleTextureError(textureSelectionJeu.playerText[i]);
        SDL_FreeSurface(playerText_surface);
    }

    // BACKGROUND

    for(int i = 0; i < 20; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/HUD/BG/%d/%d.bmp", i/4+1, (i%4)+1);
        SDL_Surface *BG_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(BG_surface);
        textureSelectionJeu.BG[i] = SDL_CreateTextureFromSurface(renderer, BG_surface);
        handleTextureError(textureSelectionJeu.BG[i]);
        SDL_FreeSurface(BG_surface);
    }

    return textureSelectionJeu;
}

TextureJeu loadGameTexture(SDL_Renderer *renderer, int backgroundTheme){
    TextureJeu gameTexture;

    // BACKGROUND

    for(int i = 0; i < 4; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/HUD/BG/%d/%d.bmp",backgroundTheme+1, i+1);
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

TextureFinJeu loadTextureFinJeu(SDL_Renderer *renderer){

    TextureFinJeu textureFinJeu;

    // Cadre principal
    for(int i = 0; i < 3; i++){
        char cheminImage[100];
        sprintf(cheminImage, "images/default/HUD/cadreFin%d.bmp", i+1);
        SDL_Surface *cadre_surface = SDL_LoadBMP(cheminImage);
        handleSurfaceError(cadre_surface);
        textureFinJeu.cadre[i] = SDL_CreateTextureFromSurface(renderer, cadre_surface);
        handleTextureError(textureFinJeu.cadre[i]);
        SDL_FreeSurface(cadre_surface);
    }

    //  Cadre Player

    SDL_Surface *cadrePlayer_surface1 = SDL_LoadBMP("images/default/HUD/cadre3.bmp");
    handleSurfaceError(cadrePlayer_surface1);
    textureFinJeu.cadrePlayer[0] = SDL_CreateTextureFromSurface(renderer, cadrePlayer_surface1);
    handleTextureError(textureFinJeu.cadrePlayer[0]);
    SDL_FreeSurface(cadrePlayer_surface1);

    SDL_Surface *cadrePlayer_surface2 = SDL_LoadBMP("images/default/HUD/cadre4.bmp");
    handleSurfaceError(cadrePlayer_surface2);
    textureFinJeu.cadrePlayer[1] = SDL_CreateTextureFromSurface(renderer, cadrePlayer_surface2);
    handleTextureError(textureFinJeu.cadrePlayer[1]);
    SDL_FreeSurface(cadrePlayer_surface2);


    return textureFinJeu;
}

void unloadTexturesPlateau(SDL_Renderer *renderer ,TextureJeu *gameTexture){

    for(int i = 0; i < sizeof(TextureJeu); i += sizeof(SDL_Texture *)){ // parcourt la struct de textures

        SDL_Texture* pSurTexture = (SDL_Texture *) ((int)gameTexture + i); // calcule la valeur du pointeur de la texture actuelle
        SDL_DestroyTexture(pSurTexture);

    }
}

float setGUIsize(uint8_t size, InfoDisplay *infoDisplay){
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

    infoDisplay->itemSize = 16*facteurResize;
    infoDisplay->tuileSize = 54*facteurResize;
    infoDisplay->borderSize = 2*facteurResize;
    infoDisplay->cadreSizeX = 176*facteurResize*1.5;
    infoDisplay->cadreSizeY = 64*facteurResize*1.5;
    infoDisplay->skinSizeX = 64*facteurResize;
    infoDisplay->skinSizeY = 64*facteurResize;
    infoDisplay->cadreTR = 28*facteurResize*3;
    infoDisplay->text_playerX = 36*facteurResize*1.5;
    infoDisplay->text_playerY = 8*facteurResize*1.5;


    char cheminImage[100];
    char cheminImageRedim[100];

    // redimentionnement des boutons 
    for(int i = 1; i <= 2; i++){
        sprintf(cheminImage, "images/default/button/button%d.bmp", i);

        SDL_Surface *image = SDL_LoadBMP(cheminImage);
        handleSurfaceError(image);

        SDL_Surface *image_redim = SDL_CreateRGBSurface(0, image->w*facteurResize*3, image->h*facteurResize*3, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
        handleSurfaceError(image_redim);

        infoDisplay->menuButtonX = image_redim->w;
        infoDisplay->menuButtonY = image_redim->h;
        
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

        infoDisplay->menuButtonX = image_redim->w;
        infoDisplay->menuButtonY = image_redim->h;
        
        redimImage(image, image_redim);

        sprintf(cheminImageRedim, "images/formated/button/button%d_pressed.bmp", i);

        // Enregistrement de l'image redimensionnée
        SDL_SaveBMP(image_redim, cheminImageRedim);
    }

    return facteurResize;
}

#endif