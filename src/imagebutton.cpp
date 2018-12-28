#include "imagebutton.h"
#include "Adafruit_ILI9341.h"

ImageButton::ImageButton(uint16_t width, uint16_t height) {
    this->alloc(width, height);
}

ImageButton::ImageButton() {
    this->canvas = NULL;
}

ImageButton::~ImageButton() {
    this->dealloc();
}

void ImageButton::dealloc() {
    if (this->canvas != NULL) {
        delete this->canvas;
    }
}

void ImageButton::alloc(uint16_t width, uint16_t height) {
    this->canvas = new GFXcanvas16(width, height);
    this->width = width;
    this->height = height;
    this->drawn = false;
}

void ImageButton::draw(Adafruit_SPITFT &display, int16_t x, int16_t y) {
    this->x = x;
    this->y = y;
    this->drawn = true;
    display.drawRGBBitmap(
        x,
        y,
        this->canvas->getBuffer(),
        this->canvas->width(),
        this->canvas->height()
    );
}

bool ImageButton::contains(int16_t x, int16_t y) {
  return ((x >= this->x) && (x < (int16_t) (this->x + this->width)) &&
          (y >= this->y) && (y < (int16_t) (this->y + this->height)));
}

PlayButton::PlayButton(Speaker *speaker) {
    this->alloc(32, 32);
    this->speaker = speaker;
    this->canvas->fillTriangle(2, 0, 2, 32, 30, 16, ILI9341_WHITE);
}

PlayButton::~PlayButton() {
  this->speaker = NULL;
  this->dealloc();
}

void PlayButton::perform() {
    Serial.println("play!");
    if (this->speaker != NULL) {
      this->speaker->play();
    }
}

PauseButton::PauseButton(Speaker *speaker) {
    this->alloc(32, 32);
    this->speaker = speaker;
    this->canvas->fillRect(4, 0, 8, 32, ILI9341_WHITE);
    this->canvas->fillRect(20, 0, 8, 32, ILI9341_WHITE);
}

PauseButton::~PauseButton() {
  this->speaker = NULL;
  this->dealloc();
}

void PauseButton::perform() {
    Serial.println("pause!");
    if (this->speaker != NULL) {
      this->speaker->pause();
    }
}