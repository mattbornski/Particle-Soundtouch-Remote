#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_STMPE610.h"
#include "soundtouch.h"

#define MAX_TRACKING 256

struct TouchTracking {
  uint16_t x;
  uint16_t y;
  uint8_t z;
  uint32_t timestamp;
};


#define STMPE_CS D3
#define TFT_CS   D4
#define TFT_DC   D5
#define SD_CS    D2

Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 touchscreen = Adafruit_STMPE610(STMPE_CS);
SoundtouchClient soundtouchClient;
Speaker *office;



// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 3800
#define TS_MAXX 100
#define TS_MINY 100
#define TS_MAXY 3750

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

TouchTracking scale(TouchTracking t) {
  // Scale from ~0->4000 to display.width using the calibration #'s
  t.x = map(t.x, TS_MINX, TS_MAXX, 0, display.width());
  t.y = map(t.y, TS_MINY, TS_MAXY, 0, display.height());

  return t;
}


void setup() {
  Serial.begin(115200);

  delay(10);
  if (!touchscreen.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  
  display.begin();
  Serial.println("Display driver started");
}

void displayReset() {
  display.fillScreen(ILI9341_BLACK);
  
  // make the color selection boxes
  display.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
  display.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
  display.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
  display.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
  display.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
  display.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
 
  // select the current color 'red'
  display.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
  currentcolor = ILI9341_RED;
}

void soundtouchLoop() {
  if (office == NULL) {
    office = soundtouchClient.discoverWithCache("Office");
    if (office != NULL) {
      displayReset();
      touchscreen.writeRegister8(STMPE_INT_STA, 0xFF);

      display.setTextSize(3);
      display.setTextColor(ILI9341_RED);
      display.setCursor(10, 10);
      display.println("Office OK");
    }
  }
}

bool draw(TouchTracking t) {
  Serial.print("X = ");
  Serial.print(t.x);
  Serial.print("\tY = ");
  Serial.print(t.y);
  Serial.print("\tPressure = ");
  Serial.println(t.z);

  if (t.y < BOXSIZE) {
     oldcolor = currentcolor;

     if (t.x < BOXSIZE) { 
       currentcolor = ILI9341_RED; 
       display.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (t.x < BOXSIZE*2) {
       currentcolor = ILI9341_YELLOW;
       display.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (t.x < BOXSIZE*3) {
       currentcolor = ILI9341_GREEN;
       display.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (t.x < BOXSIZE*4) {
       currentcolor = ILI9341_CYAN;
       display.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (t.x < BOXSIZE*5) {
       currentcolor = ILI9341_BLUE;
       display.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     } else if (t.x < BOXSIZE*6) {
       currentcolor = ILI9341_MAGENTA;
       display.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
     }

     if (oldcolor != currentcolor) {
        if (oldcolor == ILI9341_RED) 
          display.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
        if (oldcolor == ILI9341_YELLOW) 
          display.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
        if (oldcolor == ILI9341_GREEN) 
          display.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
        if (oldcolor == ILI9341_CYAN) 
          display.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
        if (oldcolor == ILI9341_BLUE) 
          display.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
        if (oldcolor == ILI9341_MAGENTA) 
          display.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
     }

     return false;
  }

  if (((t.y-PENRADIUS) > 0) && ((t.y+PENRADIUS) < display.height())) {
    display.fillCircle(t.x, t.y, PENRADIUS, currentcolor);
    return true;
  }

  return false;
}

TouchTracking tracking[MAX_TRACKING];
uint8_t trackingLru = 1;
uint8_t trackingMru = 0;
uint32_t trackingTtl = 3500;
void uiLoop() {
  uint32_t now = millis();

  for (int i = trackingLru; i <= trackingMru; i++) {
    TouchTracking t = tracking[i % MAX_TRACKING];
    if (t.timestamp + trackingTtl < now) {
      display.fillCircle(t.x, t.y, PENRADIUS, ILI9341_BLACK);
      trackingLru++;
    }
  }

  if (touchscreen.touched()) {
    while (!touchscreen.bufferEmpty()) {
      TouchTracking t;
      touchscreen.readData(&t.x, &t.y, &t.z);
      t.timestamp = now;
      t = scale(t);

      if (draw(t)) {
        tracking[++trackingMru % MAX_TRACKING] = t;
      }
    }
    // reset touch registers
    touchscreen.writeRegister8(STMPE_INT_STA, 0xFF);
  }
}

/*static bool inSpinner = false;
static uint8_t spinnerPhase = 0;
static uint32_t lastPhaseAdvanced = 0;
void spinnerLoop() {
  if (!inSpinner) {
    inSpinner = true;
    display.fillCircle(100, 100, 5, ILI9341_DARKGREY);
  }
}*/

void loop() {
  soundtouchLoop();
  uiLoop();
}
