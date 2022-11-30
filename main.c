#pragma execution_character_set( "utf-8" )

#include <Windows.h>
#include <stdio.h>

#define NB_CHARAC 10

char caractere[NB_CHARAC][10] = {"▓▓","  ","🏂","⛄","ꎊ","é",}; //liste de tous les caractères dont on a besoin

void rotationTuile(int tuile[3][3]);

const int tuileL[3][3] = {0,1,0,
                          1,1,0,
                          0,0,0};

const int tuileT[3][3] = {0,1,0,
                          1,3,0,
                          0,1,0};

int main(){
  SetConsoleOutputCP(65001); // format de la console pour afficher l'unicode

  int plateau[12][12] = {};

  for(int i = 0; i <3; i++){
    for(int j = 0; j<3; j++){
      printf("%s", caractere[tuileT[i][j]]); //affiche le deuxième caractère de la liste (bonhome de neige)
    }
    printf("\n");
  }

  rotationTuile(tuileT);

  for(int i = 0; i <3; i++){
    for(int j = 0; j<3; j++){
      printf("%s", caractere[tuileT[i][j]]); //affiche le deuxième caractère de la liste (bonhome de neige)
    }
    printf("\n");
  }

  return 0;
}

void rotationTuile(int tuile[3][3]){
  int ordre[9][2] = {0,0, //a   |a|b|c|
                     0,1, //b   |h|°|d|
                     0,2, //c   |g|f|e|
                     1,2, //d
                     2,2, //e
                     2,1, //f
                     2,0, //g
                     1,0, //h
                     0,0};//a
  int mem;
  int mem2;
  for(int i = 0; i<2; i++){ // décale de une case deux fois pour faire une rotation de 90°
    mem = tuile[ordre[1][0]][ordre[1][1]]; // mem = b
    tuile[ordre[1][0]][ordre[1][1]] = tuile[ordre[0][0]][ordre[0][1]]; // b = a 
    for(int i = 2; i < 9; i++){
      mem2 = tuile[ordre[i][0]][ordre[i][1]];
      tuile[ordre[i][0]][ordre[i][1]] = mem;
      mem = mem2;
    }
  }  
}

void printPlateau(int plateau[12][12]){
  for(int i = 0; i<12; i++){
    for(int j = 0; j<12; j++){
      for(int k = 0; k < 3; k++){
        switch(plateau[i][j]){
          case 0:
            for(int l = 0; l < 3; l++){
              printf("%s", caractere[tuileT[i][j]]);
            }
          break;
        }
      }
    }
  }
}