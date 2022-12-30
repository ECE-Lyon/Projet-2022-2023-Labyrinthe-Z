#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "bmp_manip.h"

int tuileFormat[10][9]={1,1,1,
						1,0,0,
						1,0,1,
						
						1,0,1,
						1,0,0,
						1,1,1,
						
						1,0,1,
						0,0,1,
						1,1,1,
						
						1,1,1,
						0,0,1,
						1,0,1,
						
						1,0,1,
						0,0,0,
						1,1,1,
						
						1,0,1,
						0,0,1,
						1,0,1,
						
						1,1,1,
						0,0,0,
						1,0,1,
						
						1,0,1,
						1,0,0,
						1,0,1,
						
						1,0,1,
						1,0,1,
						1,0,1,
						
						1,1,1,
						0,0,0,
						1,1,1};

int getRandomInt(int min, int max){
  int i = (rand() % (max-min+1)) + min;
  return i;
}

int main(int argc, char **argv)
{
  	if(SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Erreur init > %s\n",SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Surface *surface_wall = NULL;
    surface_wall = SDL_LoadBMP("images/default/Tiles_Raw.bmp");

	SDL_Surface *surface_grass = NULL;
    surface_grass = SDL_LoadBMP("images/default/Grass_Raw.bmp");

	srand(time(NULL));

	for(int indiceTuile = 0; indiceTuile < 10; indiceTuile++){
		SDL_Surface *tuile = SDL_CreateRGBSurface(0, 54, 54, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
		for(int i = 0; i < 9; i++){
			SDL_Rect src1 = {getRandomInt(0,4)*18, getRandomInt(0,5)*18,18,18};
			SDL_Rect src2 = {getRandomInt(0,8)*18, getRandomInt(0,8)*18,18,18};
			if(i==4) {src2.x = 0; src2.y = 0;}
			SDL_Rect dst = {i%3*18,i/3*18,18,18};

			 

			if(tuileFormat[indiceTuile][i]) SDL_BlitSurface(surface_wall, &src1, tuile, &dst);
			else SDL_BlitSurface(surface_grass, &src2, tuile, &dst);
		}
		char cheminItem[100];
		sprintf(cheminItem, "images/default/tuiles/Tuile%d.bmp", indiceTuile+1);
		SDL_SaveBMP(tuile, cheminItem);
		SDL_FreeSurface(tuile);
	}
	SDL_FreeSurface(surface_wall);

  	return 0;
}

