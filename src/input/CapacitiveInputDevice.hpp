#include <vector>;
#include <string.h>;
#include <Arduino.h>;

using namespace std;

class CapacitiveInputDeviceArrayBase {
    public:
        virtual bool poll();
        virtual int getPressedButton();
        virtual vector<vector<int>> getPressedButtons();
};

class TTP229Array: public CapacitiveInputDeviceArrayBase {
    private:
        int sclPin;
        vector<int> sdoPins;
        vector<vector<int>> pressedButtons;

    public:
        TTP229Array(int sclPin, vector<int> sdoPins) {
            // https://electropeak.com/learn/interfacing-16-channel-ttp229-capacitive-touch-16-key-keypad-with-arduino/
            pinMode(sclPin, OUTPUT);
            for (int sdoPin: sdoPins) {  
                pinMode(sdoPin, INPUT);
            } 
        }

        bool poll() override {
            // TODO: Test me!
            for (vector<int> iPressedButtons: pressedButtons) {
                if (!iPressedButtons.empty()) {
                    iPressedButtons.clear();
                }
            }
            for (int i=0; i<16; i++) {
                digitalWrite(sclPin, LOW); 
                for (int j=0; j<sdoPins.size(); i++) {
                    if (!digitalRead(sdoPins[i])) {
                        pressedButtons[j].push_back(i);
                    }
                }
                digitalWrite(sclPin, HIGH);
            }
            return true;
        }

        /**
         * 
         */
        int getPressedButton() override {
            // Get the first pressed button only
            for (vector<int> iPressedButtons: pressedButtons) {
                if (iPressedButtons.size() > 0) {
                    return iPressedButtons[0];
                }
            }
            return -1;
        }

        vector<vector<int>> getPressedButtons() override {
            return this->pressedButtons;
        }
};


class MPR121Array: public CapacitiveInputDeviceArrayBase {
    private:
        int sclPin;
        vector<int> sdoPins;
        vector<vector<int>> pressedButtons;

    public:
        MPR121Array(int sclPin, vector<int> sdoPins) {
        }

        bool poll() override {
        }

        int getPressedButton() override {
        }

        vector<vector<int>> getPressedButtons() override {
        }
};



/*********************************************************
This is a library for the MPR121 12-channel Capacitive touch sensor

Designed specifically to work with the MPR121 Breakout in the Adafruit shop 
  ----> https://www.adafruit.com/products/

These sensors use I2C communicate, at least 2 pins are required 
to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution
**********************************************************/

#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void setup() {
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

void loop() {
  // Get the currently touched pads
  currtouched = cap.touched();
  
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
    }
  }

  // reset our state
  lasttouched = currtouched;

  // comment out this line for detailed data from the sensor!
  return;
  
  // debugging info, what
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(cap.touched(), HEX);
  Serial.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();
  
  // put a delay so it isn't overwhelming
  delay(100);
}
