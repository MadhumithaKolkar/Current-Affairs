#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define JUMP_BTN 2
#define DUCK_BTN 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int GROUND_Y = 58;
const int DINO_X = 8;
const int STAND_H = 16;
const int DUCK_H = 8;
const int DUCK_W = 18;

float dinoY, vy;
bool onGround, ducking, gameOver;

#define MAX_OBS 3
int obsX[MAX_OBS];
uint8_t obsType[MAX_OBS];
bool obsActive[MAX_OBS];

float gameSpeed;
unsigned long score;
unsigned int highScore = 0;
unsigned long lastFrame;
uint8_t legFrame;

void resetGame() {
  dinoY = GROUND_Y - STAND_H; vy = 0;
  onGround = true; ducking = false; gameOver = false;
  for (int i = 0; i < MAX_OBS; i++) obsActive[i] = false;
  gameSpeed = 2.2; score = 0;
  obsX[0] = SCREEN_WIDTH + 10; obsType[0] = 0; obsActive[0] = true;
}

void setup() {
  pinMode(JUMP_BTN, INPUT_PULLUP);
  pinMode(DUCK_BTN, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  Wire.setClock(400000);
  display.setTextColor(SSD1306_WHITE);
  randomSeed(analogRead(A7));
  resetGame();
  lastFrame = millis();
}

void getObsRect(int i, int &x, int &y, int &w, int &h) {
  x = obsX[i];
  switch (obsType[i]) {
    case 0: w = 6;  h = 12; y = 46; break;
    case 1: w = 10; h = 18; y = 40; break;
    case 2: w = 12; h = 6;  y = 42; break;
    default: w = 12; h = 6; y = 28; break;
  }
}

bool overlap(int ax,int ay,int aw,int ah,int bx,int by,int bw,int bh){
  return ax < bx+bw && ax+aw > bx && ay < by+bh && ay+ah > by;
}

void spawnIfNeeded() {
  int rightmost = -1000;
  for (int i = 0; i < MAX_OBS; i++)
    if (obsActive[i] && obsX[i] > rightmost) rightmost = obsX[i];
  int gap = 55 + (int)random(0, 45);
  if (rightmost < SCREEN_WIDTH - gap) {
    for (int i = 0; i < MAX_OBS; i++) {
      if (!obsActive[i]) {
        obsActive[i] = true; obsX[i] = SCREEN_WIDTH + 5;
        int r = random(0, 100);
        if (score > 150 && r < 35) obsType[i] = (r < 17) ? 2 : 3;
        else obsType[i] = (r < 50) ? 0 : 1;
        break;
      }
    }
  }
}

void loop() {
  if (millis() - lastFrame < 40) return;
  lastFrame = millis();

  bool jumpPressed = (digitalRead(JUMP_BTN) == LOW);
  ducking = (digitalRead(DUCK_BTN) == LOW);

  if (gameOver) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(28, 18); display.print(F("GAME OVER"));
    display.setCursor(28, 32); display.print(F("Score: ")); display.print(score);
    display.setCursor(6, 48); display.print(F("Press JUMP to retry"));
    display.display();
    if (jumpPressed) resetGame();
    return;
  }

  if (jumpPressed && onGround) { vy = -6.0; onGround = false; }
  if (!onGround) {
    vy += 0.6; dinoY += vy;
    int groundTop = GROUND_Y - STAND_H;
    if (dinoY >= groundTop) { dinoY = groundTop; vy = 0; onGround = true; }
  }

  for (int i = 0; i < MAX_OBS; i++) {
    if (!obsActive[i]) continue;
    obsX[i] -= (int)gameSpeed;
    int x,y,w,h; getObsRect(i,x,y,w,h);
    if (x + w < 0) obsActive[i] = false;
  }
  spawnIfNeeded();

  int dx, dy, dw, dh;
  dx = DINO_X;
  if (ducking && onGround) { dw = DUCK_W; dh = DUCK_H; dy = GROUND_Y - DUCK_H; }
  else { dw = 14; dh = STAND_H; dy = (int)dinoY; }

  for (int i = 0; i < MAX_OBS; i++) {
    if (!obsActive[i]) continue;
    int x,y,w,h; getObsRect(i,x,y,w,h);
    if (overlap(dx,dy,dw,dh,x,y,w,h)) {
      gameOver = true;
      if (score > highScore) highScore = score;
    }
  }

  score++;
  if (score % 100 == 0 && gameSpeed < 6.0) gameSpeed += 0.3;
  legFrame = (legFrame + 1) & 3;

  display.clearDisplay();
  display.drawLine(0, GROUND_Y, SCREEN_WIDTH, GROUND_Y, SSD1306_WHITE);

  if (ducking && onGround) {
    display.fillRect(DINO_X, GROUND_Y - DUCK_H, DUCK_W, DUCK_H, SSD1306_WHITE);
  } else {
    int yy = (int)dinoY;
    display.fillRect(DINO_X, yy + 4, 8, STAND_H - 4, SSD1306_WHITE);
    display.fillRect(DINO_X + 6, yy, 8, 8, SSD1306_WHITE);
    display.fillRect(DINO_X + 11, yy + 2, 2, 2, SSD1306_BLACK);
    if (onGround) {
      if (legFrame < 2) display.fillRect(DINO_X + 1, GROUND_Y - 2, 3, 2, SSD1306_BLACK);
      else display.fillRect(DINO_X + 5, GROUND_Y - 2, 3, 2, SSD1306_BLACK);
    }
  }

  for (int i = 0; i < MAX_OBS; i++) {
    if (!obsActive[i]) continue;
    int x,y,w,h; getObsRect(i,x,y,w,h);
    if (obsType[i] <= 1) {
      display.fillRect(x, y, w, h, SSD1306_WHITE);
      display.fillRect(x - 2, y + 3, 2, 4, SSD1306_WHITE);
      display.fillRect(x + w, y + 5, 2, 4, SSD1306_WHITE);
    } else {
      display.fillRect(x + 3, y + 1, 6, 3, SSD1306_WHITE);
      if (legFrame < 2) { display.fillRect(x, y, 4, 2, SSD1306_WHITE); display.fillRect(x + 8, y, 4, 2, SSD1306_WHITE); }
      else { display.fillRect(x, y + 3, 4, 2, SSD1306_WHITE); display.fillRect(x + 8, y + 3, 4, 2, SSD1306_WHITE); }
    }
  }

  display.setTextSize(1);
  display.setCursor(0, 0); display.print(score);
  display.setCursor(80, 0); display.print(F("HI:")); display.print(highScore);
  display.display();
}