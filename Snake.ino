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


void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! ST7735 TFT Test");
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
  // Use this initializer (uncomment) if you're using a 0.96" 180x60 TFT
  //tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display
  Serial.println("Initialized");
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  //time = millis() - time;
  Serial.println(time, DEC);
  //delay(500);
  // large block of text
  //chart welcome = "Welcome to the greatest video game console ";
  //testdrawtext(welcome, 0);
  testdrawtext("The best console you will ever see", ST7735_WHITE);
  delay(500);
  tft.fillScreen(ST7735_WHITE);

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
}

void loop() {
  //tft.invertDisplay(true);
  //delay(500);
  //tft.invertDisplay(false);
  //delay(500);

  SensorWert = analogRead(A0);
  if (SensorWert > 1000){ var = 1;}
  if (SensorWert < 100){ var = 2;}
  SensorWert = analogRead(A1);
  if (SensorWert > 1000){ var = 3;}
  if (SensorWert < 100){ var = 4;} 
  /*
  switch (Richtung)​{
      case 1:
        // statements
        S[0].x += 10;
        break;
      case 2:
        // statements
        S[0].x -= 10;
        break;
      case 3:
        // statements
        S[0].y += 10;
        break;
      case 4:
        // statements
        S[0].y -= 10;
        break;
      
  }
  */
  switch (var) {
  case 1:
    //do something when var equals 1
    //for(int i = 0; i< Snakelength; i++){
      deleterecttangle(S[Snakelength-1]);
    //}
    for(int i = Snakelength-2; i > -1; i--){
      S[i+1].y = S[i].y;
      S[i+1].x = S[i].x;
    }
    //S[1] = S[0];
    S[0].x -= 10;/*
    for(int i = 1; i < Snakelength; i++){
      S[i-1] = S[i];
    }*/
    for(int i = 0; i< Snakelength; i++){
      recttangle(S[i]);
    }
    delay(1000);
    break;
  case 2:
    //do something when var equals 2
    //deleterecttangle(S[Snakelength-1]);
    //S[Snakelength] = S[Snakelength -1];
    //for(int i = 0; i< Snakelength; i++){
      deleterecttangle(S[Snakelength-1]);
    //}
    for(int i = Snakelength-2; i > -1; i--){
      S[i+1].y = S[i].y;
      S[i+1].x = S[i].x;
    }
    //S[1] = S[0];
    S[0].x += 10;
    /*for(int i = Snakelength -1; i > 1; i--){
      S[i-1] = S[i];
    }*/
    for(int i = 0; i< Snakelength; i++){
      recttangle(S[i]);
    }
    delay(1000);
    break;
  case 3:
    //do something when var equals 2
    //for(int i = 0; i< Snakelength; i++){
      deleterecttangle(S[Snakelength-1]);
    //}
    for(int i = Snakelength-2; i > -1; i--){
      S[i+1].y = S[i].y;
      S[i+1].x = S[i].x;
    }
    //S[1] = S[0];
    S[0].y += 10;/*
    for(int i = Snakelength -1; i > 1; i--){
      S[i-1] = S[i];
    }*/
    for(int i = 0; i< Snakelength; i++){
      recttangle(S[i]);
    }
    //S[Snakelength] = S[Snakelength -1];
    //S[0].y += 10;
    //recttangle(S[0]);
    delay(1000);
    break;
  case 4:
    //do something when var equals 2
    //for(int i = 0; i< Snakelength; i++){
      deleterecttangle(S[Snakelength-1]);
    //}
    for(int i = Snakelength-2; i > -1; i--){
      S[i+1].y = S[i].y;
      S[i+1].x = S[i].x;
    }
    //S[1] = S[0];
    S[0].y -= 10;
    /*for(int i = Snakelength -1; i > 1; i--){
      S[i-1] = S[i];
    }*/
    for(int i = 0; i< Snakelength; i++){
      recttangle(S[i]);
    }
    delay(1000);
    break;
  default:
    // if nothing else matches, do the default
    // default is optional
    break;
  }
  //deleterecttangle(S[0]);
  //recttangle(S[0]);
  //delay(1000);

  Serial.print((String)"Das ist X.1 " + S[0].x + "Das ist Y.1 " + S[0].y + "\n");
  Serial.print((String)"Das ist X.2 " + S[1].x + "Das ist Y.2 " + S[1].y + "\n");
  Serial.print((String)"Das ist X.3 " + S[2].x + "Das ist Y.3 " + S[2].y + "\n \n \n");
}

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

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void recttangle(struct Snake S) {
  tft.drawLine(S.x, S.y, S.x + 10, S.y , ST7735_BLACK);
  tft.drawLine(S.x, S.y, S.x, S.y + 10, ST7735_BLACK);
  tft.drawLine(S.x + 10 , S.y, S.x + 10, S.y +10, ST7735_BLACK);
  tft.drawLine(S.x, S.y + 10, S.x + 10, S.y +10 , ST7735_BLACK);
}

void deleterecttangle(struct Snake S) {
  tft.drawLine(S.x, S.y, S.x + 10, S.y , ST7735_WHITE);
  tft.drawLine(S.x, S.y, S.x, S.y + 10, ST7735_WHITE);
  tft.drawLine(S.x + 10 , S.y, S.x + 10, S.y +10, ST7735_WHITE);
  tft.drawLine(S.x, S.y + 10, S.x + 10, S.y +10 , ST7735_WHITE);
}
