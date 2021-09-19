#include <string.h>;
#include <Arduino.h>;

using namespace std;

enum Shades: unsigned char {
  OFF = 255,
  VERY_LIGHT = 224,
  LIGHT = 192,
  MEDIUM = 128,
  DARK = 64,
  VERY_DARK = 32,
  FULL = 0
};

class ColorObject {
  private:
    unsigned char r, g, b;
  
  public:
    ColorObject(char r, char g, char b) {
      this->r = r;
      this->g = g;
      this->b = b;
    }

    String toString() {
      return ("rgb(" + 
        String(this->r) + ", " +
        String(this->g) + ", " +
        String(this->b) + ", " +
      ")");
    }
};

class Colors {
  const ColorObject* RED = new ColorObject(255, 0, 0);
  const ColorObject* GREEN = new ColorObject(0, 255, 0);
  const ColorObject* BLUE = new ColorObject(0, 0, 255);
  const ColorObject* CYAN = new ColorObject(0, 255, 255);
  const ColorObject* MAGENTA = new ColorObject(255, 0, 255);
  const ColorObject* YELLOW = new ColorObject(255, 255, 0);
};
