#ifndef bmp_manip_
#define bmp_manip_

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int numerateur;
    int denominateur;
} fraction;

SDL_Surface* upscaleImage(SDL_Surface *image, int facteurGrossissement);
SDL_Surface* downscaleImage(SDL_Surface *image, int facteurScaling);
SDL_Surface* redimImage(SDL_Surface *image, float facteur);
fraction floatToFraction(float n);
int pgcd(int a, int b);

SDL_Surface* redimImage(SDL_Surface *image, float facteur){
  fraction ratio = floatToFraction(facteur);
  SDL_Surface *image_up = upscaleImage(image, ratio.numerateur);
  SDL_Surface *image_down = downscaleImage(image_up, ratio.denominateur);
  
  return image_down;
}

SDL_Surface* upscaleImage(SDL_Surface *image, int facteurScaling){

  int new_w = image->w * facteurScaling;
  int new_h = image->h * facteurScaling;

  SDL_Surface *image_redim = SDL_CreateRGBSurface(0, new_w, new_h, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  if (image_redim == NULL) {
    printf("Erreur lors de la création de la surface redimensionnée 1 : %s\n", SDL_GetError());
    
    SDL_FreeSurface(image);
    SDL_Quit();
  }

  if(facteurScaling == 1) return image_redim;

  for(int x = 0; x < image->w; x++){
    for(int s1 = 0; s1 < facteurScaling; s1++){
      for(int y = 0; y < image->h; y ++){
        for(int s2 = 0; s2 < facteurScaling; s2++){
          SDL_Rect src = { x, y, 1, 1 };
          SDL_Rect dst = { facteurScaling*x+s1, facteurScaling*y+s2, 1, 1 };
          SDL_BlitSurface(image, &src, image_redim, &dst);
        }
      }
    }
  }

  return image_redim;
}

SDL_Surface* downscaleImage(SDL_Surface *image, int facteurScaling){

  int new_w = image->w / facteurScaling;
  int new_h = image->h / facteurScaling;

  SDL_Surface *image_redim = SDL_CreateRGBSurface(0, new_w, new_h, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  if (image_redim == NULL) {
    printf("Erreur lors de la création de la surface redimensionnée 2 : %s\n", SDL_GetError());
    SDL_FreeSurface(image);
    SDL_Quit();
  }
  SDL_SetSurfaceBlendMode(image_redim, SDL_BLENDMODE_BLEND);

  if(facteurScaling == 1) return image_redim;

  for(int x = 0; x < image_redim->w; x++){
      for(int y = 0; y < image_redim->h; y ++){
          SDL_Rect src = { facteurScaling*x, facteurScaling*y, 1, 1 };
          SDL_Rect dst = { x, y, 1, 1 };
          SDL_BlitSurface(image, &src, image_redim, &dst);
      }
  }

  return image_redim;
}

fraction floatToFraction(float n) {
    fraction f;

    // Trouver l'entier le plus proche de n
    int entier = (int) n;

    // Calculer le dénominateur en utilisant le fait que n = entier + décimal
    f.denominateur = 1;
    float decimal = n - entier;
    for(int i = 0; i < 4; i++) {
        decimal *= 10;
        f.denominateur *= 10;
        entier = (int) decimal;
        if(decimal == entier) break;
    }

    // Calculer le numérateur en utilisant le fait que n = numérateur / dénominateur
    f.numerateur = n * f.denominateur;

    //simplifier la fraction avec l'algorithme d'Euclide
    int pgcd_value = pgcd(f.denominateur, f.numerateur);
    f.denominateur /= pgcd_value;
    f.numerateur /= pgcd_value;


    return f;
}

int pgcd(int a, int b){
  if (b == 0) return a;
  else return pgcd(b, a % b);
}

#endif