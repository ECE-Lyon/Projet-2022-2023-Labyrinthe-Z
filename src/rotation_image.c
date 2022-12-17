#include <SDL.h>
#include <math.h>
#include <stdio.h>

#define ANGLE 180

int main(int argc, char *argv[])
{
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Erreur lors de l'initialisation de SDL : %s", SDL_GetError());
        return 1;
    }

    // Chargement de l'image
    SDL_Surface *image = SDL_LoadBMP("images/labyrinthe-minecraft.bmp");
    if (image == NULL)
    {
        printf("Erreur lors du chargement de l'image : %s", SDL_GetError());
        return 1;
    }

    float angle = ANGLE*M_PI/180;
    // Calcul des dimensions de l'image redimensionnée
    int w = image->w;
    int h = image->h;
    int new_w = (int)(w * cos(angle) + h * sin(angle));
    int new_h = (int)(w * sin(angle) + h * cos(angle));

    // Création de l'image redimensionnée
    SDL_Surface *image_rot = SDL_CreateRGBSurface(0, new_w, new_h, 32, 0, 0, 0, 0);

    // Rotation de l'image
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int new_x = (int)(x * cos(angle) + y * sin(angle));
            int new_y = (int)(-x * sin(angle) + y * cos(angle))+new_h;
            SDL_Rect src = { x, y, 1, 1 };
            SDL_Rect dst = { new_x, new_y, 1, 1 };
            SDL_BlitSurface(image, &src, image_rot, &dst);
        }
    }

    // Enregistrement de l'image redimensionnée
    SDL_SaveBMP(image_rot, "image_rot.bmp");

    // Libération de la mémoire
    SDL_FreeSurface(image);
    SDL_FreeSurface(image_rot);

    // Fermeture de SDL
    SDL_Quit();

    return 0;
}
