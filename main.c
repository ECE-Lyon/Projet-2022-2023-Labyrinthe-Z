#pragma execution_character_set( "utf-8" )

#include <Windows.h>
#include <stdio.h>

#define NB_CHARAC 10

void rotationTuile(int tuile[9]);
void copyArray(int arr[], int copy[], int size);

char caractere[NB_CHARAC][10] = {"▓▓","  ","  ","  ","🏂","⛄","ꎊ","é",}; //liste de tous les caractères dont on a besoin

int tuileL[9] = {0,1,0,
                 1,5,0,
                 0,0,0};

int tuileT[9] = {0,1,0,
                 1,5,0,
                 0,1,0};

int tuileI[9] = {0,1,0,
                 0,5,0,
                 0,1,0};

int tuileVide[9] = {1,1,1,
                    1,1,1,
                    1,1,1};

int main(){
  SetConsoleOutputCP(65001); // format de la console pour afficher l'unicode

  int plateau[12][12] = {0,0,0,0,0,0,0,0,0,0,0,0,
                         1,1,1,1,1,1,1,1,1,1,1,1,
                         2,2,2,2,2,2,2,2,2,2,2,2,
                         3,3,3,3,3,3,3,3,3,3,3,3,
                         0,0,0,0,0,0,0,0,0,0,0,0,
                         1,1,1,1,1,1,1,1,1,1,1,1,
                         2,2,2,2,2,2,2,2,2,2,2,2,
                         3,3,3,3,3,3,3,3,3,3,3,3,
                         0,0,0,0,0,0,0,0,0,0,0,0,
                         1,1,1,1,1,1,1,1,1,1,1,1,
                         2,2,2,2,2,2,2,2,2,2,2,2,
                         3,3,3,3,3,3,3,3,3,3,3,3};

  int tuileActuelle[9] = {0};
  copyArray(tuileT, tuileActuelle, 9);

  rotationTuile(tuileActuelle);

  printPlateau(plateau);

  return 0;
}

void rotationTuile(int tuileActuelle[9]){
  int ordre[9] = {0, //a   |a|b|c|
                  1, //b   |h|°|d|
                  2, //c   |g|f|e|
                  5, //d
                  8, //e
                  7, //f
                  6, //g
                  3, //h
                  0};//a
  int mem;
  int mem2;
  for(int i = 0; i<2; i++){ // décale de une case deux fois pour faire une rotation de 90°
    mem = tuileActuelle[ordre[1]]; // mem = b
    tuileActuelle[ordre[1]] = tuileActuelle[ordre[0]]; // b = a 
    for(int i = 2; i < 9; i++){
      mem2 = tuileActuelle[ordre[i]];
      tuileActuelle[ordre[i]] = mem;
      mem = mem2;
    }
  }  
}

void printPlateau(int plateau[12][12]){
  for(int i = 0; i<12; i++){
    for(int k = 0; k < 9; k+=3){
      for(int j = 0; j<12; j++){ 
        switch(plateau[i][j]){
          case 0: // tuile I
            for(int l = 0; l < 3; l++){
              printf("%s", caractere[tuileI[l+k]]);
            }
            printf("%s", caractere[3]);
            break;
          case 1: // tuile T
            for(int l = 0; l < 3; l++){
              printf("%s", caractere[tuileT[l+k]]);
            }
            printf("%s", caractere[3]);
            break;
          case 2: // tuile L
            for(int l = 0; l < 3; l++){
              printf("%s", caractere[tuileL[l+k]]);
            }
            printf("%s", caractere[3]);
            break;
          case 3: // tuile vide
            for(int l = 0; l < 3; l++){
              printf("%s", caractere[tuileVide[l+k]]);
            }
            printf("%s", caractere[3]);
            break;
        }
      }
      printf("\n");
    }
    for(int i = 0; i<12*3; i++){
        printf("%s", caractere[2]);
      }
      printf("\n");
  }
}

void copyArray(int arr[], int copy[], int size)
{
  // loop to iterate through array
  for (int i = 0; i < size; ++i)
  {
    copy[i] = arr[i];
  }
}
