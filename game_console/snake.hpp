#ifndef SNAKEHPP
#define SNAKEHPP

// globale variabeln

//Die Variable "SensorWert" dient zum Abspeichern des Sensorwertes.
int SensorWert = 0;

//Die Variable "Snakelength" dient zum Abspeichern der Schlangenlaenge.
char Snakelength;

//In der Variable var ist eine Richtungsaenderung gespeichert.
int var = 0;

//In der Variable "State" ist der aktuelle Zunstand der State Machine enthalten.
int State = 0;

//Mithilfe der Variable xtemp und ytemp werden die Koordinaten gespeichert bevor der Modulo 10 Rest abgezogen wird
char xtemp; 
char ytemp;

//Mithilfe von copyonemore wird nicht direkt ein Schlangenkoerperteil erzeugt, dass keine Koordinaten hat.
bool copyonemore = false;
//Mithilfe von "SeekFreePlace" wird eine Funktion aktiviert, wenn die erzeugte Koordinate der Frucht auf der Schlange liegt.
bool SeekFreePlace = false;

//Nachdem sich die Schlange einmal bewegt hat, braucht wird im interval von 250 ms gewartet, bevor man sich nochmal bewegt. 
unsigned long previousMillis = 0;
const long interval = 250;

struct Snake{
  int x;
  int y;
};


//Dies ist das Arrays in dem die Koordinaten der Schlangenkoerperteile gespeichert werden koennen.
struct Snake S[100] = {0};

//Die Koordinaten der Frucht wird in dieser Variable gepseichert.
struct Snake f; 

//Mithilfe der Enums sind verschiedene Zusteande definiert.
enum joystick { JOYUP, JOYDOWN, JOYRIGHT, JOYLEFT };
enum SNAKEMOVINGSTATE { UP, DOWN, RIGHT, LEFT};



void testdrawtext(char *text, uint16_t color, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}


//Mit dieser Funktion wird das Spielfeld erstellt.
void GeneratePlaygound(){
  tft.fillScreen(ST7735_BLACK);
  tft.fillRect(10, 30, 100, 100, ST7735_WHITE);
  tft.setRotation(3);
  testdrawtext("SNAKE", ST7735_WHITE, 60, 115);
};

//Mit dieser Funktion wird der Joystick eingelesen
void ReadJoyStick(){
  //JOYUP, JOYDOWN, JOYRIGHT, JOYLEFT
  SensorWert = analogRead(A0);
  if (SensorWert > 800){ var = JOYUP;}
  if (SensorWert < 200){ var = JOYDOWN;}
  SensorWert = analogRead(A1);
  if (SensorWert > 800){ var = JOYRIGHT;}
  if (SensorWert < 200){ var = JOYLEFT;} 
}
//ST7735_BLACK
//ST7735_WHITE

//Die Unterschiedlichen Funktionen wie MoveUp, MoveDown, MoveRight und Moveleft sind Funktionen die in verschiedenen Zustaenden ausgeführt wird. 
void MoveUp(){
  tft.drawRect(S[Snakelength-1].x+1,S[Snakelength-1].y+1,8,8,ST7735_WHITE);
  if(copyonemore == true){
    copyonemore = false;
    Snakelength++;
  }
  for(int i = Snakelength-2; i > -1; i--){
    S[i+1] = S[i];
  }
  S[0].x -= 10;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_BLACK);
}
void MoveDown(){
  tft.drawRect(S[Snakelength-1].x+1,S[Snakelength-1].y+1,8,8,ST7735_WHITE);
  if(copyonemore == true){
    copyonemore = false;
    Snakelength++;
  }
  for(int i = Snakelength-2; i > -1; i--){
    S[i+1] = S[i];
  }
  S[0].x += 10;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_BLACK);
}
void MoveRight(){
  tft.drawRect(S[Snakelength-1].x+1,S[Snakelength-1].y+1,8,8,ST7735_WHITE);
  if(copyonemore == true){
    copyonemore = false;
    Snakelength++;
  }
  for(int i = Snakelength-2; i > -1; i--){
    S[i+1] = S[i];
  }
  S[0].y += 10;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_BLACK);
}
void MoveLeft(){
  tft.drawRect(S[Snakelength-1].x+1,S[Snakelength-1].y+1,8,8,ST7735_WHITE);
  if(copyonemore == true){
    copyonemore = false;
    Snakelength++;
  }
  for(int i = Snakelength-2; i > -1; i--){
    S[i+1] = S[i];
  }
  S[0].y -= 10;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_BLACK);
}


//Diese Funktion sucht nach einem freien Feld, welches nicht von der Schlange belegt ist.
void LookOutForFreeSpace(){
    if(SeekFreePlace == true){
      for(int ii = 0; ii < 10; ii++){
        for(int iii = 0; iii < 10; iii++){
          for(int iv = 0; iv < Snakelength; iv++){
            if(!(ii*10+30 == S[iv].x && iii*10+10 == S[iv].y)){
              f.x = ii*10+30;
              f.y = iii*10+10;
              SeekFreePlace == false;
              return;
            }
          }
        }
      }
    }
}

//Mit dieser Funktion werden die Früchte generiert
void generatefood(){
  if(S[0].x == f.x && S[0].y == f.y){
    tft.drawRect(f.x+2,f.y+2,6,6,ST7735_WHITE);
    xtemp = random(30,120); 
    ytemp = random(10,100);
    //Das dient dazu die Koordianten abzurunden auf volle Vielfache von 10.
    f.x = xtemp - (xtemp%10);
    f.y = ytemp - (ytemp%10);
    copyonemore = true;
    //Ueberpruefe ob die Frucht auf einem belegten Feld liegt.
    for(int k = 0; k < Snakelength; k++){
      if(S[k].x == f.x && S[k].y == f.y){
        SeekFreePlace = true;
        break;
      }
    }
    LookOutForFreeSpace();
    tft.drawRect(f.x+2,f.y+2,6,6,ST7735_RED);
  }
}
//char Array2d[10][10] = {{0,0}};

void isgameover(){
  tft.fillScreen(ST7735_BLACK);
  //tft.setRotation(3);
  testdrawtext("GAME OVER", ST7735_WHITE, 0, 80);
  delay(50000);
}

void collisiondetection(){
  
  //Hier wird die Wandkollision berechnet
  if((S[0].x > 120 || S[0].x < 30) || (S[0].y > 100 || S[0].y < 10)){
    isgameover();
  }

  //Hier wird die Kollision mit dem eigenen Koerper berchnet.
  for(int i = 1; i < Snakelength-1; i++){
    if(S[0].x == S[i].x && S[0].y == S[i].y ){
    isgameover();
    }
  }
}

//Diese Funktion zeigt aber einer Schlangenlaenge von 100 an dass das Spiel als gewonnen gilt.
void isgamewon(){
  if(Snakelength == 100){
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  testdrawtext("YOU WON!", ST7735_WHITE, 38, 70);
  testdrawtext("CONGRATULATIONS", ST7735_WHITE, 38, 80);
  while(true){delay(50000);}
  }
}

//Alle Funktionen im initSnake werden nur einmal ausgeführt.
void initSnake(){

  tft.setTextSize(1);

  uint16_t time = millis();
  Serial.println(time, DEC);
  delay(100);

  //Hiermit das Spielfeld generiert.
  GeneratePlaygound();

  //Die Anfangsschlangenlaenge ist 3
  Snakelength = 3;

  //Hier wird der Schlangenkopf initialisiert
  S[0].x = 90;
  S[0].y = 70;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_WHITE);

  //Hier wird der Schlangenkörper initialisiert
  S[1].x = 90;
  S[1].y = 60;
  tft.drawRect(S[1].x+1,S[1].y+1,8,8,ST7735_WHITE);

  //Hier wird das Schlangenende initialisiert
  S[2].x = 90;
  S[2].y = 50;
  tft.drawRect(S[2].x+1,S[2].y+1,8,8,ST7735_WHITE);

  //Diese Koordinaten dienen der erzeugen der ersten Frucht
  f.x = 80;
  f.y = 80;
  tft.drawRect(f.x+2, f.y+2, 6, 6,ST7735_RED);
  State = RIGHT;
}


//Dieser Codeabschnitt kann mithilfe des Abstrahierten Spielablaufs und der State Machine gut nachvollzogen werden. 
void SnakeGame(){
  unsigned long currentMillis = millis();
  //JOYUP, JOYDOWN, JOYRIGHT, JOYLEFT
  ReadJoyStick();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    switch (State) {
        case UP:
        MoveUp();
        if(var == JOYRIGHT){
            State = RIGHT;
        }
        if(var == JOYLEFT){
            State = LEFT;
        }
        break;
        case DOWN:
        MoveDown();
        if(var == JOYRIGHT){
            State = RIGHT;
        }
        if(var == JOYLEFT){
            State = LEFT;
        }
        break;
        case RIGHT:
        MoveRight();
        if(var == JOYUP){
            State = UP;
        }
        if(var == JOYDOWN){
            State = DOWN;
        }
        break;
        case LEFT:
        MoveLeft();
        if(var == JOYUP){
            State = UP;
        }
        if(var == JOYDOWN){
            State = DOWN;
        }
        break;
        default:
        break;
    }
    isgamewon();
    generatefood();
    collisiondetection();
  }
}

#endif
