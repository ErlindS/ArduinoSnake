#ifndef PONGHPP
#define PONGHPP

/// PONG defines    -----------------------------------------------------------------------------------------------------------------------

//joystick
#define PLAYER1_JOY_Y A1
#define PLAYER2_JOY_Y A3

// framerate
#define FRAMERATE 10
unsigned long timer;

//fixed values
#define MAX_X 159
#define MAX_Y 127
#define BAT_TO_SCREEN_DISTANCE 10
#define BAT_SPEED 4              // in pixels per frame (dont use odd values!)
#define BALL_SIZE 2               // radius in pixels

#define COL_BACK ST7735_BLACK
#define COL_BAT ST7735_WHITE
#define COL_WIN  ST7735_GREEN

//variable values
unsigned char bat_hight = 48;            // Values between 10 and 100
uint16_t      ball_color;
double        angle;                      // starting angle of ball
uint16_t      collisions = 0;             // keeping track of how many "pongs" on both players handle

//ball speed
#define BALL_SPEED 8;                     // Values from 5 to 10
unsigned int  ball_speed;                 // max 15

//scores
unsigned int scores[2] = {0,0};

//tracking positions on tft
typedef Position pos;

pos p1_bat, p2_bat;
pos ball_pos, ball_vel;


/// PONG functions --------------------------------------------------------------------------------------------------------------------------

//playing Sound on Buzzer
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

//only for Debug purposes
void pos_print(pos *p){
  Serial.print(p->x);
  Serial.print(" ");
  Serial.print(p->y);
  Serial.print('\n');
}

//you have to calculating the speed of the ball after it got changed
void recalculateBallSpeed(){
  ball_vel.x = (ball_vel.x > 0) ? ball_speed : -ball_speed;
  ball_vel.y = (ball_vel.y > 0) ? tan(angle) * ball_speed : -tan(angle) * ball_speed;
}


/// Displaying stuff on tft ----------------------------------------------------------------------------------------------------------------------

//Display Stripes in the middle of the Screen
void drawMiddleLine() {
  for(int i = 1; i < MAX_Y; i += 5){
    tft.drawRect(MAX_X/2, i, 2, 2, COL_BAT);
  }
}

//moving bat up and down
void moveBat(pos *bat, char direction){

  //border detection
  if(bat->y < BAT_SPEED                       && direction == 1) return; //wanting to go up but at hightes up!
  if(bat->y > MAX_Y+1 - BAT_SPEED - bat_hight  && direction == -1) return; //wanting to go down but at lowest down!

  //move the bat
  switch(direction){
    case 0:
      return;
    case 1:   //move up
      tft.drawFastVLine(bat->x, bat->y - BAT_SPEED               , bat_hight, COL_BAT); //add pixels on top
      tft.drawFastVLine(bat->x, bat->y + bat_hight - BAT_SPEED  , BAT_SPEED, COL_BACK); //remove pixels on bottoms
      bat->y -= BAT_SPEED;
      break;
    case -1:  //move down
      tft.drawFastVLine(bat->x, bat->y + BAT_SPEED , bat_hight, COL_BAT); //add pixels on bottom
      tft.drawFastVLine(bat->x, bat->y              , BAT_SPEED, COL_BACK); //remove pixels on top
      bat->y += BAT_SPEED;
      break;
    default: break; //should never be reached
  }
}

//drawing the ball with correct color and making sure it doesn't destroy other lines
void drawBall(pos *old, pos *new_pos){  

  tft.fillCircle(old->x,      old->y,     BALL_SIZE, COL_BACK);
  if (old->x >= 78 && old->x <= 81) { //middle markings are endangered!!!
    drawMiddleLine();
  }

  tft.fillCircle(new_pos->x,  new_pos->y, BALL_SIZE, ball_color);
}

//moving the ball
//returns true if the game is over
bool moveBall(pos *ball, pos *vel){
  //fix ball destroying bat
  char fixWall = 0;

  //determine new position
  //pos new_pos = pos_add(ball, vel);
  pos new_pos = *ball + *vel;

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

  if(new_pos.x > MAX_X - (BAT_TO_SCREEN_DISTANCE + BALL_SIZE) && new_pos.y > p2_bat.y && new_pos.y < p2_bat.y + bat_hight){ //ball hits p2 bat
    vel->x = -vel->x;
    new_pos = *ball + *vel;
    playTone(2);
    fixWall = 2;
    collisions++;
  }

  if(new_pos.x < 0 + (BAT_TO_SCREEN_DISTANCE + BALL_SIZE) && new_pos.y > p1_bat.y && new_pos.y < p1_bat.y + bat_hight){ //ball hits p1 bat
    vel->x = -vel->x;
    new_pos = *ball + *vel;
    playTone(1);
    fixWall = 1;
    collisions++;
  }

  drawBall(ball, &new_pos);
  *ball = new_pos; // set new position

  if(fixWall == 1) tft.drawFastVLine(p1_bat.x, p1_bat.y, bat_hight, COL_BAT);
  if(fixWall == 2) tft.drawFastVLine(p2_bat.x, p2_bat.y, bat_hight, COL_BAT);

  return false;
}

//Displaying scores at the end
void drawScores(){
  tft.fillScreen(COL_BACK);

  tft.setCursor(52, 12);
  tft.setTextColor(COL_BAT);
  tft.setTextSize(2);
  tft.println("SCORE");

  //Score Player 1
  tft.setCursor(34 , 64);
  tft.setTextSize(3);
  if(scores[0] > scores[1]) tft.setTextColor(COL_WIN);
  else tft.setTextColor(COL_BAT);
  tft.print(scores[0]);

  //Score Player 2
  tft.setCursor(80+34 , 64);
  tft.setTextSize(3);
  if(scores[1] > scores[0]) tft.setTextColor(COL_WIN);
  else tft.setTextColor(COL_BAT);
  tft.print(scores[1]);

  // :
  tft.fillRect(MAX_X/2, MAX_Y/2+4 + 10, 2, 2, COL_BAT);
  tft.fillRect(MAX_X/2, MAX_Y/2-4 + 10, 2, 2, COL_BAT);
}


// initialize         --------------------------------------------------------------------------------------------------------------------------
bool initPong(){

  //tft settings
  tft.setRotation(3); //for widescreen
  tft.fillScreen(COL_BACK);
  
  //tracking
  p1_bat   = {.x = 0+BAT_TO_SCREEN_DISTANCE        , .y = (MAX_Y+1)/2 - bat_hight/2};
  p2_bat   = {.x = MAX_X - BAT_TO_SCREEN_DISTANCE  , .y = (MAX_Y+1)/2 - bat_hight/2};
  ball_pos  = {.x = MAX_X / 2                        , .y = MAX_Y / 2                 };

  //draw starting bats
  tft.drawFastVLine( p1_bat.x , p1_bat.y , bat_hight, COL_BAT); // left bat in middle at start
  tft.drawFastVLine( p2_bat.x , p2_bat.y , bat_hight, COL_BAT); // right bat in middle at start

  //draw middle line
  drawMiddleLine();

  //get random spawn
  ball_pos.x = random(30, MAX_X-30);
  ball_pos.y = random(24, MAX_Y-24);

  //get random direction
  collisions = 0;
  ball_speed = BALL_SPEED;
  angle = ((double) random((3.141/12)*1000,(3.141/6)*1000) ) / 1000;
  ball_vel.x = BALL_SPEED;
  ball_vel.y = tan(angle) * BALL_SPEED;

  if(ball_pos.x > MAX_X/2) ball_vel.x = -ball_vel.x;
  if(ball_pos.y > MAX_Y/2) ball_vel.y = -ball_vel.y;

  //spawn ball
  ball_color = 0x07E0; //green as a start -> easy
  tft.fillCircle(ball_pos.x, ball_pos.y, BALL_SIZE, ball_color);

  //Debug
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

  //recalculate speed
  static uint16_t collisionsOld = 0;
  if(collisions != collisionsOld){
    collisionsOld = collisions;
    if((collisions % 7) == 0){
      if(ball_speed < 15) ball_speed++;
      recalculateBallSpeed();

      switch(ball_speed){
        case 9:
          ball_color = 0xFFE0; //yellow
          break;
        case 10:
          ball_color = 0xF800; //orange
          break;
        case 11:
          ball_color = 0xF800; //red
          break;
        case 12:
          ball_color = 0xB8BF; //purple
          break;
        case 13:
          ball_color = 0x001F; //blue
          break;
        case 14:
          ball_color = 0xBEFD; //light blue
          break;
        case 15:
          ball_color = 0x28A5; //expert mode :o
          break;
      }
    }
  }

  //update tft frame
  if(millis() > timer + (1000 / FRAMERATE)) {

    //move bats for each player
    moveBat(&p1_bat, p1_dir);
    moveBat(&p2_bat, p2_dir);

    //move the ball + colision detection / win detection
    if(moveBall(&ball_pos, &ball_vel)){
      //game over -> start next round

      //reset speed value
      collisions = 0;
      collisionsOld = 0;

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


#endif