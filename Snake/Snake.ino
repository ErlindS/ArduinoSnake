#include <Adafruit_GFX.h>    // include Core graphics library
#include <Adafruit_ST7735.h> // include Hardware-specific library
#include <SPI.h>
// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset                      
#define TFT_DC     9  // in which case, set this #define pin to -1!
// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);



int SensorWert = 0;
int var = 0;
int Snakelength;

struct Snake{
  int x;
  int y;
};
struct Snake S[100];
struct Snake f; 

int delaytime;

enum { JOYUP, JOYDOWN, JOYRIGHT, JOYLEFT } jostick;
//enum { JOYUP, JOYDOWN, JOYRIGHT, JOYLEFT } state;

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! ST7735 TFT Test");
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  //tft.initR(INITR_WHITETAB);
  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
  // Use this initializer (uncomment) if you're using a 0.96" 180x60 TFT
  //tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display
  Serial.println("Initialized");
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  tft.fillScreen(ST7735_WHITE);
  //time = millis() - time;
  Serial.println(time, DEC);
  //delay(500);
  // large block of text
  //chart welcome = "Welcome to the greatest video game console ";
  //testdrawtext(welcome, 0);
  testdrawtext("The best console you will ever see", ST7735_BLACK);
  delay(500);
  tft.fillScreen(ST7735_BLACK);

  Snakelength = 3;
  S[0].x = 40;
  S[0].y = 40;

  S[1].x = 30;
  S[1].y = 40;

  S[2].x = 20;
  S[2].y = 40;

  //starting Point  
  recttangle(S[0]);

  //For generating the first fruit Food 
  f.x = 100;
  f.y = 100;
  recttangle(f);
  delaytime = 250;
}

void loop() {
  generatefood();
  isgameover();
  //JOYUP, JOYDOWN, JOYRIGHT, JOYLEFT
  SensorWert = analogRead(A0);
  if (SensorWert > 900){ var = JOYUP;}
  if (SensorWert < 200){ var = JOYDOWN;}
  SensorWert = analogRead(A1);
  if (SensorWert > 900){ var = JOYRIGHT;}
  if (SensorWert < 200){ var = JOYLEFT;} 

  switch (var) {
  case JOYUP:
    MoveUp();
    break;
  case JOYDOWN:
    MoveDown();
    break;
  case JOYRIGHT:
    MoveRight();
    break;
  case JOYLEFT:
    MoveLeft();
    break;
  default:
    break;
  }
}

void MoveUp(){
    deleterecttangle(S[Snakelength-1]);
    for(int i = Snakelength-2; i > -1; i--){
      S[i+1] = S[i];
    }
    S[0].x -= 10;
    for(int i = 0; i< Snakelength; i++){
      recttangle(S[i]);
    }
    delay(delaytime);
}
void MoveDown(){
    deleterecttangle(S[Snakelength-1]);
    for(int i = Snakelength-2; i > -1; i--){
      S[i+1] = S[i];
    }
    S[0].x += 10;
    for(int i = 0; i< Snakelength; i++){
      recttangle(S[i]);
    }
    delay(delaytime);
}
void MoveRight(){
    deleterecttangle(S[Snakelength-1]);
    for(int i = Snakelength-2; i > -1; i--){
      S[i+1] = S[i];
    }
    S[0].y += 10;
    for(int i = 0; i< Snakelength; i++){
      recttangle(S[i]);
    }
    delay(delaytime);
}
void MoveLeft(){
    deleterecttangle(S[Snakelength-1]);
    for(int i = Snakelength-2; i > -1; i--){
      S[i+1] = S[i];
    }
    S[0].y -= 10;
    for(int i = 0; i< Snakelength; i++){
      recttangle(S[i]);
    }
    delay(delaytime);
}

  /*
  Serial.print((String)"Das ist X.1 " + S[0].x + "Das ist Y.1 " + S[0].y + "\n");
  Serial.print((String)"Das ist X.2 " + S[1].x + "Das ist Y.2 " + S[1].y + "\n");
  Serial.print((String)"Das ist X.3 " + S[2].x + "Das ist Y.3 " + S[2].y + "\n \n \n");
  */

void generatefood(){
    if(S[0].x == f.x && S[0].y == f.y){
    Snakelength++;
    deleterecttangle(f);
    int xtemp; 
    int ytemp;
    xtemp = random(120); 
    ytemp = random(160);
    f.x = xtemp - (xtemp%10);
    f.y = ytemp - (ytemp%10);
    recttangle(f);
  }
}

void isgameover(){
  if((S[0].x > 120 || S[0].x < 0) || (S[0].y > 160 || S[0].y < 0)){
  tft.fillScreen(ST7735_BLACK);
  testdrawtext("GAME OVER", ST7735_WHITE);
  delay(50000);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void recttangle(struct Snake S) {
  tft.drawLine(S.x, S.y, S.x + 10, S.y , ST7735_WHITE);
  tft.drawLine(S.x, S.y, S.x, S.y + 10, ST7735_WHITE);
  tft.drawLine(S.x + 10 , S.y, S.x + 10, S.y +10, ST7735_WHITE);
  tft.drawLine(S.x, S.y + 10, S.x + 10, S.y +10 , ST7735_WHITE);
}

void deleterecttangle(struct Snake S) {
  tft.drawLine(S.x, S.y, S.x + 10, S.y , ST7735_BLACK);
  tft.drawLine(S.x, S.y, S.x, S.y + 10, ST7735_BLACK);
  tft.drawLine(S.x + 10 , S.y, S.x + 10, S.y +10, ST7735_BLACK);
  tft.drawLine(S.x, S.y + 10, S.x + 10, S.y +10 , ST7735_BLACK);
}
