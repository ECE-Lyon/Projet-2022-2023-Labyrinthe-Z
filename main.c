#pragma execution_character_set( "utf-8" )

#include <Windows.h>
#include <stdio.h>

#define NB_CHARAC 100

typedef struct{ // la structure de base n'a pas de nom, uniquement un alias
  int forme, rotation, item;
}casePlateau;

void rotationTuile(int tuile[9], int nbTour);
void copyArray(int arr[], int copy[], int size);
void printPlateau(casePlateau plateau[7][7]);

char caractere[NB_CHARAC][10] = {"  ","▓▓","  ","  ",//3
                                 "🟥","🟩","🟦","🟨",//7
                                 "🔴","🟢","🔵","🟡",//11
                                 "🎈","🧨","✨","🎫",//15
                                 "🎁","🎨","🧶","🍩",//19
                                 "🍗","👑","💎","🏀",//23
                                 "🎯","🥇","🧩","🧸",//27
                                 "🪅","🍕","🎵","🎸",//31
                                 "🪛","🔨","🪨","🪵"};//35   //liste de tous les caractères dont on a besoin

int tuileVide[9] = {0,0,0,
                    0,0,0,
                    0,0,0};

int tuileL[9] = {1,0,1,
                 0,0,1,
                 1,1,1};

int tuileT[9] = {1,0,1,
                 0,0,1,
                 1,0,1};

int tuileI[9] = {1,0,1,
                 1,0,1,
                 1,0,1};



int main(){
  SetConsoleOutputCP(65001); // format de la console pour afficher l'unicode

  casePlateau plateau[7][7] = {1,2,6,   0,0,0,  2,3,12,  0,0,0,   2,3,13,  0,0,0,  1,3,5,
                               0,0,0,   0,0,0,  0,0,0,   0,0,0,   0,0,0,   0,0,0,  0,0,0,
                               2,2,14,  0,0,0,  2,2,15,  0,0,0,   2,3,16,  0,0,0,  2,0,17,
                               0,0,0,   0,0,0,  0,0,0,   0,0,0,   0,0,0,   0,0,0,  0,0,0,
                               2,2,18,  0,0,0,  2,1,19,  0,0,0,   2,0,20,  0,0,0,  2,0,21,
                               0,0,0,   0,0,0,  0,0,0,   0,0,0,   0,0,0,   0,0,0,  0,0,0,
                               1,1,7,   0,0,0,  2,1,22,  0,0,0,   2,1,23,  0,0,0,  1,0,4,};

  int tuileActuelle[9] = {0};
  copyArray(tuileT, tuileActuelle, 9);

  rotationTuile(tuileActuelle, 0);

  printPlateau(plateau);

  return 0;
}

void rotationTuile(int tuileActuelle[9], int nbTour){
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
  for(int i = 0; i< nbTour; i++){
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
}

void printPlateau(casePlateau plateau[7][7]){
  int tuileActuelle[9];
  for(int i = 0; i<7; i++){
    for(int k = 0; k < 9; k+=3){
      for(int j = 0; j<7; j++){ 
        switch(plateau[i][j].forme){
          case 0: // tuile Vide
            copyArray(tuileVide, tuileActuelle, 9);
            rotationTuile(tuileActuelle, plateau[i][j].rotation);
            for(int l = 0; l < 3; l++){
              if(l+k == 4){
                printf("%s", caractere[plateau[i][j].item]);
              }else{
                printf("%s", caractere[tuileActuelle[l+k]]);
              }
            }
            printf("%s", caractere[3]);
            break;
          case 1: // tuile L
            copyArray(tuileL, tuileActuelle, 9);
            rotationTuile(tuileActuelle, plateau[i][j].rotation);
            for(int l = 0; l < 3; l++){
              if(l+k == 4){
                printf("%s", caractere[plateau[i][j].item]);
              }else{
                printf("%s", caractere[tuileActuelle[l+k]]);
              }
            }
            printf("%s", caractere[3]);
            break;
          case 2: // tuile T
            copyArray(tuileT, tuileActuelle, 9);
            rotationTuile(tuileActuelle, plateau[i][j].rotation);
            for(int l = 0; l < 3; l++){
              if(l+k == 4){
                printf("%s", caractere[plateau[i][j].item]);
              }else{
                printf("%s", caractere[tuileActuelle[l+k]]);
              }
            }
            printf("%s", caractere[3]);
            break;
          case 3: // tuile I
            copyArray(tuileI, tuileActuelle, 9);
            rotationTuile(tuileActuelle, plateau[i][j].rotation);
            for(int l = 0; l < 3; l++){
              if(l+k == 4){
                printf("%s", caractere[plateau[i][j].item]);
              }else{
                printf("%s", caractere[tuileActuelle[l+k]]);
              }
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
