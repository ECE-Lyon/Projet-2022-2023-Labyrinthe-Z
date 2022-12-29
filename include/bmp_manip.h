#ifndef bmp_manip_
#define bmp_manip_

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int numerateur;
    int denominateur;
} fraction;

void upscaleImage(SDL_Surface *image, SDL_Surface *image_redim, int facteurX, int facteurY);
void downscaleImage(SDL_Surface *image, SDL_Surface *image_redim, int facteurX, int facteurY);
void redimImage(SDL_Surface *image, SDL_Surface *image_redim);
fraction floatToFraction(float n);
int pgcd(int a, int b);

void handleSurfaceError(SDL_Surface* surface){
	if (surface == NULL) {
		printf("Erreur lors de la création de la Surface : %s\n", SDL_GetError());
		
		SDL_FreeSurface(surface);
		SDL_Quit();
  	}
}

void handleTextureError(SDL_Texture* texture){
	if (texture == NULL) {
		printf("Erreur lors de la création de la Texture: %s\n", SDL_GetError());
		
		SDL_DestroyTexture(texture);
		SDL_Quit();
  	}
}

void printImage(SDL_Renderer *renderer, SDL_Rect rect_image, const char *chemin_image){

    SDL_Surface *surface_image = NULL;
    SDL_Texture *texture_image = NULL;

    surface_image = SDL_LoadBMP(chemin_image);
	//handleSurfaceError(surface_image);

    texture_image = SDL_CreateTextureFromSurface(renderer, surface_image);
	//handleTextureError(texture_image);

    SDL_FreeSurface(surface_image);
    SDL_QueryTexture(texture_image, NULL, NULL, &rect_image.w, &rect_image.h);
    SDL_RenderCopy(renderer, texture_image, NULL, &rect_image);

    SDL_DestroyTexture(texture_image);
}

void printImageFromSurface(SDL_Renderer *renderer, SDL_Surface *surface_image,SDL_Rect rect_image){
    SDL_Texture *texture_image = NULL;

    clock_t start_t, end_t;
    double total_t;

    start_t = clock();
    texture_image = SDL_CreateTextureFromSurface(renderer, surface_image);
    end_t = clock();

    SDL_RenderCopy(renderer, texture_image, NULL, &rect_image);

    SDL_DestroyTexture(texture_image);

    total_t = (double)(end_t - start_t) / (CLOCKS_PER_SEC/1000);
    printf("Texture from surface : %f ms, ", total_t  );
}

void printImageFromTexture(SDL_Renderer *renderer, SDL_Texture *texture_image,SDL_Rect rect_image){
    if(SDL_RenderCopy(renderer, texture_image, NULL, &rect_image)) printf("erreur d'affichage de texture");
}

void reduceFraction(fraction *f){

    //simplifier la fraction avec l'algorithme d'Euclide
    int pgcd_value = pgcd(f->denominateur, f->numerateur);
    f->denominateur /= pgcd_value;
    f->numerateur /= pgcd_value;

}

void redimImage(SDL_Surface *image, SDL_Surface *image_redim){

	fraction facteurX = {image_redim->w, image->w};
	fraction facteurY = {image_redim->h, image->h};
    reduceFraction(&facteurX);
    reduceFraction(&facteurY);

	SDL_Surface *image_up = SDL_CreateRGBSurface(0, image->w*facteurX.numerateur, image->h*facteurY.numerateur, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    handleSurfaceError(image_up);
	
	upscaleImage(image, image_up, facteurX.numerateur, facteurY.numerateur);
	downscaleImage(image_up, image_redim, facteurX.denominateur, facteurY.denominateur);
  
}

void upscaleImage(SDL_Surface *image, SDL_Surface *image_redim, int facteurX, int facteurY){

	if(facteurX == 1 && facteurY == 1) {
		SDL_Rect src = { 0, 0, image->w, image->h };
		SDL_BlitSurface(image, &src, image_redim, &src);
		return;
	}

	for(int x = 0; x < image->w; x++){
		for(int sX = 0; sX < facteurX; sX++){
			for(int y = 0; y < image->h; y ++){
				for(int sY = 0; sY < facteurY; sY++){
					SDL_Rect src = { x, y, 1, 1 };
					SDL_Rect dst = { facteurX*x+sX, facteurY*y+sY, 1, 1 };
					SDL_BlitSurface(image, &src, image_redim, &dst);
				}
			}
		}
	}
}

void downscaleImage(SDL_Surface *image, SDL_Surface *image_redim, int facteurX, int facteurY){

  if(facteurX == 1 && facteurY == 1){
	SDL_Rect src = { 0, 0, image->w, image->h };
	SDL_BlitSurface(image, &src, image_redim, &src);
	return;
  }

  for(int x = 0; x < image_redim->w; x++){
      for(int y = 0; y < image_redim->h; y ++){
          SDL_Rect src = { facteurX*x, facteurY*y, 1, 1 };
          SDL_Rect dst = { x, y, 1, 1 };
          SDL_BlitSurface(image, &src, image_redim, &dst);
      }
  }
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

void shiftImage(SDL_Surface *image, SDL_Surface *image_shifted, SDL_bool direction, int deplacement){

	SDL_Rect src = { deplacement, 0, image_shifted->w-deplacement, image_shifted->h };
	SDL_Rect dst = { 0, 0, image_shifted->w-deplacement, image_shifted->h };
	SDL_BlitSurface(image, &src, image_shifted, &dst);

	SDL_Rect src2 = { 0, 0, deplacement, image_shifted->h };
	SDL_Rect dst2 = { image_shifted->w-deplacement, 0, deplacement, image_shifted->h };
	SDL_BlitSurface(image, &src2, image_shifted, &dst2);
	
}

#endif