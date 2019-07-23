#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_STMPE610.h"
#include "soundtouch.h"
#include "imagebutton.h"


#define STMPE_CS D3
#define TFT_CS   D4
#define TFT_DC   D5
#define SD_CS    D2

Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 touchscreen = Adafruit_STMPE610(STMPE_CS);
Debouncer debouncer;
SoundtouchClient soundtouchClient;
Speaker *office;



// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 3800
#define TS_MAXX 100
#define TS_MINY 100
#define TS_MAXY 3750

TouchTracking scale(TouchTracking t) {
  // Scale from ~0->4000 to display.width using the calibration #'s
  t.x = map(t.x, TS_MINX, TS_MAXX, 0, display.width());
  t.y = map(t.y, TS_MINY, TS_MAXY, 0, display.height());

  return t;
}



void setup() {
  Serial.begin(115200);

  SoundtouchClient::setDebug(3);

  delay(10);
  if (!touchscreen.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  
  display.begin();
  Serial.println("Display driver started");

  displayReset();
}

void displayReset() {
  display.fillScreen(ILI9341_BLACK);
}

ImageButton *availableActions[] = { NULL, NULL, NULL, NULL, NULL };
void displaySpeaker(Speaker *speaker) {
  display.setTextSize(2);
  display.setTextColor(ILI9341_RED);
  display.setCursor(20, 10);
  if (office != NULL) {
    if (office->online) {
      display.println("Office Ready");
    } else {
      display.println("Office Offline");
    }
  } else {
    display.println("Office not found");
  }

  for (ImageButton *action : availableActions) {
    if (action != NULL) {
      delete action;
    }
  }

  if (office != NULL) {
    if (office->playing) {
      availableActions[0] = new PauseButton(office);
    } else {
      availableActions[0] = new PlayButton(office);
    }
  }

  int i = 0;
  for (ImageButton *action : availableActions) {
    i++;
    if (action != NULL) {
      action->draw(display, i * 50, 50);
    }
  }
}

void soundtouchLoop() {
  if (office == NULL) {
    office = soundtouchClient.discoverSpeakerWithCache("Office");
    
    if (office != NULL) {
      displaySpeaker(office);
      touchscreen.writeRegister8(STMPE_INT_STA, 0xFF);
    }
  }
}

bool handleAction(TouchTracking t) {
  if (!t.valid) {
    return false;
  }

  for (ImageButton *action : availableActions) {
    if (action != NULL && action->contains(t.x, t.y)) {
      action->perform(t);
      return true;
    }
  }

  return false;
}

bool debouncerLoop(Debouncer debouncer, Adafruit_STMPE610 touchscreen) {
  if (touchscreen.touched()) {
    while (!touchscreen.bufferEmpty()) {
      uint32_t now = millis();
      TouchTracking t;
      touchscreen.readData(&t.x, &t.y, &t.z);
      t.timestamp = now;
      t.valid = true;
      t = scale(t);
      Serial.print("Touch at ");
      Serial.print(t.x);
      Serial.print(", ");
      Serial.println(t.y);

      debouncer.input(t);

      return true;
    }
  }

  // reset touch registers
  touchscreen.writeRegister8(STMPE_INT_STA, 0xFF);
  return false;
}

void uiLoop() {
  while (debouncerLoop(debouncer, touchscreen)) {
    handleAction(debouncer.output());
  }

  handleAction(debouncer.output());
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

uint32_t lastDebug = 0;
void debugLoop() {
  uint32_t now = millis();
  if (now - lastDebug > 30000) {
    Serial.print("Heap free memory high water mark: ");
    Serial.println(System.freeMemory());
    lastDebug = now;
  }
}

void loop() {
  debugLoop();
  soundtouchLoop();
  uiLoop();
}
