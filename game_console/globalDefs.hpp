#ifndef GLOBALDEFSHPP
#define GLOBALDEFSHPP

#include <Adafruit_GFX.h>    // include Core graphics library
#include <Adafruit_ST7735.h> // include Hardware-specific library
#include <SPI.h>
#include <ArduinoSTL.h>

//pins
#define TFT_CS    10
#define TFT_RST   8              
#define TFT_DC    9
#define TFT_SCLK  13
#define TFT_MOSI  11

#define BUZZER    7     // Digital pin 7
#define SW_ONE    2     // Joy ONE Press

//tft
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

//position
struct Position{
    int x;
    int y;

    Position()              {this->x = 0; this->y = 0;}
    Position(int x, int y)  {this->x = x; this->y = y;}

    void set(int x, int y)  {this->x = x; this->y = y;}

    //operators
    Position operator+(Position other)  {return Position(this->x + other.x  , this->y + other.y);   }
};

#endif