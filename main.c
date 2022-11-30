#pragma execution_character_set( "utf-8" )

#include <Windows.h>
#include <stdio.h>

#define NB_CHARAC 10

void rotationTuile(int tuile[3][3]);
void copyTab(int tab1[][], int tab2[][], size_t sx, size_t sy);

char caractere[NB_CHARAC][10] = {"▓▓","  ","🏂","⛄","ꎊ","é",}; //liste de tous les caractères dont on a besoin

int tuileL[3][3] = {0,1,0,
                    1,1,0,
                    0,0,0};

int tuileT[3][3] = {0,1,0,
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

  int len = sizeof(tuileT)/sizeof(tuileT[0]);
  //printf("%d ", len);

  rotationTuile(tuileT);

  for(int i = 0; i <3; i++){
    for(int j = 0; j<3; j++){
      printf("%s", caractere[tuileT[i][j]]);
    }
    printf("\n");
  }

  int tuileActuelle[3][3] = {0};

  copyTab(tuileT, tuileActuelle, 3, 3);

  for(int i = 0; i <3; i++){
    for(int j = 0; j<3; j++){
      printf("%s", caractere[tuileActuelle[i][j]]);
    }
    printf("\n");
  }

  return 0;
}

void rotationTuile(int tuileActuelle[3][3]){
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
    mem = tuileActuelle[ordre[1][0]][ordre[1][1]]; // mem = b
    tuileActuelle[ordre[1][0]][ordre[1][1]] = tuileActuelle[ordre[0][0]][ordre[0][1]]; // b = a 
    for(int i = 2; i < 9; i++){
      mem2 = tuileActuelle[ordre[i][0]][ordre[i][1]];
      tuileActuelle[ordre[i][0]][ordre[i][1]] = mem;
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

void copyTab(int tab1[][], int tab2[][], size_t sx, size_t sy){
  memcpy(tab1,tab2,sizeof(tab1[0])*sx*sy);
}
