#include <stdio.h>
#include <SDL.h>

// gcc src/testsdl.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2

void SDL_ExitError(const char *message);
void SDL_ExitErrorRenderer(SDL_Renderer *renderer);
void SDL_ExitErrorWindow(SDL_Window *window);
void SDL_ExitErrorBoth(const char *message, SDL_Renderer *renderer, SDL_Window *window);

int main(int argc, char **argv)
{

    SDL_Window *window = NULL;
    SDL_Renderer *background;
    SDL_Renderer *rendu;

    if(SDL_Init(SDL_INIT_VIDEO))
        SDL_ExitError("Erreur initialisation video");

    window = SDL_CreateWindow("Labyrinthe-Z", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1980, 1080, SDL_WINDOW_MAXIMIZED);

    if( window == NULL )
        SDL_ExitErrorWindow(window);

    if(SDL_CreateRenderer(window, -1, 0))
        SDL_ExitErrorRenderer(background);
    
    if(SDL_SetRenderDrawColor(background, 164, 122, 17, SDL_ALPHA_OPAQUE))
        SDL_ExitError("Erreur de couleur backgroud");

    if(SDL_CreateRenderer(window, -1, 0))
        SDL_ExitErrorRenderer(rendu);

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

    SDL_bool launched = SDL_TRUE;

    while ( launched ){   

        SDL_Event event;
        SDL_bool background_test = SDL_FALSE; 

        while ( SDL_PollEvent(&event) ){

            switch ( event.type ){
            case SDL_KEYDOWN:

                switch ( event.key.keysym.sym ){
                case SDLK_ESCAPE:                 // Appuie sur Echap
                    launched = SDL_FALSE;    
                    break;
                case SDLK_0:
                    background_test = SDL_TRUE;
                    continue;
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
                    SDL_ExitErrorBoth("Erreur de création de la texture", rendu , window);     
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

                if( background_test == SDL_TRUE ){
                    SDL_RenderPresent(background);
                }else{background_test = SDL_TRUE;}

                SDL_RenderPresent(rendu);
                break;
            }

        }

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
    SDL_ExitError("Erreur d'afficher la texture");       
}

void SDL_ExitErrorWindow(SDL_Window *window){
    SDL_DestroyWindow(window);
    SDL_ExitError("Erreur de fenetre");       
}

void SDL_ExitErrorRenderer(SDL_Renderer *renderer){
    SDL_DestroyRenderer(renderer);
    SDL_ExitError("Erreur de rendu");       
}
