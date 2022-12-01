#include <stdio.h>
#include <SDL.h>

// gcc src/testsdl.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2

void SDL_ExitError(const char *message);

int main(int argc, char **argv)
{
    
    SDL_version nb;
    SDL_VERSION(&nb);

    printf("\nBienvenue sur la version SDL %d.%d.%d !\n", nb.major,nb.minor,nb.patch);

    SDL_Window *window;
    SDL_Renderer *rendu;

    if( SDL_Init(SDL_INIT_VIDEO) != 0 )
        SDL_ExitError("Erreur initialisation video");

    if(SDL_CreateWindowAndRenderer(1920,1080, SDL_RENDERER_ACCELERATED, &window, &rendu) != 0) // CREE UNE FENETRE ET UN RENDU
        SDL_ExitError("Erreur de fenetre et rendu");

    if(SDL_SetRenderDrawColor(rendu,82,192,213, SDL_ALPHA_OPAQUE) != 0) // SET COULEUR POUR DESSINER
        SDL_ExitError("Erreur drawcolor pour rendu");

    if(SDL_RenderDrawPoint(rendu, 960,540) != 0) // DESSINE UN POINT EN COORD 940/540 (au centre)
        SDL_ExitError("Erreur dessin point");

    if(SDL_RenderDrawLine(rendu,0,0,1920,1080) != 0) // DESSINE UNE LIGNE DEPUIS 0/0 à 1920/1080
        SDL_ExitError("Erreur dessin ligne");

    SDL_Rect rectangle;
    rectangle.x = 300;
    rectangle.y = 300;
    rectangle.h = 120;
    rectangle.w = 200;

    if(SDL_RenderDrawRect(rendu,&rectangle) != 0)
        SDL_ExitError("Erreur dessin rectangle");

    SDL_RenderPresent(rendu); // ACTIVE LE RENDU

    SDL_Delay(5000);

    SDL_DestroyRenderer( rendu );
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 0;
}

void SDL_ExitError(const char *message){
    SDL_Log("Erreur d'initialisation SDL > %s\n",SDL_GetError());
    exit(EXIT_FAILURE);
    SDL_Quit();
}