#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT.h"

class ImageButton {
protected:
  GFXcanvas16 *canvas;

public:
  ImageButton();
  ImageButton(uint16_t width, uint16_t height);
  ~ImageButton();

  void alloc(uint16_t width, uint16_t height);
  void dealloc();
  void draw(Adafruit_SPITFT &display, int16_t x, int16_t y);
};

class PauseButton : public ImageButton {
public:
    PauseButton();
};

class PlayButton : public ImageButton {
public:
    PlayButton();
};
