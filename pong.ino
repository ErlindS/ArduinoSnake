#include <Adafruit_GFX.h>    // include Core graphics library
#include <Adafruit_ST7735.h> // include Hardware-specific library
#include <SPI.h>

//pins
#define TFT_CS    10
#define TFT_RST   0              
#define TFT_DC    9
#define TFT_SCLK  13
#define TFT_MOSI  11
#define BUZZER    7   // Digital pin 7

//tft
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

//joystick
#define PLAYER1_JOY_Y A1
#define PLAYER2_JOY_Y A3

/// pong

// framerate
#define FRAMERATE 10
unsigned long timer;

//fixed values
#define MAX_X 159
#define MAX_Y 127
#define WALL_TO_SCREEN_DISTANCE 10
#define WALL_SPEED 4              // in pixels per frame (dont use odd values!)
#define BALL_SIZE 2               // radius in pixels

#define COL_BACK ST7735_WHITE
#define COL_WALL ST7735_BLACK
#define COL_WIN  ST7735_GREEN

//variable values
unsigned char wall_hight = 48;            // Values between 10 and 100
unsigned int  ball_speed = 10;           // Values from 10 to 30
uint16_t      ball_color = ST7735_RED;

//scores
unsigned int scores[2] = {0,0};

//tracking positions on tft
typedef struct _pos {
  int x;
  int y;
} pos;

pos p1_wall, p2_wall;
pos ball_pos, ball_vel;

//helper functions    --------------------------------------------------------------------------------------------------------------------------

pos pos_add(pos *a, pos *b){
  pos ret = {.x = a->x + b->x, .y = a->y + b->y};
  return ret;
}

pos pos_times(pos *a, float k){
  pos ret = {.x = a->x * k, .y = a->y * k};
  return ret;
}

void playTone(char player){
  switch(player){
    case 1:
      tone(BUZZER , 330, 250);
      break;
    case 2:
      tone(BUZZER , 740, 250);
      break;
    case 3: //hitting top / bottom
      tone(BUZZER , 494, 250);
      break;
    default: break; //should never reach
  }
}

void pos_print(pos *p){
  Serial.print(p->x);
  Serial.print(" ");
  Serial.print(p->y);
  Serial.print('\n');
}


void drawMiddleLine() {
  for(int i = 1; i < MAX_Y; i += 5){
    tft.drawRect(MAX_X/2, i, 2, 2, COL_WALL);
  }
}


/*
void oldmoveWall(pos *wall, char direction){


  //border detection
  if(wall->y < WALL_SPEED                       && direction == 1) return; //wanting to go up but at hightes up!
  if(wall->y > MAX_Y+1 - WALL_SPEED - wall_hight  && direction == -1) return; //wanting to go down but at lowest down!

  //move the wall
  switch(direction){
    case 0:
      return;
    case 1:   //move up
      tft.drawFastVLine(wall->x, wall->y - WALL_SPEED               , WALL_SPEED, COL_WALL); //add pixels on top
      tft.drawFastVLine(wall->x, wall->y + wall_hight - WALL_SPEED  , WALL_SPEED, COL_BACK); //remove pixels on bottoms
      wall->y -= WALL_SPEED;
      break;
    case -1:  //move down
      tft.drawFastVLine(wall->x, wall->y + wall_hight , WALL_SPEED, COL_WALL); //add pixels on bottom
      tft.drawFastVLine(wall->x, wall->y              , WALL_SPEED, COL_BACK); //remove pixels on top
      wall->y += WALL_SPEED;
      break;
    default: break; //should never be reached
  }
}
*/

void moveWall(pos *wall, char direction){

  //border detection
  if(wall->y < WALL_SPEED                       && direction == 1) return; //wanting to go up but at hightes up!
  if(wall->y > MAX_Y+1 - WALL_SPEED - wall_hight  && direction == -1) return; //wanting to go down but at lowest down!

  //move the wall
  switch(direction){
    case 0:
      return;
    case 1:   //move up
      tft.drawFastVLine(wall->x, wall->y - WALL_SPEED               , wall_hight, COL_WALL); //add pixels on top
      tft.drawFastVLine(wall->x, wall->y + wall_hight - WALL_SPEED  , WALL_SPEED, COL_BACK); //remove pixels on bottoms
      wall->y -= WALL_SPEED;
      break;
    case -1:  //move down
      tft.drawFastVLine(wall->x, wall->y + WALL_SPEED , wall_hight, COL_WALL); //add pixels on bottom
      tft.drawFastVLine(wall->x, wall->y              , WALL_SPEED, COL_BACK); //remove pixels on top
      wall->y += WALL_SPEED;
      break;
    default: break; //should never be reached
  }
}

//moving the ball, returns true if the game is over
bool moveBall(pos *ball, pos *vel){
  //fix ball destroying wall
  char fixWall = 0;

  //determine new position
  pos new_pos = pos_add(ball, vel);

  //win detection
  if(new_pos.x < -BALL_SIZE){ //p2 won -> p1 missed the ball
    Serial.println("Player 2 scored a goal!");
    scores[1] += 1;
    return true;
  }

  if(new_pos.x > MAX_X + BALL_SIZE){ //p1 won -> p2 missed the ball
    Serial.println("Player 1 scored a goal!");
    scores[0] += 1;
    return true;
  }

  //colision detection
  if(new_pos.y < 0 || new_pos.y > MAX_Y){
    vel->y = -vel->y; //ball hits top / bottom edge
    playTone(3);
  }

  if(new_pos.x > MAX_X - (WALL_TO_SCREEN_DISTANCE + BALL_SIZE) && new_pos.y > p2_wall.y && new_pos.y < p2_wall.y + wall_hight){ //ball hits p2 wall
    vel->x = -vel->x;
    new_pos = pos_add(ball, vel);
    playTone(2);
    fixWall = 2;
  }

  if(new_pos.x < 0 + (WALL_TO_SCREEN_DISTANCE + BALL_SIZE) && new_pos.y > p1_wall.y && new_pos.y < p1_wall.y + wall_hight){ //ball hits p1 wall
    vel->x = -vel->x;
    new_pos = pos_add(ball, vel);
    playTone(1);
    fixWall = 1;
  }

  drawBall(ball, &new_pos);
  *ball = new_pos; // set new position

  if(fixWall == 1) tft.drawFastVLine(p1_wall.x, p1_wall.y, wall_hight, COL_WALL);
  if(fixWall == 2) tft.drawFastVLine(p2_wall.x, p2_wall.y, wall_hight, COL_WALL);

  return false;
}

void drawBall(pos *old, pos *new_pos){  

  tft.fillCircle(old->x,      old->y,     BALL_SIZE, COL_BACK);
  if (old->x >= 78 && old->x <= 81) { //middle markings are endangered!!!
    drawMiddleLine();
  }

  tft.fillCircle(new_pos->x,  new_pos->y, BALL_SIZE, ball_color);
}

void drawScores(){
  tft.fillScreen(COL_BACK);

  tft.setCursor(52, 12);
  tft.setTextColor(COL_WALL);
  tft.setTextSize(2);
  tft.println("SCORE");

  //Score Player 1
  tft.setCursor(34 , 64);
  tft.setTextSize(3);
  if(scores[0] > scores[1]) tft.setTextColor(COL_WIN);
  else tft.setTextColor(COL_WALL);
  tft.print(scores[0]);

  //Score Player 2
  tft.setCursor(80+34 , 64);
  tft.setTextSize(3);
  if(scores[1] > scores[0]) tft.setTextColor(COL_WIN);
  else tft.setTextColor(COL_WALL);
  tft.print(scores[1]);

  // :
  tft.fillRect(MAX_X/2, MAX_Y/2+4 + 10, 2, 2, COL_WALL);
  tft.fillRect(MAX_X/2, MAX_Y/2-4 + 10, 2, 2, COL_WALL);
}


// initialize         --------------------------------------------------------------------------------------------------------------------------
bool initPong(){

  //tft settings
  tft.setRotation(3); //for widescreen
  tft.fillScreen(COL_BACK);
  
  //tracking
  p1_wall   = {.x = 0+WALL_TO_SCREEN_DISTANCE        , .y = (MAX_Y+1)/2 - wall_hight/2};
  p2_wall   = {.x = MAX_X - WALL_TO_SCREEN_DISTANCE  , .y = (MAX_Y+1)/2 - wall_hight/2};
  ball_pos  = {.x = MAX_X / 2                        , .y = MAX_Y / 2                 };

  //draw starting walls
  tft.drawFastVLine( p1_wall.x , p1_wall.y , wall_hight, COL_WALL); // left wall in middle at start
  tft.drawFastVLine( p2_wall.x , p2_wall.y , wall_hight, COL_WALL); // right wall in middle at start

  //draw middle line
  drawMiddleLine();

  //get random spawn
  ball_pos.x = random(30, MAX_X-30);
  ball_pos.y = random(24, MAX_Y-24);

  //get random direction
  double angle = ((double) random((3.141/12)*1000,(3.141/6)*1000) ) / 1000;

  ball_vel.x = ball_speed;
  ball_vel.y = tan(angle) * ball_speed;


  if(ball_pos.x > MAX_X/2) ball_vel.x = -ball_vel.x;
  if(ball_pos.y > MAX_Y/2) ball_vel.y = -ball_vel.y;

  //spawn ball
  tft.fillCircle(ball_pos.x, ball_pos.y, BALL_SIZE, ball_color);

  

  Serial.println("Random ball velocity:");
  Serial.print("x: ");
  Serial.print(ball_vel.x);
  Serial.print(" y: ");
  Serial.print(ball_vel.y);
  Serial.print('\n');

  return 1;
}


// main loop          --------------------------------------------------------------------------------------------------------------------------
void loopPong(){
  //read joysticks
  int p1_y = analogRead(PLAYER1_JOY_Y);
  int p2_y = analogRead(PLAYER2_JOY_Y);

  //player directions
  int p1_dir = 0;
  int p2_dir = 0;

  //determine player directions
  if(p1_y > 750)        p1_dir = -1; //move down
  else if(p1_y < 350)   p1_dir =  1; //move up
  else                  p1_dir =  0;

  if(p2_y > 750)        p2_dir = -1; //move down
  else if(p2_y < 350)   p2_dir =  1; //move up
  else                  p2_dir =  0;

  //update tft frame
  if(millis() > timer + (1000 / FRAMERATE)) {

    //move walls for each player
    moveWall(&p1_wall, p1_dir);
    moveWall(&p2_wall, p2_dir);

    //move the ball + colision detection / win detection
    if(moveBall(&ball_pos, &ball_vel)){
      //game over -> start next round

      //debug printing scores for now -> later lcd screen
      Serial.println("");
      Serial.print("Score      ");
      Serial.print(scores[0]);
      Serial.print(" : ");
      Serial.print(scores[1]);
      Serial.print('\n');
      Serial.println("");

      //drawing scores on tft
      drawScores();

      //small melody
      tone(BUZZER , 415 , 250);
      delay(250);
      tone(BUZZER , 330 , 250);
      delay(250);
      tone(BUZZER , 415 , 250);
      delay(250);
      tone(BUZZER , 220 , 400);
      delay(400);

      initPong();
    }

    timer = millis(); //reset timer
  }

}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  
  //init random
  randomSeed(analogRead(0)*analogRead(1)+analogRead(2)); //change to time joystick pressed to start game -> more random seed

  if(initPong()) Serial.println("Pong init done!");
}

void loop() {
  // put your main code here, to run repeatedly:
  loopPong();
}
