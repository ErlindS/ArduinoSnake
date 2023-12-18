#ifndef SNAKEHPP
#define SNAKEHPP

// globale variabeln
int SensorWert = 0;
int var = 0;
char Snakelength;
int State = 0;
char xtemp; 
char ytemp;
unsigned long previousMillis = 0;
const long interval = 250;
char avoidmissgenerationx = 0;
char avoidmissgenerationy = 0;
bool copyonemore = false;
char currentPositionx;
char currentPositiony;


struct Snake{
  int x;
  int y;
};

struct Snake S[100] = {0};
struct Snake f; 


enum joystick { JOYUP, JOYDOWN, JOYRIGHT, JOYLEFT };
enum SNAKEMOVINGSTATE { UP, DOWN, RIGHT, LEFT};



void testdrawtext(char *text, uint16_t color, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void GeneratePlaygound(){
  tft.fillRect(10,40,100,100, ST7735_WHITE);
  testdrawtext("SNAKE", ST7735_WHITE, 10, 10);
  testdrawtext("Press Joystick to", ST7735_WHITE, 10, 20);
  testdrawtext("go to the Menu", ST7735_WHITE, 10, 30);
  
};


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
void MoveUp(){
    //decreaselegnth();
  tft.drawRect(S[Snakelength-1].x+1,S[Snakelength-1].y+1,8,8,ST7735_WHITE);
  if(copyonemore == true){
    copyonemore = false;
    Snakelength++;
  }
  //currentPositiony += 1;
  //Field[currentPositiony][currentPositionx] = Snakelength;
  for(int i = Snakelength-2; i > -1; i--){
    S[i+1] = S[i];
  }
  S[0].x -= 10;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_BLACK);
}
void MoveDown(){
    //decreaselegnth();
  tft.drawRect(S[Snakelength-1].x+1,S[Snakelength-1].y+1,8,8,ST7735_WHITE);
  if(copyonemore == true){
    copyonemore = false;
    Snakelength++;
  }
  //currentPositiony -= 1;
  //Field[currentPositiony][currentPositionx] = Snakelength;
  for(int i = Snakelength-2; i > -1; i--){
    S[i+1] = S[i];
  }
  S[0].x += 10;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_BLACK);
}
void MoveRight(){
   // decreaselegnth();
  tft.drawRect(S[Snakelength-1].x+1,S[Snakelength-1].y+1,8,8,ST7735_WHITE);
  if(copyonemore == true){
    copyonemore = false;
    Snakelength++;
  }
  //currentPositionx += 1;
  //Field[currentPositiony][currentPositionx] = Snakelength;
  for(int i = Snakelength-2; i > -1; i--){
    S[i+1] = S[i];
  }
  S[0].y += 10;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_BLACK);
}
void MoveLeft(){
    //decreaselegnth();
  tft.drawRect(S[Snakelength-1].x+1,S[Snakelength-1].y+1,8,8,ST7735_WHITE);
  if(copyonemore == true){
    copyonemore = false;
    Snakelength++;
  }
  //currentPositionx -= 1;
  //Field[currentPositiony][currentPositionx] = Snakelength;
  for(int i = Snakelength-2; i > -1; i--){
    S[i+1] = S[i];
  }
  S[0].y -= 10;
  tft.drawRect(S[0].x+1,S[0].y+1,8,8,ST7735_BLACK);
}


void generatefood(){
  if(S[0].x == f.x && S[0].y == f.y){
    tft.drawRect(f.x+2,f.y+2,6,6,ST7735_WHITE);
    xtemp = random(10,100); 
    ytemp = random(40,130);
    f.x = xtemp - (xtemp%10);
    f.y = ytemp - (ytemp%10);
    copyonemore = true;
    for(int k = 0; k < Snakelength; k++){
      if(S[k].x == f.x && S[k].y == f.y){
        f.x = (k%10)*10+10;
        f.y = (k/10)*10+40;
      }
    }
    tft.drawRect(f.x+2,f.y+2,6,6,ST7735_RED);
    /*
    for(int LmaoA = 0; LmaoA < 10; LmaoA++){
      for(int LmaoB = 0; LmaoB < 10; LmaoB++){
        if(Field[LmaoA][LmaoB] == 0){
          //Offsets 10,40
          f.x = LmaoA*10+10;
          f.y = LmaoB*10+40;
          tft.drawRect(LmaoA+12,LmaoB+42,6,6,ST7735_RED);
          return;
        }
      }
    }*/
  }
}
//char Array2d[10][10] = {{0,0}};

void isgameover(){
  tft.fillScreen(ST7735_BLACK);
  testdrawtext("GAME OVER", ST7735_WHITE, 38, 70);
  testdrawtext("Press Joystick to", ST7735_WHITE, 10, 120);
  testdrawtext("go to the Menu", ST7735_WHITE, 10, 130);
  delay(50000);
}

void collisiondetection(){
  if((S[0].x > 110 || S[0].x < 10) || (S[0].y > 140 || S[0].y < 40)){
    Serial.print(S[0].x);
    Serial.print(S[0].y);
    //Serial.print(x);
    //Serial.print(x);
    isgameover();
  }
  for(int i = 1; i < Snakelength; i++){
    if(S[0].x == S[i].x && S[0].y == S[i].y ){
    isgameover();
    }
  }
}

void isgamewon(){
  if(Snakelength == 100){
  tft.fillScreen(ST7735_BLACK);
  testdrawtext("YOU WON, congratulations", ST7735_WHITE, 38, 70);
  testdrawtext("CONGRATULATIONS", ST7735_WHITE, 38, 80);
  testdrawtext("Press Joystick to", ST7735_WHITE, 10, 120);
  testdrawtext("go to the Menu", ST7735_WHITE, 10, 130);
  delay(50000);
  }
}

void initSnake(){

  tft.setRotation(0);
  tft.setTextSize(1);

  uint16_t time = millis();
  Serial.println(time, DEC);
  delay(100);
  tft.fillScreen(ST7735_BLACK);
  GeneratePlaygound();

  Snakelength = 3;
  S[0].x = 90;
  S[0].y = 70;
  Field[8][3] = Snakelength; 

  S[1].x = 90;
  S[1].y = 60;
  Field[8][2] = Snakelength;

  S[2].x = 90;
  S[2].y = 50;
  Field[8][2] = Snakelength;

  currentPositionx = 9;
  currentPositiony = 7;

  //For generating the first fruit Food, hardcoded since no one cares
  f.x = 100;
  f.y = 100;
  tft.drawRect(f.x+2, f.y+2, 6, 6,ST7735_RED);
  //GeneratePlayground();
  State = RIGHT;
}

void SnakeGame(){
  unsigned long currentMillis = millis();
  isgamewon();
  generatefood();
  collisiondetection();
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
  }
}

#endif
