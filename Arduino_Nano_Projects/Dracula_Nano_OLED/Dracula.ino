#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include <string.h>
#include <avr/pgmspace.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

enum Effect {
  EFFECT_NONE = 0, EFFECT_POP, EFFECT_SHAKE, EFFECT_INVERT,
  EFFECT_GLITCH, EFFECT_ZOOM_IN, EFFECT_TYPEWRITER, EFFECT_BOUNCE
};

enum BackgroundMode { BG_CUBE = 0, BG_LIGHTNING };

// Lyric table lives in FLASH (PROGMEM) to save RAM
struct Lyric {
  uint16_t delayBeforeMs;
  uint16_t durationMs;
  char word[10];
  uint8_t effect;
  uint8_t size;
};

const Lyric lyrics[] PROGMEM = {
  { 0, 300, "The", EFFECT_NONE, 2 },
  { 0, 300, "morning", EFFECT_POP, 2 },
  { 0, 300, "light", EFFECT_NONE, 2 },
  { 0, 300, "is", EFFECT_NONE, 2 },
  { 0, 300, "turning", EFFECT_NONE, 2 },
  { 100, 500, "BLUE", EFFECT_INVERT, 3 },

  { 0, 300, "the", EFFECT_NONE, 2 },
  { 0, 300, "feeling", EFFECT_BOUNCE, 2 },
  { 0, 300, "is", EFFECT_NONE, 2 },
  { 0, 800, "BIZARRE", EFFECT_TYPEWRITER, 2 },

  { 200, 300, "The", EFFECT_NONE, 2 },
  { 0, 300, "night", EFFECT_POP, 2 },
  { 0, 300, "is", EFFECT_NONE, 2 },
  { 0, 300, "almost", EFFECT_NONE, 2 },
  { 0, 500, "over", EFFECT_NONE, 2 },

  { 200, 300, "I", EFFECT_NONE, 2 },
  { 0, 300, "still", EFFECT_BOUNCE, 2 },
  { 0, 300, "don't", EFFECT_NONE, 2 },
  { 0, 300, "know", EFFECT_NONE, 2 },
  { 0, 200, "where", EFFECT_NONE, 2 },
  { 0, 200, "you", EFFECT_NONE, 2 },
  { 0, 600, "ARE", EFFECT_SHAKE, 3 },

  { 200, 300, "The", EFFECT_NONE, 2 },
  { 0, 400, "shadows", EFFECT_SHAKE, 2 },
  { 0, 300, "yeah", EFFECT_NONE, 2 },
  { 0, 300, "they", EFFECT_NONE, 2 },
  { 0, 300, "keep", EFFECT_NONE, 2 },
  { 0, 300, "me", EFFECT_NONE, 2 },
  { 0, 300, "pretty", EFFECT_POP, 2 },
  { 0, 300, "like", EFFECT_NONE, 2 },
  { 0, 300, "a", EFFECT_NONE, 2 },
  { 0, 300, "movie", EFFECT_NONE, 2 },
  { 0, 800, "STAR", EFFECT_INVERT, 3 },

  { 300, 500, "Daylight", EFFECT_TYPEWRITER, 2 },
  { 0, 300, "makes", EFFECT_NONE, 2 },
  { 0, 300, "me", EFFECT_NONE, 2 },
  { 0, 300, "feel", EFFECT_BOUNCE, 2 },
  { 0, 200, "like", EFFECT_NONE, 2 },

  { 100, 2800, "DRACULA", EFFECT_ZOOM_IN, 3 }
};
const int numLyrics = sizeof(lyrics) / sizeof(Lyric);

uint16_t startTimes[numLyrics];   // computed once, RAM
unsigned long TOTAL_LOOP_TIME = 0;
unsigned long startTime = 0;
int lastLyricIdx = -1;
BackgroundMode currentBg = BG_CUBE;

// --- Cube state ---
float cubeAX = 0, cubeAY = 0, cubeAZ = 0;

// --- Lightning state ---
#define MAX_BOLT_PTS 7
#define MAX_BOLTS 3
struct LightningBolt {
  int bx[MAX_BOLT_PTS], by[MAX_BOLT_PTS];
  int numPts, framesLeft; bool active;
} bolts[MAX_BOLTS];

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Wire.setClock(400000);
  display.setTextColor(WHITE);
  display.setTextWrap(false);

  for (int i = 0; i < MAX_BOLTS; i++) bolts[i].active = false;

  unsigned long running = 0;
  for (int i = 0; i < numLyrics; i++) {
    running += pgm_read_word(&lyrics[i].delayBeforeMs);
    startTimes[i] = running;
    running += pgm_read_word(&lyrics[i].durationMs);
  }
  TOTAL_LOOP_TIME = running + 3000;
  startTime = millis();
}

void loop() {
  unsigned long now = millis() - startTime;
  if (now > TOTAL_LOOP_TIME) {
    startTime = millis(); now = 0; lastLyricIdx = -1; currentBg = BG_CUBE;
  }

  display.clearDisplay();

  int idx = -1;
  for (int i = 0; i < numLyrics; i++) {
    uint16_t dur = pgm_read_word(&lyrics[i].durationMs);
    if (now >= startTimes[i] && now <= (unsigned long)startTimes[i] + dur) { idx = i; break; }
  }

  Effect eff = EFFECT_NONE;
  bool lyricActive = false;
  float progress = 0;
  bool invertScreen = false;
  Lyric l;

  if (idx != -1) {
    memcpy_P(&l, &lyrics[idx], sizeof(Lyric));
    lyricActive = true;
    eff = (Effect)l.effect;
    progress = (now - startTimes[idx]) / (float)l.durationMs;

    if (idx != lastLyricIdx) {
      if (eff == EFFECT_INVERT || eff == EFFECT_SHAKE || eff == EFFECT_ZOOM_IN)
        currentBg = BG_LIGHTNING;
      else
        currentBg = BG_CUBE;
      lastLyricIdx = idx;
    }
  }

  if (currentBg == BG_LIGHTNING) drawLightning(eff, lyricActive);
  else drawCube(eff, lyricActive);

  if (idx != -1) {
    int textSize = l.size, offsetX = 0, offsetY = 0;
    char typeBuf[12];
    const char* displayWord = l.word;
    bool showCursor = false;

    if (eff == EFFECT_TYPEWRITER) {
      int total = strlen(l.word);
      int show = 1 + (int)(progress * total);
      if (show > total) show = total;
      strncpy(typeBuf, l.word, show); typeBuf[show] = '\0';
      displayWord = typeBuf; showCursor = (show < total);
    }

    if (eff == EFFECT_POP) { if (progress < 0.15) textSize = l.size + 1; }
    else if (eff == EFFECT_BOUNCE) {
      float amp = 22.0 * exp(-5.0 * progress);
      offsetY = -(int)(amp * fabs(cos(progress * PI * 4.0)));
    } else if (eff == EFFECT_SHAKE) { offsetX = random(-3, 4); offsetY = random(-3, 4); }
    else if (eff == EFFECT_INVERT) {
      invertScreen = true;
      if (random(10) > 5) { offsetX = random(-2, 3); offsetY = random(-2, 3); }
    } else if (eff == EFFECT_ZOOM_IN) {
      if (progress < 0.05) textSize = l.size > 1 ? l.size - 1 : 1;
      else if (progress < 0.1) textSize = l.size;
      else textSize = l.size + 1;
      if (progress > 0.4) { offsetX = random(-4, 5); offsetY = random(-4, 5); }
      if (progress > 0.3 && random(10) > 7) invertScreen = true;
    }

    uint16_t w = strlen(displayWord) * 6 * textSize;
    uint16_t h = 8 * textSize;
    if (w > SCREEN_WIDTH) {
      textSize = 2;
      w = strlen(displayWord) * 6 * textSize;
      h = 8 * textSize;
    }
    display.setTextSize(textSize);

    int drawX = (SCREEN_WIDTH - w) / 2 + offsetX;
    int drawY = (SCREEN_HEIGHT - h) / 2 + offsetY;

    display.fillRoundRect(drawX - 6, drawY - 5, w + 12, h + 10, 3, BLACK);
    display.drawRoundRect(drawX - 6, drawY - 5, w + 12, h + 10, 3, WHITE);
    display.setTextColor(WHITE);
    display.setCursor(drawX, drawY);
    display.print(displayWord);

    if (showCursor && ((millis() / 150) % 2 == 0))
      display.fillRect(drawX + w + 2, drawY, 2, h, WHITE);
  }

  display.invertDisplay(invertScreen);
  display.display();
}

// ---------- Lightning ----------
void generateBolt(int idx) {
  bolts[idx].numPts = MAX_BOLT_PTS;
  bolts[idx].bx[0] = SCREEN_WIDTH / 2 + random(-25, 26);
  bolts[idx].by[0] = 0;
  for (int i = 1; i < MAX_BOLT_PTS; i++) {
    bolts[idx].by[i] = bolts[idx].by[i - 1] + (SCREEN_HEIGHT / (MAX_BOLT_PTS - 1));
    bolts[idx].bx[i] = bolts[idx].bx[i - 1] + random(-14, 15);
    if (bolts[idx].bx[i] < 3) bolts[idx].bx[i] = 3;
    if (bolts[idx].bx[i] > SCREEN_WIDTH - 3) bolts[idx].bx[i] = SCREEN_WIDTH - 3;
  }
  bolts[idx].framesLeft = random(3, 8);
  bolts[idx].active = true;
}

void drawLightning(Effect eff, bool lyricActive) {
  int spawnChance = lyricActive ? 35 : 8;
  if (eff == EFFECT_SHAKE || eff == EFFECT_INVERT || eff == EFFECT_ZOOM_IN) spawnChance = 65;
  if (random(100) < spawnChance)
    for (int i = 0; i < MAX_BOLTS; i++)
      if (!bolts[i].active) { generateBolt(i); break; }

  for (int i = 0; i < MAX_BOLTS; i++) {
    if (!bolts[i].active) continue;
    for (int j = 0; j < bolts[i].numPts - 1; j++) {
      display.drawLine(bolts[i].bx[j], bolts[i].by[j], bolts[i].bx[j + 1], bolts[i].by[j + 1], WHITE);
      display.drawLine(bolts[i].bx[j] + 1, bolts[i].by[j], bolts[i].bx[j + 1] + 1, bolts[i].by[j + 1], WHITE);
    }
    if (bolts[i].numPts > 4) {
      int bi = random(2, bolts[i].numPts - 2);
      display.drawLine(bolts[i].bx[bi], bolts[i].by[bi],
                       bolts[i].bx[bi] + random(-22, 23), bolts[i].by[bi] + random(5, 15), WHITE);
    }
    if (--bolts[i].framesLeft <= 0) bolts[i].active = false;
  }

  int sparkles = lyricActive ? 8 : 3;
  for (int i = 0; i < sparkles; i++)
    display.drawPixel(random(SCREEN_WIDTH), random(SCREEN_HEIGHT), WHITE);
}

// ---------- 3D Cube ----------
void drawCube(Effect eff, bool lyricActive) {
  float sm = lyricActive ? 1.5 : 0.5;
  if (eff == EFFECT_ZOOM_IN) sm = 5.0;
  else if (eff == EFFECT_SHAKE || eff == EFFECT_INVERT) sm = 3.0;
  else if (eff == EFFECT_BOUNCE || eff == EFFECT_POP) sm = 2.0;

  cubeAX += 0.02 * sm; cubeAY += 0.03 * sm; cubeAZ += 0.01 * sm;

  float sz = (eff == EFFECT_POP || eff == EFFECT_BOUNCE) ? 22.0 : 18.0;
  int cx = SCREEN_WIDTH / 2, cy = SCREEN_HEIGHT / 2;

  const int8_t verts[8][3] = {
    {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
    {-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}
  };
  int proj[8][2];
  for (int i = 0; i < 8; i++) {
    float x = verts[i][0] * sz, y = verts[i][1] * sz, z = verts[i][2] * sz;
    float y1 = y * cos(cubeAX) - z * sin(cubeAX);
    float z1 = y * sin(cubeAX) + z * cos(cubeAX); y = y1; z = z1;
    float x1 = x * cos(cubeAY) + z * sin(cubeAY);
    z1 = -x * sin(cubeAY) + z * cos(cubeAY); x = x1; z = z1;
    x1 = x * cos(cubeAZ) - y * sin(cubeAZ);
    y1 = x * sin(cubeAZ) + y * cos(cubeAZ); x = x1; y = y1;
    float scale = 60.0 / (60.0 + z);
    proj[i][0] = cx + (int)(x * scale);
    proj[i][1] = cy + (int)(y * scale);
  }
  const uint8_t edges[12][2] = {
    {0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}
  };
  for (int i = 0; i < 12; i++)
    display.drawLine(proj[edges[i][0]][0], proj[edges[i][0]][1],
                     proj[edges[i][1]][0], proj[edges[i][1]][1], WHITE);
  for (int i = 0; i < 8; i++)
    display.fillCircle(proj[i][0], proj[i][1], 2, WHITE);
}