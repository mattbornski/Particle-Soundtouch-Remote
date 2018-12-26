#include "imagebutton.h"
#include "Adafruit_ILI9341.h"

ImageButton::ImageButton(uint16_t width, uint16_t height) {
    this->alloc(width, height);
}

ImageButton::ImageButton() {
    this->canvas = NULL;
}

ImageButton::~ImageButton() {
  dealloc();
}

void ImageButton::dealloc() {
    if (this->canvas != NULL) {
        delete this->canvas;
    }
}

void ImageButton::alloc(uint16_t width, uint16_t height) {
    this->canvas = new GFXcanvas16(width, height);
}

void ImageButton::draw(Adafruit_SPITFT &display, int16_t x, int16_t y) {
     display.drawRGBBitmap(
        x,
        y,
        this->canvas->getBuffer(),
        this->canvas->width(),
        this->canvas->height()
    );
}

PlayButton::PlayButton() {
    this->alloc(32, 32);
    this->canvas->fillTriangle(2, 0, 2, 32, 30, 16, ILI9341_WHITE);
}

PauseButton::PauseButton() {
    this->alloc(32, 32);
    this->canvas->fillRect(4, 0, 8, 32, ILI9341_WHITE);
    this->canvas->fillRect(20, 0, 8, 32, ILI9341_WHITE);
}