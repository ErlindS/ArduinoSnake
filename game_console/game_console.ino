#include "globalDefs.hpp"
#include "pong.hpp"
#include "snake.hpp"
#include "flappy_bird.hpp"

#define MENU_DISTANCE 30

enum games {
  snake = 1,
  pong,
  flappy
};

enum games selectedGame = snake;

void drawMenu() {

  tft.fillRect(5, 10 + MENU_DISTANCE, 10, MENU_DISTANCE * 3, ST7735_BLACK);

  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 40);
  tft.println("SNAKE");
  tft.setCursor(20, 70);
  tft.println("PONG");
  tft.setCursor(20, 100);
  tft.println("FLAPPY BIRD");

  tft.setTextColor(ST7735_GREEN);
  switch (selectedGame) {
    case snake:
      tft.setCursor(5, 10 + selectedGame * MENU_DISTANCE);
      tft.println(">");
      tft.setCursor(20, 10 + selectedGame * MENU_DISTANCE);
      tft.println("SNAKE");
      break;
    case pong:
      tft.setCursor(5, 10 + selectedGame * MENU_DISTANCE);
      tft.println(">");
      tft.setCursor(20, 10 + selectedGame * MENU_DISTANCE);
      tft.println("PONG");
      break;
    case flappy:
      tft.setCursor(5, 10 + selectedGame * MENU_DISTANCE);
      tft.println(">");
      tft.setCursor(20, 10 + selectedGame * MENU_DISTANCE);
      tft.println("FLAPPY BIRD");
      break;
    default: break;
  }
}


void setup() {
  //Initializing Serial Port and TFT
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);

  //Joystick ONE init
  pinMode(SW_ONE, INPUT_PULLUP);
  digitalWrite(SW_ONE, HIGH);

  //init random
  randomSeed(analogRead(0) * analogRead(1) + analogRead(2));  //might not be perfect but works just fine


  //Displaying Menu
  tft.setRotation(3);  //widescreen view
  tft.fillScreen(ST7735_BLACK);

  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);

  tft.setCursor(2, 2);
  tft.println("Navigate with Joystick ONE");
  tft.setCursor(2, 12);
  tft.println("Press Joystick to select!");
  tft.drawFastHLine(0, 22, 160, ST7735_WHITE);

  drawMenu();
}

void loop() {

  uint8_t joy_pressed = digitalRead(SW_ONE);
  int selector = analogRead(PLAYER1_JOY_Y);
  static int game_dir_old = 0;  //< has to be static because it would get initialized in every single loop

  int8_t game_dir = 0;
  if (selector > 750) game_dir = -1;      //move down
  else if (selector < 350) game_dir = 1;  //move up
  else game_dir = 0;


  if (game_dir != game_dir_old) {
    game_dir_old = game_dir;

    switch (game_dir) {
      case 0:
        break;
      case -1:  //moving down
        if (selectedGame == flappy) selectedGame = snake;
        else selectedGame = (games)((char) selectedGame + 1);
        drawMenu();
        break;
      case 1:  //moving up
        if (selectedGame == snake) selectedGame = flappy;
        else selectedGame = (games)((char) selectedGame - 1);
        drawMenu();
        break;
    }
  }


  if (joy_pressed == 0) {  //< Joystick is low active

    switch (selectedGame) {
      case snake:
        initSnake();
        while(1) SnakeGame();
        break;
      case pong: //< execute pong
        if(initPong()) Serial.println("Pong init done!");
        while(1) loopPong();
        break;
      case flappy:
        initFlappyBird();
        gameloop1.loop();
        break;
    }
  }
}
