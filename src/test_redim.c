#include <stdio.h>
#include <SDL.h>
#include "bmp_manip.h"

int main(int argc, char **argv)
{
  
  SDL_Surface *image = SDL_LoadBMP("images/default/skin/player_1.bmp");
  handleSurfaceError(image);

  SDL_Surface *image_redim = SDL_CreateRGBSurface(0, 128, 128, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  handleSurfaceError(image);

  redimImage(image, image_redim);

  SDL_SaveBMP(image_redim, "images/formated/skin/skin_1.bmp");

  return 0;
}
