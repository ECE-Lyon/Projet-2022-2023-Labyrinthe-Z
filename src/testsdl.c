#include <stdio.h>
#include <SDL.h>

// gcc src/testsdl.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2

int main(int argc, char **argv)
{
    
    SDL_version nb;
    SDL_VERSION(&nb);

    printf("\nBienvenue sur la version SDL %d.%d.%d !\n", nb.major,nb.minor,nb.patch);

    SDL_Window *window;

    if( SDL_Init(SDL_INIT_VIDEO) != 0 ){
        SDL_Log("Erreur d'initialisation SDL > %s\n",SDL_GetError());
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Labyrinte-Z", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_MAXIMIZED );

    if ( window == 0)
    {
        SDL_Log("Erreur d'ouverture de fenetre SDL > %s\n",SDL_GetError());
        exit(EXIT_FAILURE);        
    }
    
    SDL_Delay(5000);

    SDL_DestroyWindow ( window );
    SDL_Quit();
    return 0;
}
