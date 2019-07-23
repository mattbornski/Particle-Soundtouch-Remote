#include "imagebutton.h"
#include "Adafruit_ILI9341.h"

Debouncer::Debouncer() {
  this->lastPeak.valid = false;
  this->lastValley.valid = false;
  this->returnedLastPeak = true;
}

void Debouncer::setPeak(TouchTracking t) {
  this->lastPeak.x = t.x;
  this->lastPeak.y = t.y;
  this->lastPeak.z = t.z;
  this->lastPeak.timestamp = t.timestamp;
  this->lastPeak.valid = t.valid;
  this->returnedLastPeak = false;

  this->lastValley.valid = false;

  Serial.print("Peak at ");
  Serial.print(this->lastPeak.x);
  Serial.print(", ");
  Serial.println(this->lastPeak.y);
  Serial.print("Debouncer this = ");
  Serial.println((void *)this);
  Serial.print("Debouncer this->returnedLastPeak = ");
  Serial.println(this->returnedLastPeak);
}

void Debouncer::setValley(TouchTracking t) {
  Serial.print("Valley at ");
  Serial.print(t.x);
  Serial.print(", ");
  Serial.println(t.y);
  this->lastValley.x = t.x;
  this->lastValley.y = t.y;
  this->lastValley.z = t.z;
  this->lastValley.timestamp = t.timestamp;
  this->lastValley.valid = t.valid;
}

void Debouncer::input(TouchTracking t) {
  if (!t.valid) {
    return;
  }

  if (!this->lastPeak.valid) {
    this->setPeak(t);
    return;
  }

  if (t.z <= this->lastPeak.z) {
    this->setPeak(t);
    return;
  }

  if (this->returnedLastPeak && (abs(t.x - this->lastPeak.x) > 5) && (abs(t.y - this->lastPeak.y) > 5)) {
    this->setPeak(t);
    return;
  }

  if (!this->lastValley.valid) {
    this->setValley(t);
    return;
  }

  if (t.z >= this->lastValley.z) {
    this->setValley(t);
    return;
  }
}

TouchTracking Debouncer::output() {
  Serial.println("gathering output point");
  Serial.print("Debouncer this = ");
  Serial.println((void *)this);
  Serial.print("Debouncer this->returnedLastPeak = ");
  Serial.println(this->returnedLastPeak);
  if (!(this->returnedLastPeak)) {
    Serial.print("Returning point at ");
    Serial.print(this->lastPeak.x);
    Serial.print(", ");
    Serial.println(this->lastPeak.y);
    this->returnedLastPeak = true;
    return this->lastPeak;
  } else {
    TouchTracking t;
    t.valid = false;
    return t;
  }
}



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

void PlayButton::perform(TouchTracking t) {
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

void PauseButton::perform(TouchTracking t) {
    Serial.println("pause!");
    if (this->speaker != NULL) {
      this->speaker->pause();
    }
}