#include <Arduino.h>
#include "display/GrayscaleDisplay.hpp"

Nokia5110GrayscaleDisplay * displayInst;

void setup() {
    displayInst = new Nokia5110GrayscaleDisplay();
}

void loop() {
    displayInst->poll();
}

