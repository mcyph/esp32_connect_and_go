#include <Wire.h>;
#include <Arduino.h>;
#include "FontObject.hpp";

class DisplayDeviceBase {
  public:
    virtual bool poll() = 0;
    
    virtual int drawPixel(int x, int y) = 0;
    virtual int drawLine(int x1, int x2, int y1, int y2) = 0;
    virtual int drawBox(int x1, int y1, int x2, int y2) = 0;
    virtual int drawText(FontObject font, int x, int y) = 0;
    virtual int drawImage() = 0;
};

class ColorDisplayDevice: DisplayDeviceBase {
  public:
    virtual int setBackgroundColor(Color color) = 0;
    virtual int setForegroundColor(Color color) = 0;
};


class GrayscaleDisplayDevice: DisplayDeviceBase {
  public:
    virtual int setBackgroundShade() = 0;
    virtual int setForegroundShade() = 0;
};

class PWMMonoDisplayDevice: GrayscaleDisplayDevice {
  public:
    bool poll() {
    }
};

class VGAOutput: ColorDisplayDevice {

};
