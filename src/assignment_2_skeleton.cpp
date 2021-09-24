#include "Arduino.h"
#include "display/GrayscaleDisplay.hpp"

Nokia5110GrayscaleDisplay * displayInst;

void setup() {
    displayInst = new Nokia5110GrayscaleDisplay(15, 2, 4, 23, 18);

    for (int x=0; x<84; x++) {
        for (int y=0; y<48; y++) {
            displayInst->putPixel(x, y, y/16);
        }
    }
    
}

void loop() {
    displayInst->poll();
}

