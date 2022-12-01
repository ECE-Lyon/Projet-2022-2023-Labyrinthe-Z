#pragma execution_character_set( "utf-8" )

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_CHARAC 100

typedef struct{ 
  char forme, rotation, item; // on utilise des char pour économiser la mémoire (1 char = 1 octet alors que 1 int = 4 octets)
}casePlateau; // definition d'une structure qui stocke toutes les infos d'une tuile

//déclaration de tous les prototypes
void rotationTuile(int tuile[9], int nbTour);
void copyTab(int tab1[], int tab2[], int size);
void printPlateau(casePlateau plateau[7][7]);
void printTuile(int typeTuile[9], int rotation, int item, int indiceLigne);
void genererPlateau();
int getRandomInt(int min, int max);

//liste de tous les caractères dont on a besoin
char caractere[NB_CHARAC][10] = {"  ","▓▓","░░","░░",//3 // espace vide // mur // séparateur de tuile horizontal // séparateur de tuile vertical
                                 "🟥","🟩","🟦","🟨",//7
                                 "🔴","🟢","🔵","🟡",//11
                                 "🎈","🧨","✨","🎫",//15
                                 "🎁","🎨","🧶","🍩",//19
                                 "🍗","👑","💎","🏀",//23 // de 12 à 23 pour les tuiles fixes
                                 "🎯","🥇","🧩","🧸",//27
                                 "🍬","🍕","🎵","🎸",//31
                                 "💊","🔨","🌻","🍖"};//35 // de 24 à 35 pour les tuiles mobiles

//déclaration des tuiles et de leur composition
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

casePlateau plateau[7][7] = {1,2,6,   0,0,0,  2,3,12,  0,0,0,   2,3,13,  0,0,0,  1,3,5,
                             0,0,0,   0,0,0,  0,0,0,   0,0,0,   0,0,0,   0,0,0,  0,0,0,
                             2,2,14,  0,0,0,  2,2,15,  0,0,0,   2,3,16,  0,0,0,  2,0,17,
                             0,0,0,   0,0,0,  0,0,0,   0,0,0,   0,0,0,   0,0,0,  0,0,0,
                             2,2,18,  0,0,0,  2,1,19,  0,0,0,   2,0,20,  0,0,0,  2,0,21,
                             0,0,0,   0,0,0,  0,0,0,   0,0,0,   0,0,0,   0,0,0,  0,0,0,
                             1,1,7,   0,0,0,  2,1,22,  0,0,0,   2,1,23,  0,0,0,  1,0,4,};

int main(){
  SetConsoleOutputCP(65001); // format de la console pour afficher l'unicode

  genererPlateau();
  printPlateau(plateau);

  return 0;
}

void rotationTuile(int tuileActuelle[9], int nbTour){
  int ordre[9] = {0, //a   |a|b|c|   b va devenir a, puis c va devenir b et ainsi de suite
                  1, //b   |h|°|d|   ° ne change jamais car il est au centre
                  2, //c   |g|f|e|   ce tableau sert a savoir quelle case déplacer en fonction de sa position
                  5, //d
                  8, //e
                  7, //f
                  6, //g
                  3, //h
                  0};//a
  int mem;
  int mem2;
  for(int i = 0; i< nbTour; i++){ //pour faire tourner de n tours
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
  for(int i = 0; i<7*4+1; i++){ // affiche le contour de la première ligne
    printf("%s", caractere[2]);
  }
  printf("\n");
  for(int i = 0; i<7; i++){ // affiche les 7 lignes de tuiles
    for(int k = 0; k < 9; k+=3){ // permet d'afficher les lignes 2 et 3 en prenant l'indice de la tuile +3k
    printf("%s", caractere[2]);
      for(int j = 0; j<7; j++){ // parcourt les 7 tuiles de la ligne
        switch(plateau[i][j].forme){ // regarde quelle tuile prendre parmis les 4
          case 0: // tuile Vide
            printTuile(tuileVide, plateau[i][j].rotation, plateau[i][j].item, k);
            break;
          case 1: // tuile L
            printTuile(tuileL, plateau[i][j].rotation, plateau[i][j].item, k);
            break;
          case 2: // tuile T
            printTuile(tuileT, plateau[i][j].rotation, plateau[i][j].item, k);
            break;
          case 3: // tuile I
            printTuile(tuileI, plateau[i][j].rotation, plateau[i][j].item, k);
            break;
        }
      }
      printf("\n");
    }
    for(int i = 0; i<7*4+1; i++){ // affiche un séparateur entre chaque ligne
      printf("%s", caractere[2]);
    }
    printf("\n");
  }
}

void printTuile(int typeTuile[9], int rotation, int item, int indiceLigne){
  int tuileActuelle[9];
  copyTab(typeTuile, tuileActuelle, 9); // on copie le modèle de la tuile pour ne pas tourner l'orgiginal
  rotationTuile(tuileActuelle, rotation); // rotaion de la tuile de 0°/90°/180° ou 270° selon "rotation"
  for(int l = 0; l < 3; l++){
    if(l+indiceLigne == 4){ // permet d'afficher l'item au centre
      printf("%s", caractere[item]);
    }else{ // affiche le reste des cases selon le modèle
      printf("%s", caractere[tuileActuelle[l+indiceLigne]]);
    }
  }
  printf("%s", caractere[3]); // affiche un séparateur entre chaque tuile
}

void copyTab(int tab1[], int tab2[], int size){
  for (int i = 0; i < size; ++i)
  {
    tab2[i] = tab1[i];
  }
}

void genererPlateau(){
  srand(time(NULL));
  char caseARemplir[7][7] = {0,1,0,1,0,1,0,
                             1,1,1,1,1,1,1,
                             0,1,0,1,0,1,0,
                             1,1,1,1,1,1,1,
                             0,1,0,1,0,1,0,
                             1,1,1,1,1,1,1,
                             0,1,0,1,0,1,0}; // défini les cases vides qu'il faut donc remplacer par une tuile aléatoire

  char nbTuileRestant[4] = {6,6,10,12}; // 6 tuiles T avec trésor // 6 tuiles L avec trésor // 10 tuiles L vides // 12 tuiles I vides

  int indice = 24;
  for(int i = 0; i< 7; i++){
    for(int j = 0; j< 7; j++){
      if(caseARemplir[i][j]){
        while(1){
          int rand = getRandomInt(0, 3);
          //printf("%d", rand);
          if(nbTuileRestant[rand] > 0){ // ne marche pas si la valeur est a 0
            switch (rand){
              case 0:
                plateau[i][j].forme = 2; // tuile T
                plateau[i][j].item = indice; // on donne un item
                plateau[i][j].rotation = getRandomInt(0, 3);
                nbTuileRestant[0] -= 1;
                indice++;
                break;
              case 1:
                plateau[i][j].forme = 1; // tuile L
                plateau[i][j].item = indice; // on donne un item
                plateau[i][j].rotation = getRandomInt(0, 3);
                nbTuileRestant[1] -= 1;
                indice++;
                break;
              case 2:
                plateau[i][j].forme = 2; // tuile L
                plateau[i][j].item = 0; // on ne donne pas d'item
                plateau[i][j].rotation = getRandomInt(0, 3);
                nbTuileRestant[2] -= 1;
                break;
              case 3:
                plateau[i][j].forme = 3; // tuile I
                plateau[i][j].item = 0; // on ne donne pas d'item
                plateau[i][j].rotation = getRandomInt(0, 3);
                nbTuileRestant[3] -= 1;
                break;
            }
          break;
          }
        }
        
      }
    }
  }
}

int getRandomInt(int min, int max){
  
  int i = (rand() % (max-min+1)) + min;
  return i;
}
