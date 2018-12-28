#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT.h"
#include "speaker.h"

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
  virtual void perform() = 0;
};

class PauseButton : public ImageButton {
protected:
  Speaker *speaker;

public:
    PauseButton(Speaker *speaker);
    ~PauseButton();

    void perform();
};

class PlayButton : public ImageButton {
protected:
  Speaker *speaker;

public:
    PlayButton(Speaker *speaker);
    ~PlayButton();

    void perform();
};
