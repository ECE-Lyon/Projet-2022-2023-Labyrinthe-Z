#pragma execution_character_set( "utf-8" )

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_CHARAC 100

typedef struct{ 
  char forme, rotation, item; // on utilise des char pour économiser la mémoire (1 char = 1 octet alors que 1 int = 4 octets)
}casePlateau; // definition d'une structure qui stocke toutes les infos d'une tuile

typedef struct{
  char posX, posY, itemFound;
}playerData;

//déclaration de tous les prototypes
void rotationTuile(int tuile[9], int nbTour);
void copyTab(int tab1[], int tab2[], int size);
void printPlateau(casePlateau plateau[7][7]);
void printTuile(int typeTuile[9], int rotation, int item, int indiceLigne, int posX, int posY);
void genererPlateau();
int getRandomInt(int min, int max);
void clrScreen(void);
void delay(int tps_sec);
void getTuileFormated(int tuileResultat[9], int posX, int posY);
int checkDeplacement(int joueur, int direction);
void pushTuile(int emplacement);
void afficherEspace(int nombre);
void getGUI(int nbLigne, char ligne[110]);
void melangerTab(int* tab, size_t tailleTab);
void printTab(int* tab, size_t tailleTab);
void genererCarte(int carte[24]);
void movePlayer(int player, int direction);
int askDeplacement(int joueur);
void printGUI(int indiceLigne);

//liste de tous les caractères dont on a besoin
char caractere[NB_CHARAC][10] = {"  ","▓▓","░░","░░",//3 // espace vide // mur // séparateur de tuile horizontal // séparateur de tuile vertical
                                 "🟥","🟩","🟦","🟨",//7
                                 "🔴","🟢","🔵","🟡",//11
                                 "🎈","🧨","✨","🎫",//15
                                 "🎁","🎨","🧶","🍩",//19
                                 "🍗","👑","💎","🏀",//23 // de 12 à 23 pour les tuiles fixes
                                 "🎯","🥇","🧩","🧸",//27
                                 "🍬","🍕","🎵","🎸",//31
                                 "💊","🔨","🌻","🍖",//35 // de 24 à 35 pour les tuiles mobiles
                                 "↓↓","↑↑","⇉ ","⇇ ",   //39
                                 "1.","2.","3."," 4. ",   //43
                                 " 5. "," 6. ","7.","8.",   //47
                                 "9.","10. ","11. ","12. "};  //51

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
                             1,1,7,   0,0,0,  2,1,22,  0,0,0,   2,1,23,  0,0,0,  1,0,4};

casePlateau tuileRestante;

playerData playerList[4] = {0,0,0,  0,6,0,  6,0,0,  6,6,0}; //position initiale des joueurs (0,0 etant le coin supérieur gauche, et 6,6 le coin inférieur droit)

int main(int argc, char **argv){
  SetConsoleOutputCP(65001); // format de la console pour affichser l'unicode

  genererPlateau();
  printPlateau(plateau);
  int carte[24];
  genererCarte(carte);
  askDeplacement(0);
  
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
  afficherEspace(3);
  for(int i = 0; i < 7; i++){
    if(i == 1 || i == 3 || i == 5){
      afficherEspace(2);
      printf("%s",caractere[40+i/2]);
      afficherEspace(1);
    }
    else{
      afficherEspace(4);
    }
  }
  afficherEspace(4);
  printGUI(0);
  printf("\n");
  afficherEspace(4);
  for(int i = 0; i < 7; i++){
    if(i == 1 || i == 3 || i == 5){
      afficherEspace(1);
      printf("%s",caractere[36]);
      afficherEspace(2);
    }
    else{
      afficherEspace(4);
    }
  }
  afficherEspace(3);
  printGUI(1);
  printf("\n");
  afficherEspace(3);
  for(int i = 0; i<7*4+1; i++){ // affiche le contour de la première ligne
    printf("%s", caractere[2]);
  }
  afficherEspace(3);
  printGUI(2);
  printf("\n");
  for(int i = 0; i<7; i++){ // affiche les 7 lignes de tuiles
    for(int k = 0; k < 9; k+=3){ // permet d'afficher les lignes 2 et 3 en prenant l'indice de la tuile +3k
    if(i == 1 || i == 3 || i == 5){
        if(k==3){
          int nombre = (i == 1) ? 11 : (i == 3) ? 10 : (i == 5) ? 9 : 0;
          printf("%s%s",caractere[40+nombre],caractere[38]);
        }
        else{
          afficherEspace(3);
        }
      }
      else {
        afficherEspace(3);
      }
    printf("%s",caractere[2]);
      for(int j = 0; j<7; j++){ // parcourt les 7 tuiles de la ligne
        switch(plateau[i][j].forme){ // regarde quelle tuile prendre parmis les 4
          case 0: // tuile Vide
            printTuile(tuileVide, plateau[i][j].rotation, plateau[i][j].item, k, i, j);
            break;
          case 1: // tuile L
            printTuile(tuileL, plateau[i][j].rotation, plateau[i][j].item, k, i, j);
            break;
          case 2: // tuile T
            printTuile(tuileT, plateau[i][j].rotation, plateau[i][j].item, k, i, j);
            break;
          case 3: // tuile I
            printTuile(tuileI, plateau[i][j].rotation, plateau[i][j].item, k, i, j);
            break;
        }
      }
      if(i == 1 || i == 3 || i == 5){
        if(k==3){
          int nombre = (i == 1) ? 3 : (i == 3) ? 4 : (i == 5) ? 5 : 0;
          printf("%s%s",caractere[39],caractere[40+nombre]);
        }
        else{
          afficherEspace(3);
        }
      }
      else {
        afficherEspace(3);
      }
      printGUI(k/3+4*i +3);
      printf("\n");
    }
    afficherEspace(3);
    for(int i = 0; i<7*4+1; i++){ // affiche un séparateur entre chaque ligne
      printf("%s", caractere[2]);
    }
    afficherEspace(3);
    printGUI(4*(i+1)+2);
    printf("\n");
  }
  afficherEspace(4);
  for(int i = 0; i < 7; i++){
    if(i == 1 || i == 3 || i == 5){
      afficherEspace(1);
      printf("%s",caractere[37]);
      afficherEspace(2);
    }
    else{
      afficherEspace(4);
    }
  }
  printf("\n");
  afficherEspace(4);
  int nombre;
  for(int i = 0; i < 7; i++){
    if(i == 1 || i == 3 || i == 5){
      nombre = (i == 1) ? 8 : (i == 3) ? 7 : (i == 5) ? 6 : 0; 
      afficherEspace(1);
      printf("%s",caractere[40+nombre]);
      afficherEspace(2);
    }
    else{
      afficherEspace(4);
    }
  }
  printf("\n");
}

void printTuile(int typeTuile[9], int rotation, int item, int indiceLigne, int posX, int posY){
  int tuileActuelle[9];
  int playerHere = 0;
  copyTab(typeTuile, tuileActuelle, 9); // on copie le modèle de la tuile pour ne pas tourner l'original
  rotationTuile(tuileActuelle, rotation); // rotaion de la tuile de 0°/90°/180° ou 270° selon "rotation"
  for(int l = 0; l < 3; l++){
    if(l+indiceLigne == 4){ // permet d'afficher l'item au centre
      for(int i = 0; i < 4; i++){
        if(playerList[i].posX == posX && playerList[i].posY == posY){
          printf("%s", caractere[i+8]);
          playerHere = 1;
        }
      }
      if (!playerHere){
        printf("%s", caractere[item]);
      }
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
  int i, j;
  for(int k = 0; k < 50; k++){
    i = k % 7;
    j = k / 7;
    if(k < 49){
      if(caseARemplir[i][j]){ // regarde dans le tableau si la tuile doit être générée
        while(1){
          int rand = getRandomInt(0, 3);
          //printf("%d", rand);
          if(nbTuileRestant[rand]){ // ne marche pas si la valeur est a 0
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
                plateau[i][j].forme = 1; // tuile L
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
          break; // break le while(1)
          }
        }
      }
    }
    else{
    while(1){
        int rand = getRandomInt(0, 3);
        if(nbTuileRestant[rand]){ // ne marche pas si la valeur est a 0
          switch (rand){
            case 0:
              tuileRestante.forme = 2; // tuile T
              tuileRestante.item = indice; // on donne un item
              tuileRestante.rotation = getRandomInt(0, 3);
              nbTuileRestant[0] -= 1;
              indice++;
              break;
            case 1:
              tuileRestante.forme = 1; // tuile L
              tuileRestante.item = indice; // on donne un item
              tuileRestante.rotation = getRandomInt(0, 3);
              nbTuileRestant[1] -= 1;
              indice++;
              break;
            case 2:
              tuileRestante.forme = 1; // tuile L
              tuileRestante.item = 0; // on ne donne pas d'item
              tuileRestante.rotation = getRandomInt(0, 3);
              nbTuileRestant[2] -= 1;
              break;
            case 3:
              tuileRestante.forme = 3; // tuile I
              tuileRestante.item = 0; // on ne donne pas d'item
              tuileRestante.rotation = getRandomInt(0, 3);
              nbTuileRestant[3] -= 1;
              break;
          }
        break; // break le while(1)
        }
      }
    }
  }
}

int getRandomInt(int min, int max){
  int i = (rand() % (max-min+1)) + min;
  return i;
}

void clrScreen(void){
  printf("\e[1;1H\e[2J");
}

void delay(int tps_ms){
  clock_t start_time = clock();
  while (clock() < start_time + tps_ms);
}

int checkDeplacement(int joueur, int direction){ // 0 = haut, 1 = droite, 2 = bas, 3 = gauche
  int tuileActuelle[9];
  getTuileFormated(tuileActuelle, playerList[joueur].posX, playerList[joueur].posY);

  int tuileSuivante[9];
  switch (direction)
  {
  case 0: // le joueur veut aller en haut
    if(!playerList[joueur].posY){// vérifie que le joueur n'est pas tout en haut du plateau
      getTuileFormated(tuileSuivante, playerList[joueur].posX, playerList[joueur].posY-1); // la tuile située au dessus
      if(tuileActuelle[1] == 0 && tuileSuivante[7] == 0){
        return 1;
      }else return 0;
    }
    else return 0;
    break;
  case 1: // le joueur veut aller a droite
    if(playerList[joueur].posX != 6){// vérifie que le joueur n'est pas tout à droite du plateau
      getTuileFormated(tuileSuivante, playerList[joueur].posX+1, playerList[joueur].posY); // la tuile située à droite
      if(tuileActuelle[5] == 0 && tuileSuivante[3] == 0){
        return 1;
      }else return 0;
    }
    else return 0;
    break;
  case 2: // le joueur veut aller en bas
    if(playerList[joueur].posY != 6){// vérifie que le joueur n'est pas tout en bas du plateau
      getTuileFormated(tuileSuivante, playerList[joueur].posX, playerList[joueur].posY+1); // la tuile située en dessous
      if(tuileActuelle[7] == 0 && tuileSuivante[1] == 0){
        return 1;
      }else return 0;
    }
    else return 0;
    break;
  case 3: // le joueur veut aller a gauche
    if(!playerList[joueur].posX){// vérifie que le joueur n'est pas tout à gauche du plateau
      getTuileFormated(tuileSuivante, playerList[joueur].posX-1, playerList[joueur].posY); // la tuile située à gauche
      if(tuileActuelle[3] == 0 && tuileSuivante[5] == 0){
        return 1;
      }else return 0;
    }
    else return 0;
    break;
  }
}

void getTuileFormated(int tuileResultat[9], int posX, int posY){
  int formeTuile = plateau[posY][posX].forme;
  switch (formeTuile) // selection de la bonne case depuis le modèle puis rotation
  {
  case 0: // case Vide
    copyTab(tuileVide, tuileResultat, 9); 
    rotationTuile(tuileResultat, plateau[posY][posX].rotation);
    break;
  case 1: // case L
    copyTab(tuileL, tuileResultat, 9);
    rotationTuile(tuileResultat, plateau[posY][posX].rotation);
    break;
  case 2: // case T
    copyTab(tuileT, tuileResultat, 9);
    rotationTuile(tuileResultat, plateau[posY][posX].rotation);
    break;
  case 3: // case I
    copyTab(tuileI, tuileResultat, 9);
    rotationTuile(tuileResultat, plateau[posY][posX].rotation);
    break;   
  }
}

/*pousse les tuile
       0   1   2
       ↓   ↓   ↓
    |_|_|_|_|_|_|_|
11 →|_|_|_|_|_|_|_|← 3
    |_|_|_|_|_|_|_|
10 →|_|_|_|_|_|_|_|← 4
    |_|_|_|_|_|_|_|
 9 →|_|_|_|_|_|_|_|← 5
    |_|_|_|_|_|_|_|
       ↑   ↑   ↑
       8   7   6
*/

void pushTuile(int emplacement){
  casePlateau bufferTuile = tuileRestante;
  if(emplacement < 3){
    int pos = (emplacement % 3)*2 + 1;
    tuileRestante = plateau[6][pos]; // définition de la nouvelle tuile restante
    for(int i = 6; i > 0; i-=1){
      plateau[i][pos] = plateau[i-1][pos];
    }
    plateau[0][pos] = bufferTuile; // tuile restante précédente
  }
  else if(emplacement >= 3 && emplacement < 6){
    int pos = (emplacement % 3)*2 + 1;
    tuileRestante = plateau[pos][0]; // définition de la nouvelle tuile restante
    for(int i = 0; i < 6; i+=1){
      plateau[pos][i] = plateau[pos][i+1];
    }
    plateau[pos][6] = bufferTuile; // tuile restante précédente
  }
  else if(emplacement >= 6 && emplacement < 9){
    int pos = (emplacement % 3)*2 + 1;
    pos = (pos == 1) ? 5 : (pos == 5) ? 1 : 3; // part de 1 3 5 pour avoir 5 3 1
    tuileRestante = plateau[0][pos]; // définition de la nouvelle tuile restante
    for(int i = 0; i < 6; i+=1){
      plateau[i][pos] = plateau[i+1][pos];
    }
    plateau[6][pos] = bufferTuile; // tuile restante précédente
  }
  else if(emplacement >= 9 && emplacement < 12){
    int pos = (emplacement % 3)*2 + 1;
    pos = (pos == 1) ? 5 : (pos == 5) ? 1 : 3; // part de 1 3 5 pour avoir 5 3 1
    tuileRestante = plateau[pos][6]; // définition de la nouvelle tuile restante
    for(int i = 6; i > 0; i-=1){
      plateau[pos][i] = plateau[pos][i-1];
    }
    plateau[pos][0] = bufferTuile; // tuile restante précédente
  }
}

void afficherEspace(int nombre){
  for(int i = 0; i < nombre; i++){
    printf("%s", caractere[0]);
  }
}

void printGUI(int indiceLigne){
  char ligne[130] = {0};
  getGUI(indiceLigne, ligne);
  printf("%s", ligne);
}



void getGUI(int nbLigne, char ligne[130]){
  switch (nbLigne)
  {
  case 0: sprintf(ligne,"         /$$                 /$$                           /$$             /$$     /$$                      "); break;
  case 1: sprintf(ligne,"        | $$                | $$                          |__/            | $$    | $$                      "); break;
  case 2: sprintf(ligne,"        | $$        /$$$$$$ | $$$$$$$  /$$   /$$  /$$$$$$  /$$ /$$$$$$$  /$$$$$$  | $$$$$$$   /$$$$$$       "); break;
  case 3: sprintf(ligne,"        | $$       |____  $$| $$__  $$| $$  | $$ /$$__  $$| $$| $$__  $$|_  $$_/  | $$__  $$ /$$__  $$      "); break;
  case 4: sprintf(ligne,"        | $$        /$$$$$$$| $$  \\ $$| $$  | $$| $$  \\__/| $$| $$  \\ $$  | $$    | $$  \\ $$| $$$$$$$$      "); break;
  case 5: sprintf(ligne,"        | $$       /$$__  $$| $$  | $$| $$  | $$| $$      | $$| $$  | $$  | $$ /$$| $$  | $$| $$_____/      "); break;
  case 6: sprintf(ligne,"        | $$$$$$$$|  $$$$$$$| $$$$$$$/|  $$$$$$$| $$      | $$| $$  | $$  |  $$$$/| $$  | $$|  $$$$$$$      "); break;
  case 7: sprintf(ligne,"        |________/ \\_______/|_______/  \\____  $$|__/      |__/|__/  |__/   \\___/  |__/  |__/ \\_______/      "); break;
  case 8: sprintf(ligne,"                                       /$$  | $$                                                            "); break;
  case 9: sprintf(ligne,"                                      |  $$$$$$/                                                            "); break;
  case 10: sprintf(ligne,"                                       \\______/                                                             "); break;
  case 11: sprintf(ligne,"                                                                                                           "); break;
  case 12: sprintf(ligne,"          +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+        "); break;
  case 13: sprintf(ligne,"          |J|O|U|E|U|R| |1|       |J|O|U|E|U|R| |2|       |J|O|U|E|U|R| |3|       |J|O|U|E|U|R| |4|        "); break;
  case 14: sprintf(ligne,"          +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+        "); break;
  case 15: sprintf(ligne,"                                                                                                           "); break;
  case 16: sprintf(ligne,"          +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+        "); break;
  case 17: sprintf(ligne,"          +               +       +               +       +               +       +               +        "); break;
  case 18: sprintf(ligne,"          +               +       +               +       +               +       +               +        "); break;
  case 19: sprintf(ligne,"          +               +       +               +       +               +       +               +        "); break;
  case 20: sprintf(ligne,"          +     🎈🎈      +       +     🏀🏀      +       +     💎💎      +       +     🎁🎁      +        "); break;
  case 21: sprintf(ligne,"          +     🎈🎈      +       +     🏀🏀      +       +     💎💎      +       +     🎁🎁      +        "); break;
  case 22: sprintf(ligne,"          +               +       +               +       +               +       +               +        "); break;
  case 23: sprintf(ligne,"          +               +       +               +       +               +       +               +        "); break;
  case 24: sprintf(ligne,"          +               +       +               +       +               +       +               +        "); break;
  case 25: sprintf(ligne,"          +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+       +-+-+-+-+-+-+-+-+        "); break;  
  }
}

void genererCarte(int carte[24]){
  for(int i = 0; i<24; i++){ //on génère 24 tuiles
    carte[i] = i;
  }
}

void printTab(int* tab, size_t tailleTab){
  for(int i = 0; i < tailleTab; i++){
    printf("%d ",tab[i]);
  }
}

void melangerTab(int* tab, size_t tailleTab){
  int j; //position avec laquelle notre valeur va être échangée
  int buffer;
  for(int i = 0; i < tailleTab; i++){ //on parcours le tableau pour échanger la valeur d'une case avec une autre
    j = getRandomInt(0,tailleTab-1); //on génère l'indice de la case avec laquelle notre valeur est échangée
    buffer = tab[j]; //on mémorise la valeur pour ne pas la perdre
    tab[j] = tab[i]; //on met i dans j
    tab[i] = buffer; //on met j dans i
  }
}

void movePlayer(int player, int direction){
  if(checkDeplacement(player, direction)){
    switch (direction) {
      case 0: playerList[player].posY -= 1; break;
      case 1: playerList[player].posX += 1; break;
      case 2: playerList[player].posY += 1; break;
      case 3: playerList[player].posX -= 1; break;
    }
  }
}

int askDeplacement(int joueur){
  int direction;
  printf("Joueur %d, c'est à vous. Dans quelle direction voulez vous aller ? \n", joueur+1);
  printf("%s : 1     %s : 2     %s : 3     %s : 4 \n", caractere[37], caractere[38], caractere[36], caractere[39]);
  scanf("%d", &direction);
  return direction-1;
}