#include <stdio.h>
#include <SDL.h>

// gcc src/testsdl.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2

void SDL_ExitError(const char *message);
void SDL_ExitErrorRenderer(const char *message,SDL_Renderer *renderer);
void SDL_ExitErrorWindow(const char *message,SDL_Window *window);
void SDL_ExitErrorBoth(const char *message, SDL_Renderer *renderer, SDL_Window *window);

int main(int argc, char **argv)
{

    SDL_Window *window = NULL;
    SDL_Renderer *rendu = NULL;
    SDL_Surface *image = NULL;
    SDL_Texture *texture = NULL;

    if(SDL_Init(SDL_INIT_VIDEO))
        SDL_ExitError("Erreur initialisation video");

    window = SDL_CreateWindow("Labyrinthe-Z", 0, 0, 1980, 1080, SDL_WINDOW_MAXIMIZED);

    if( window == NULL )
        SDL_ExitErrorWindow("Erreur fenetre",window);

    rendu = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if( rendu == NULL )
        SDL_ExitErrorRenderer("Erreur rendu",rendu);
    
    SDL_SetRenderDrawColor(rendu, 255, 233, 210, 255);
    SDL_RenderClear(rendu);
    
    SDL_bool launched = SDL_TRUE;

    while ( launched ){   

        SDL_Event event;

        while ( SDL_PollEvent(&event) ){

            switch ( event.type ){
            case SDL_KEYDOWN:

                switch ( event.key.keysym.sym ){
                case SDLK_ESCAPE:                 // Appuie sur Echap
                    launched = SDL_FALSE;    
                    break;
                    
                default:
                    continue;
                }

            case SDL_MOUSEBUTTONDOWN:

                switch ( event.button.clicks ){
                case 2:                            // Double-click
                    launched = SDL_FALSE; 
                    break;
                
                default:
                    continue;
                }  

            case SDL_QUIT:
                launched = SDL_FALSE;
                break;
            
            default:
                image = SDL_LoadBMP("src/test.bmp");

                if( image == NULL ){
                    SDL_ExitErrorBoth("Erreur de chargement de l'image", rendu , window);  
                }

                texture = SDL_CreateTextureFromSurface(rendu, image);
                SDL_FreeSurface(image);

                if( texture == NULL ){
                    SDL_ExitErrorBoth("Erreur de creation de la texture", rendu , window);     
                }

                SDL_Rect rectangle;
                rectangle.h = 257;
                rectangle.w = 261;

                if(SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h)){
                    SDL_ExitErrorBoth("Erreur de chargement de la texture", rendu , window); 
                }

                rectangle.x = (1920 - rectangle.w) / 2;
                rectangle.y = (1080 - rectangle.h) / 2;

                if(SDL_RenderCopy(rendu, texture, NULL, &rectangle)){
                    SDL_ExitErrorBoth("Erreur d'affichage de l'image", rendu , window);       
                }

                SDL_RenderPresent(rendu);
                break;
            }

        }

    }

    return 0;
}

void SDL_ExitError(const char *message){
    SDL_Log("%s > %s\n",message,SDL_GetError());
    exit(EXIT_FAILURE);
    SDL_Quit();
}   

void SDL_ExitErrorBoth(const char *message, SDL_Renderer *renderer,SDL_Window *window){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("%s\t",message);
    SDL_ExitError("Erreur d'affichage de la texture");     
}

void SDL_ExitErrorWindow(const char *message,SDL_Window *window){
    SDL_DestroyWindow(window);
    SDL_ExitError("Erreur de fenetre");       
}

void SDL_ExitErrorRenderer(const char *message,SDL_Renderer *renderer){
    SDL_DestroyRenderer(renderer);
    SDL_ExitError("Erreur de rendu");       
}


    /*image = SDL_LoadBMP("src/test.bmp");

    if( image == NULL ){
        SDL_ExitErrorBoth("Erreur de chargement de l'image", rendu , window);  
    }

    texture = SDL_CreateTextureFromSurface(rendu, image);
    SDL_FreeSurface(image);

    if( texture == NULL ){
        SDL_ExitErrorBoth("Erreur de création de la texture", rendu , window);     
    }

    SDL_Rect rectangle;
    rectangle.h = 257;
    rectangle.w = 261;

    if(SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h)){
        SDL_ExitErrorBoth("Erreur de chargement de la texture", rendu , window); 
    }

    rectangle.x = (1920 - rectangle.w) / 2; // CENTRE L'IMAGE
    rectangle.y = (1080 - rectangle.h) / 2;

    if(SDL_RenderCopy(rendu, texture, NULL, &rectangle)){
        SDL_ExitErrorBoth("Erreur d'affichage de l'image", rendu , window);       
    }

    SDL_RenderPresent(rendu);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rendu);
    SDL_DestroyWindow(window);
    SDL_Quit();*/