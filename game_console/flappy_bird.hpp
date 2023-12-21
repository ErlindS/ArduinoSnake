#ifndef FLAPPY_BIRD
#define FLAPPY_BIRD




/* 128x160

Pinnung
 *  Display (Pin) - Arduino Nano
 *  GND  (1) - GND  
 *  VCC  (2) - 5V
 *  SCK  (3) - D13
 *  SDA  (4) - D11
 *  RES  (5) - D8
 *  RS   (6) - D9
 *  CS   (7) - D10
 *  LEDA (8) - 3.3V
 *  
 *  LEDA kann auch an 5V doch dann wird das Display sehr schnell sehr heiß - was ich nicht für optimal halte.
 *  Beim Betrieb mit 3.3V ist das Dispklay nur minimal dunkler und bleibt kalt.

https://learn.adafruit.com/adafruit-gfx-graphics-library/coordinate-system-and-units
https://www.greth.me/arduino-nano-mit-1-77-zoll-spi-tft-display-st7735-chipsatz/

#define ST7735_BLACK ST77XX_BLACK
#define ST7735_WHITE ST77XX_WHITE
#define ST7735_RED ST77XX_RED
#define ST7735_GREEN ST77XX_GREEN
#define ST7735_BLUE ST77XX_BLUE
#define ST7735_CYAN ST77XX_CYAN
#define ST7735_MAGENTA ST77XX_MAGENTA
#define ST7735_YELLOW ST77XX_YELLOW
#define ST7735_ORANGE ST77XX_ORANGE
 */

#define buttonpinA 2
#define buttonpinB 3
#define buttonpinC 4
#define buttonpinD 5

#define do_not_erase 0
#define erase_screen 1
#define erase_square 2

enum pillar_directions {bottom, top};
#define PILLAR_WIDTH 23
#define DISTANCE_BETWEEN_PILLARS 37
#define BIRD_SIZE 15


/*
 * Innerhalb einer Sekunde werden zahlreiche Schleifeniterationen des gameloops durchlaufen.
 * Wenn ein Nutzer einen Knopf drückt, wird der Knopf höchstwahrscheinlich über meherere Schleifeniterationen hinweg gedrückt.
 * Optimaler Weise ist ein Knopf nur für eine einzige Iteration "aktiviert" und ist danach, bis der Knopf losgelassen und wieder gedrückt wird, "deaktiviert".
 * Mit dieser Klasse können Knöpfe in solcher Weise genutzt werden. Die Funktion "refreshAll" muss in jeder Schleifeniteration ausgeführt werden.
 * Anschließend kann man der Funktion "isOn" auf einem Objekt "Button" prüfen, ob dieser in der aktuellen Iteration als "aktiviert" gilt.
 */
struct buttons_class {
  struct button {
    bool state;
    int pin;
    int digitalread;
    int counter;
    unsigned long timer;
    unsigned long nDelay;

    button () {
      //this->pin = iPin;
      this->digitalread = 1;
      this->state = false;
      this->counter = 0;
      this->timer = 0;
      this->nDelay = 50;
      pinMode(this->pin, INPUT_PULLUP);
    }

    bool isOn() {
      return state;
    }

    void readPin() {
      this->digitalread = digitalRead(this->pin);
    }

    void refresh() {
      readPin();
      if (this->digitalread == LOW && this->counter == 0) {
        this->state = true;
        counter = 1;
        this->timer = millis();
        return;
      }
      if (this->digitalread == LOW && this->counter == 1) {
        this->state = false;
        counter = 2;
        return;
      }
      if (this->digitalread == HIGH && this->counter >= 1 && millis() >= this->timer + this->nDelay) {
        this->state = false;
        counter = 0;
        this->timer = 0;
        return;
      }
    }

  };

  // Für die Spielekonsole werden nur zwei Buttons benötigt
  button A;
  button B;

  buttons_class () {
    this->A.pin = buttonpinA;
    this->B.pin = buttonpinB;
  }

  void refreshAll() {
    this->A.refresh();
    this->B.refresh();
  }

};

// Mit diesem zwei-dimensionalen Vektor können X- und Y-Koordinaten kompakter gehandhabt werden
struct vector2 {
  int x;
  int y;
  vector2()               { this->x = 0;  this->y = 0; }
  vector2(int ix, int iy) { this->x = ix; this->y = iy; }

  void set(int ix, int iy) { this->x = ix; this->y = iy; }
};


// Um die Kollisionserkennung zu vereinfachen bekommt jedes rechteckige Objekt eine Instanz der Klasse "corners".
// Darin enthalten sind vier Vektoren, welche die vier Ecken des Rechtecks repräsentieren
struct corners {
  vector2 topLeft;
  vector2 topRight;
  vector2 bottomLeft;
  vector2 bottomRight;
  corners () {
    topLeft.set(0, 0);
    topRight.set(0, 0);
    bottomLeft.set(0, 0);
    bottomRight.set(0, 0);
  }
};


// Diese Klasse repräsentiert den Vogel. Hier wird die Position des Vogels gespeichert, die neue Position und Geschwindigkeit berechnet,
// und der Vogel auf der Anzeige dargestellt.
struct bird_class {
  corners corner;
  vector2 previous_position;
  int g;
  double maxSpeed;
  double speed;
  double speed_diff;


  bird_class() {
    this->reset();
  }

  void applyPhysics() { // Physikalischer Versuch: Wurf nach Oben
    this->previous_position.y = this->corner.topLeft.y;

    corner.topLeft.y = (corner.topLeft.y + (speed * speed_diff)) - (0.5 * this->g * speed_diff * speed_diff);
    speed = speed + (this->g * speed_diff);

    // Prevent bird from exceeding vertical border
    if (corner.topLeft.y < 0) {
      corner.topLeft.y = 0;
    }

    this->calculateCorners_topLeft();
  }


  // Um nach oben zu fliegen muss die Geschwindigkeit auf einen Wert kleiner als Null gesetzt werden
  void flap() {
    this->speed = -27.0;
  }


  // Mit dieser Funktion wird der Vogel mit seiner aktuellen Position auf der Anzeige dargestellt.
  // Mit dem bevorzugten Parameter "erase_square" wird der Vogel auf seiner vorherigen Position auf dem Bildschirm schwarz übermalt.
  // Dies ist deutlich schneller als bei jeder Schleifeniteration den gesamten Bildschirm zu übermalen
  void draw(int eraseMethod) {
    switch (eraseMethod) {
    case do_not_erase:
      break;

    case erase_screen: 
      tft.fillScreen(ST7735_BLACK);
      break;

    case erase_square:
      tft.fillRect(previous_position.x, previous_position.y, BIRD_SIZE, BIRD_SIZE, ST7735_BLACK);
      break;

    default:
      break; 
    }

    tft.fillRect(corner.topLeft.x, corner.topLeft.y, BIRD_SIZE, BIRD_SIZE, ST7735_YELLOW);
  }

  // Setzt alle Attribute auf ihre Standardwerte. Wird bei jedem neuen Spiel aufgerufen
  void reset() {
    corner.topLeft.y = 30;
    corner.topLeft.x = 30;
    previous_position.y = 30;
    previous_position.x = 30;

    this->g                 = 20;          // default: 20
    this->maxSpeed          = 20.0;        // default: 20.0
    this->speed             = maxSpeed;
    this->speed_diff        = 0.175;       // default: 0.05
  }

  // Hier werden alle Ecken, ausgehend von der Ecke "topLeft" berechnet
  void calculateCorners_topLeft() {
    corner.topRight.x = corner.topLeft.x + BIRD_SIZE;
    corner.topRight.y = corner.topLeft.y;

    corner.bottomLeft.x = corner.topLeft.x;
    corner.bottomLeft.y = corner.topLeft.y + BIRD_SIZE;

    corner.bottomRight.x = corner.topLeft.x + BIRD_SIZE;
    corner.bottomRight.y = corner.topLeft.y + BIRD_SIZE;
  }

  // Mit dieser Hilfsfunktion kann ermittelt werden, ob der Vogel den Boden berührt hat
  bool check_border_collision() {
    // Returns true if border is hit
    calculateCorners_topLeft();
    if (corner.bottomLeft.y >= 128) {
      return true;
    }
    else return false;
  }

};


// Diese Klasse verwaltet alle Säulen im gesamten Spiel
struct pillars {

  // Die Klasse "single_pillar" umfasst eine einzelne Säule.
  // Wenn es sich dabei um eine "untere" Säule handelt, wird die Funktion make_pillar_bottom() aufgerufen
  // Wenn es sich dabei um eine "obere" Säule handelt, wird die Funktion make_pillar_top() aufgerufen
  // Diese einzelnen Pillars werden nur von der Klasse "two_pillars" allokiert.

  struct single_pillar {
    corners corner;
    pillar_directions direction; // Veraltet
    int height;
    vector2 previous_position;

    single_pillar () {
      setTopLeft(0, 0);
      height = 0;
    }

    single_pillar (int iHeight) {
      this->height = iHeight;
    }

    void setTopLeft(int ix, int iy) {
      this->corner.topLeft.set(ix, iy);
    }

    // Mit der Hilfsfunktion kann eine Säule durch Angabe eine Höhe generiert werden
    void make_pillar_from_height(int iHeight, int x_coordinate, int y_offset) {
      this->height = iHeight;
      corner.topLeft.x = x_coordinate;
      corner.topLeft.y = 128 - iHeight - y_offset;
    }

    // Identisch zu gleichnamiger Funktion in bird_class
    void calculateCorners_topLeft() {
      corner.topRight.x = corner.topLeft.x + PILLAR_WIDTH;
      corner.topRight.y = corner.topLeft.y;

      corner.bottomLeft.x = corner.topLeft.x;
      corner.bottomLeft.y = corner.topLeft.y + height;

      corner.bottomRight.x = corner.topLeft.x + PILLAR_WIDTH;
      corner.bottomRight.y = corner.topLeft.y + height;

    }

    /* Um zwei Pillar zu generieren muss erst der untere Pillar generiert werden
     * Nötige Parameter: Random Height, Random Distance, X-Koordinate
     * Der untere Pillar braucht nur die Height und X-Koordinate,
     * der obere Pillar braucht die selbe Height, eine Distance und eine X-Koordinate
     * Davon ausgehend generiert sich der obere Pillar eigenständig
     */

    void make_pillar_bottom(int iHeight, int x_coordinate) {
      this->direction = bottom;

      make_pillar_from_height(iHeight, x_coordinate, 0);
      calculateCorners_topLeft();
    }

    void make_pillar_top(int iHeight_of_bottom, int x_coordinate, int distance) {
      this->direction = top;
      // HEIGHT_UPPER = 128 - distance - height
      // y_offset = distance + height

      int upper_height = 128 - distance - iHeight_of_bottom;
      int offset       = distance + iHeight_of_bottom;

      make_pillar_from_height(upper_height, x_coordinate, offset);
      calculateCorners_topLeft();
    }

    // Identisch zu gleichnamiger Funktion in bird_class
    void draw(int eraseMethod) {
      switch (eraseMethod) {
        case do_not_erase:
          break;

        case erase_screen: 
          tft.fillScreen(ST7735_BLACK);
          break;

        case erase_square:
          tft.drawRect(previous_position.x, previous_position.y, PILLAR_WIDTH, height, ST7735_BLACK);
          break;

        default:
          break; 
        }
      tft.fillRect(corner.topLeft.x, corner.topLeft.y, PILLAR_WIDTH, height, ST7735_GREEN);
    }
    
    // Speichert die aktuelle Position als die vorherige Position und verschiebt die X-Koordinaten aller Säulen um eins nach links
    void shift_left() {
      previous_position.x = corner.topLeft.x;
      previous_position.y = corner.topLeft.y;

      --corner.topLeft.x;
      --corner.topRight.x;
      --corner.bottomLeft.x;
      --corner.bottomRight.x;
    }

  }; // single_pillar

  struct two_pillars {
    single_pillar lower;
    single_pillar upper;
    bool score_counted; // If true, the player already got a Score for passing this pillars

    two_pillars () {
      score_counted = false;
      this->initialize();
    }

    void initialize() {
      // pull random numbers
      // random(minHeight, maxHeight);
      int nHeight;
      int nDistance;
      int border_size = 15;
      int x_coordinate = 159;

      do {
      randomSeed(analogRead(0) * analogRead(1) + analogRead(2));
      nHeight    = random(border_size, 90);
      nDistance  = random(50, 80);
      }
      while (nHeight + nDistance > 128 - border_size); // shuffle again if height+distance is too large

      lower.make_pillar_bottom(nHeight, x_coordinate);
      upper.make_pillar_top(nHeight, x_coordinate, nDistance);
    }

    void draw(int eraseMethod) {
      lower.draw(eraseMethod);
      upper.draw(eraseMethod);
    }

    void shift_left() {
      lower.shift_left();
      upper.shift_left();
    }

    // Hier wird geprüft, ob der vorderste Pillar nichtmehr sichtbar ist
    bool check_if_outofbounds() {
      // Returns true if invisible
      if (lower.corner.topRight.x < 0) {
        return true;
      }
      else return false;
    }

    // Hier wird geprüft, ob ein neuer Pillar nachgerückt werden muss
    bool check_for_distance_between_pillars() {
      if (lower.corner.topRight.x < 160 - DISTANCE_BETWEEN_PILLARS) {
        return true;
      }
      else return false;
    }

    // Bird-Pillar collision is determined in gameloop, because bird and pillar do not have access to each other
    // Similarly, the Score is also determined in gameloop

    // Bugfix
    void set_score_counted(bool input) {
      this->score_counted = input;
    }

  }; // two_pillars

  // Diese Klasse verwaltet alle Säulen im Spiel. Bei Bedarf werden Säulen allokiert oder deallokiert

  // Hier werden alle Säulenpaare gespeichert
  std::vector<two_pillars *> pillarvector;

  // Vektor initialisieren
  void initialize_vector() {
    pillarvector.push_back(new two_pillars);
  }

  // Vektor leeren
  void clear_vector() {
    for (auto i : pillarvector) {
      delete i;
    }
    pillarvector.clear();
  }

  // Verschiebt alle Säulenpaare nach links
  void shift_left() {
    for (auto i : pillarvector) {
      i->shift_left();
    }
  }

  // Löscht ggf. vordersten Pillar
  void check_if_front_is_outofbounds() {
    if (pillarvector.front()->check_if_outofbounds()) {
      delete pillarvector.front();
      pillarvector.erase(pillarvector.begin());
    }
  }

  // Rückt ggf. einen neuen Pillar nach
  void check_if_new_pillar_is_needed() {
    if (pillarvector.back()->check_for_distance_between_pillars()) {
      pillarvector.push_back(new two_pillars);
    }
  }

  // Kombiniert Linksverschiebung mit Grenzüberprüfung
  void shift_left_and_check() {
    shift_left();
    check_if_front_is_outofbounds();
    check_if_new_pillar_is_needed();
  }

  // Stellt alle Säulen auf der Anzeige dar
  void draw_all_pillars(int eraseMethod) {
    for (auto i : pillarvector) {
      i->draw(eraseMethod);
    }
  }

  
}; // pillars

// Übergreifende Klasse, welche den Spielablauf regelt
struct gameloop {
  bird_class * bird;
  buttons_class * buttonsobj;
  pillars * pillarsobj;

  bool runLoop;
  bool pause;

  unsigned int score;

  gameloop() {
    bird = new bird_class;
    buttonsobj = new buttons_class;
    pillarsobj = new pillars;
    this->runLoop = false;
    this->pause = false;
    this->score = 0;
  }

  ~gameloop() {
    delete bird;
    delete buttonsobj;
    delete pillarsobj;
  }

  // Hier findet der gesamte Spielablauf statt
  void loop() {
    unsigned long oldTimer = millis();
    unsigned long nDelay = 20;

    tft.fillScreen(ST7735_BLACK);
    pillarsobj->initialize_vector();


    while (!pause) {
      // Hier wird der Startbildschirm angezeigt und das Spiel auf eine neue Runde vorbereitet
      this->press_button_screen();
      this->reset_game();
      tft.fillScreen(ST7735_BLACK);
      this->runLoop = true;

      // Hier findet der zentrale Spielalgorithmus statt
      while (runLoop) {
        if (millis() >= oldTimer + nDelay) {
          buttonsobj->refreshAll();

          if (buttonsobj->A.isOn()) {
            this->bird->flap();
          }

          this->bird->applyPhysics();
          this->pillarsobj->shift_left_and_check();

          this->bird->draw(erase_square);
          this->pillarsobj->draw_all_pillars(erase_square);

          this->check_score();
          this->print_score(); 

          if (this->check_bird_pillar_collision()) {
            this->game_over_screen();
          }

          if (this->bird->corner.bottomLeft.y >= 128) {
            this->game_over_screen();
          }

          oldTimer = millis();
        }
      }
    }

  }

  // Alle Attribute zurücksetzen
  void reset_game() {
    this->runLoop = true;
    this->pause = false;
    this->score = 0;
    this->bird->reset();
    this->pillarsobj->clear_vector();
    this->pillarsobj->initialize_vector();
  }

  void setLoop(bool activateLoop) {
    this->runLoop = activateLoop;
  }

  void setPause(bool activatePause) {
    this->pause = activatePause;
  }

  void switchLoop() {
         if (this->runLoop == false) { this->runLoop = true; }
    else if (this->runLoop == true)  { this->runLoop = false; }
  }
  
  void switchPause() {
         if (this->pause == false) { this->pause = true; }
    else if (this->pause == true)  { this->pause = false; }
  }

  // Hier wird die Kollision zwischen Vogel und Säule geprüft
  bool check_bird_pillar_collision() {

    for (auto i : pillarsobj->pillarvector) {
      if (
            // X-Axis
          ( (bird->corner.topRight.x > i->upper.corner.topLeft.x && bird->corner.topRight.x < i->upper.corner.topRight.x)
            ||
            (bird->corner.topLeft.x > i->upper.corner.topLeft.x && bird->corner.topLeft.x < i->upper.corner.topRight.x) )

            &&

            // Y-Axis
          ( (bird->corner.topRight.y > i->upper.corner.topLeft.y && bird->corner.topRight.y < i->upper.corner.bottomLeft.y) // Upper Pillar
            ||
            (bird->corner.bottomRight.y > i->lower.corner.topLeft.y && bird->corner.bottomRight.y < i->lower.corner.bottomLeft.y) ) // Lower Pillar

        ) {
          return true;
        } // if
    } // for
    return false;
  }

  // Hier wird geprüft, ob der Spieler einen Punkt bekommt
  void check_score() {
    if ((pillarsobj->pillarvector.front()->lower.corner.topRight.x < bird->corner.topLeft.x) && pillarsobj->pillarvector.front()->score_counted == false) {
      score++;
      //pillarsobj->pillarvector.front()->score_counted == true; // Somehow doesnt work
      pillarsobj->pillarvector.front()->set_score_counted(true);
    }
  }

  // Zeigt den Punktestand an
  void print_score() {
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setTextSize(1);
    tft.setTextWrap(true);

    tft.print("Score: ");
    tft.print(this->score);
  }

  // Startbildschirm
  void press_button_screen() {
    tft.fillScreen(ST7735_BLACK);

    // Flappy Bird Title
    tft.setCursor(29, 17);
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setTextSize(1);
    tft.setTextWrap(false);

    tft.print("Flappy Bird HSPF");

    // White Rectangle
    int wWidth = 160;
    int wHeight = 50;
    int wTopLeft_x = (160 - wWidth) / 2;
    int wTopLeft_y = (128 - wHeight) / 2;

    tft.fillRoundRect(wTopLeft_x, wTopLeft_y, wWidth, wHeight, 5, ST7735_WHITE);

    // Press Button Text
    int tTopLeft_x = 9;
    int tTopLeft_y = 56;

    tft.setCursor(tTopLeft_x, tTopLeft_y);
    tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
    tft.setTextSize(2);
    tft.setTextWrap(false);

    tft.print("Press Button");
    
    // Blinking Border & Button Polling
    int rect_distance = 4;
    int rTopLeft_x = wTopLeft_x + 1;
    int rTopLeft_y = wTopLeft_y + 1;
    int rWidth = wWidth - 2;
    int rHeight = wHeight - 2;

    unsigned long timer = millis();
    unsigned long delay = 500;
    int lState = 0;
    while (1) {
      buttonsobj->refreshAll();
      if (buttonsobj->A.isOn()) {
        return;
      }

      if (millis() >= timer + delay) {
        if (lState == 0) {
          for (int i = 0; i < rect_distance; i++) {
            tft.drawRoundRect(rTopLeft_x + i, rTopLeft_y + i, rWidth - (i * 2), rHeight - (i * 2), 5, ST7735_BLACK);
            lState = 1;
          }
        }
        else if (lState == 1) {
          for (int i = 0; i < rect_distance; i++) {
            tft.drawRoundRect(rTopLeft_x + i, rTopLeft_y + i, rWidth - (i * 2), rHeight - (i * 2), 5, ST7735_WHITE);
            lState = 0;
          }
        }

        timer = millis();
      }

    }

  }

  // Game Over Anzeige
  void game_over_screen() {
    this->runLoop = false;

    // Blue Rectangle
    int wWidth = 140;
    int wHeight = 115;
    int wTopLeft_x = (160 - wWidth) / 2;
    int wTopLeft_y = (128 - wHeight) / 2;

    tft.fillRect(wTopLeft_x, wTopLeft_y, wWidth, wHeight, ST7735_BLUE);

    // White Border
    int rect_size = 17;
    int initial_distance = 5;

    for (int i = 0; i <= rect_size; i++) {
      tft.drawRect( wTopLeft_x + initial_distance + i, 
                    wTopLeft_y + initial_distance + i, 
                    wWidth - (initial_distance * 2) - (i * 2), 
                    wHeight - (initial_distance * 2) - (i * 2), 
                    ST7735_WHITE);
    }

    // Game Over Text
    tft.setCursor(50, 16);
    tft.setTextColor(ST7735_BLUE, ST7735_WHITE);
    tft.setTextSize(1);
    tft.setTextWrap(false);

    tft.print("Game Over!");

    // Score Text
    tft.setCursor(37, 42);
    tft.setTextColor(ST7735_WHITE, ST7735_BLUE);
    tft.setTextSize(1);
    tft.setTextWrap(false);

    tft.print("Your Score: ");
    tft.print(this->score);

    // Press Button Text
    tft.setCursor(45, 70);
    tft.setTextColor(ST7735_WHITE, ST7735_BLUE);
    tft.setTextSize(1);
    tft.setTextWrap(true);

    tft.print("Press Button");

    tft.setCursor(45, 80);
    tft.setTextColor(ST7735_WHITE, ST7735_BLUE);
    tft.setTextSize(1);
    tft.setTextWrap(true);

    tft.print("to try again");

    while (1) {
      buttonsobj->refreshAll();
      if (buttonsobj->A.isOn()) {
        return;
      }
    }

  }

};

gameloop gameloop1;

// Setup Funktion
void initFlappyBird() {
  // Initialize Screen
  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(3);      // equivalent to adafruit doc example orientation

  // Initialize Buttons
  pinMode(buttonpinA, INPUT_PULLUP);
  pinMode(buttonpinB, INPUT_PULLUP);

  tft.fillScreen(ST7735_BLACK);
  
  gameloop1.setLoop(true);
}


#endif
