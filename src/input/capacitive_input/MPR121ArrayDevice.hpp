#include <Wire.h>
#include <vector>;
#include <string.h>;
#include <Arduino.h>;
#include "Adafruit_MPR121.h"
#include "CapacitiveInputDeviceArrayBase.hpp"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

class MPR121ArrayDevice: public CapacitiveInputDeviceArrayBase {
    private:
        int sclPin;
        vector<int> sdoPins;
        vector<vector<int>> pressedButtons;
        Adafruit_MPR121 cap;

        // Keeps track of the last pins touched
        // so we know when buttons are 'released'
        uint16_t lasttouched = 0;
        uint16_t currtouched = 0;

    public:
        MPR121ArrayDevice(int sclPin, vector<int> sdoPins) {
            // You can have up to 4 on one i2c bus but one is enough for testing!
            cap = Adafruit_MPR121();

            // Default address is 0x5A, if tied to 3.3V its 0x5B
            // If tied to SDA its 0x5C and if SCL then 0x5D
            if (!cap.begin(0x5A)) {
              Serial.println("MPR121 not found, check wiring?");
              while (1);
            }
            Serial.println("MPR121 found!");
        }

        bool poll() override {
            return true;  // FIXME!
        }

        int getPressedButton() override {
            return 0;  // FIXME!
        }

        vector<vector<int>> getPressedButtons() override {
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

            return pressedButtons;
        }
};
