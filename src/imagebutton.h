#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT.h"
#include "speaker.h"

struct TouchTracking {
  bool valid;
  uint16_t x;
  uint16_t y;
  uint8_t z;
  uint32_t timestamp;
};



class Debouncer {
protected:
  bool returnedLastPeak;
  TouchTracking lastPeak;
  TouchTracking lastValley;

  void setPeak(TouchTracking t);
  void setValley(TouchTracking t);

public:
  Debouncer();

  void input(TouchTracking t);
  TouchTracking output();
  bool loop();
};



class ImageButton {
protected:
  GFXcanvas16 *canvas;

  bool drawn;
  uint16_t width;
  uint16_t height;
  uint16_t x;
  uint16_t y;

public:
  ImageButton();
  ImageButton(uint16_t width, uint16_t height);
  ~ImageButton();

  void alloc(uint16_t width, uint16_t height);
  void dealloc();
  void draw(Adafruit_SPITFT &display, int16_t x, int16_t y);

  bool contains(int16_t x, int16_t y);
  virtual void perform(TouchTracking t) = 0;
};

class PauseButton : public ImageButton {
protected:
  Speaker *speaker;

public:
    PauseButton(Speaker *speaker);
    ~PauseButton();

    void perform(TouchTracking t);
};

class PlayButton : public ImageButton {
protected:
  Speaker *speaker;

public:
    PlayButton(Speaker *speaker);
    ~PlayButton();

    void perform(TouchTracking t);
};
