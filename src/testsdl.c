#include <stdio.h>
#include <SDL.h>

// gcc src/testsdl.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2

void SDL_ExitError(const char *message);

int main(int argc, char **argv)
{

    SDL_Window *window;
    SDL_Renderer *rendu;

    if(SDL_Init(SDL_INIT_VIDEO))
        SDL_ExitError("Erreur initialisation video");

    if(SDL_CreateWindowAndRenderer(1920,1080, SDL_RENDERER_ACCELERATED, &window, &rendu)) // CREE UNE FENETRE ET UN RENDU
        SDL_ExitError("Erreur de fenetre et rendu");

    /*if(SDL_SetRenderDrawColor(rendu,82,192,213, SDL_ALPHA_OPAQUE)) // SET COULEUR POUR DESSINER
        SDL_ExitError("Erreur drawcolor pour rendu");

    if(SDL_RenderDrawPoint(rendu, 960,540)) // DESSINE UN POINT EN COORD 940/540 (au centre)
        SDL_ExitError("Erreur dessin point");

    if(SDL_RenderDrawLine(rendu,0,0,1920,1080)) // DESSINE UNE LIGNE DEPUIS 0/0 à 1920/1080
        SDL_ExitError("Erreur dessin ligne");

    SDL_Rect rectangle;
    rectangle.x = 300;
    rectangle.y = 300;
    rectangle.h = 120;
    rectangle.w = 200;

    if(SDL_RenderFillRect(rendu,&rectangle) != 0) // REMPLI
        SDL_ExitError("Erreur dessin rectangle");

    if(SDL_SetRenderDrawColor(rendu,190,245,255, SDL_ALPHA_OPAQUE)) // SET COULEUR POUR DESSINER
        SDL_ExitError("Erreur drawcolor pour rendu");

    if(SDL_RenderDrawRect(rendu,&rectangle) != 0) // CONTOUR 
        SDL_ExitError("Erreur dessin rectangle");

    SDL_RenderPresent(rendu); // ACTIVE LE RENDU  */

    SDL_Surface *image = NULL;
    SDL_Texture *texture = NULL;

    image = SDL_LoadBMP("src/test.bmp");

    if(image){
        SDL_DestroyRenderer( rendu );
        SDL_DestroyWindow( window );
        SDL_ExitError("Erreur de chargement de l'image");
    }

    texture = SDL_CreateTextureFromSurface(rendu, image);
    SDL_FreeSurface(image);

    if(texture){
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(window);
        SDL_ExitError("Erreur de création de la texture");
    }

    SDL_Rect rectangle;
    rectangle.h = 257;
    rectangle.w = 261;

    if(SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h)){
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(window);
        SDL_ExitError("Erreur de chargement de la texture");
    }

    rectangle.x = (261 - rectangle.w) / 2;
    rectangle.y = (257 - rectangle.h) / 2;

    if(SDL_RenderCopy(rendu, texture, NULL, &rectangle)){
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(window);
        SDL_ExitError("Erreur d'afficher la texture");        
    }

    SDL_RenderPresent(rendu);

    SDL_Delay(5000);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rendu);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void SDL_ExitError(const char *message){
    SDL_Log("%s > %s\n",message,SDL_GetError());
    exit(EXIT_FAILURE);
    SDL_Quit();
}