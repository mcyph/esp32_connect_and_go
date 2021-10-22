// Originally from https://github.com/Grumbel/grayscale5110/blob/master/grayscale5110.ino
// with modifications by Dave Morrissey https://github.com/mcyph
// 
// Copyright (C) 2017 Ingo Ruhnke <grumbel@gmail.com>
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include <SPI.h>
#include <Arduino.h>

#define LCD_C LOW
#define LCD_D HIGH

#define LCD_W 84
#define LCD_H 48
#define LCD_PIXELS 4032

#define NORMAL_MODE 0x0C
#define INVERSE_MODE 0x0D

#define BRIGHTNESS_STEPS 2

class Nokia5110GrayscaleDisplay {
    private:
        SPIClass spiInst = SPIClass(HSPI);

        int scePin; // Chip Select
        int resetPin; // Reset
        int dcPin; // Mode Select
        int sdinPin; // Data In
        int sclkPin; // Clock

        byte g_bitmap[BRIGHTNESS_STEPS][LCD_PIXELS/8];

    public:
        Nokia5110GrayscaleDisplay(int scePin, int resetPin, int dcPin, int sdinPin, int sclkPin) {
            this->scePin = scePin;
            this->resetPin = resetPin;
            this->dcPin = dcPin;
            this->sdinPin = sdinPin;
            this->sclkPin = sclkPin;

            pinMode(scePin, OUTPUT);
            pinMode(resetPin, OUTPUT);
            pinMode(dcPin, OUTPUT);
            pinMode(sdinPin, OUTPUT);
            pinMode(sclkPin, OUTPUT);

            digitalWrite(scePin, LOW);
            digitalWrite(resetPin, LOW);
            delay(1);
            digitalWrite(resetPin, HIGH);
            digitalWrite(scePin, HIGH);

            // See page 14 of https://sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
            setContrast(50);
            setTemperatureCoefficient();
            setLCDBiasMode(5);
            setLCDDisplayMode(4);
        }

        /**********************************************************************
         * Drawing functions
         **********************************************************************/

        bool poll() {
            for (int i=0; i<BRIGHTNESS_STEPS; i++) {
                pollStep(i);
            }
            return true;
        }

        int getBrightnessSteps() {
            return BRIGHTNESS_STEPS;
        }

        inline void pollStep(byte brightnessStep) {
            acquireSPI();

            // Move cursor to the start of the current line
            setCursorPos(0, 0);

            // Write data to LCD
            digitalWrite(dcPin, LCD_D);
            digitalWrite(scePin, LOW);
            
            for (int j=0; j<LCD_PIXELS/8; j++) {
                // Can't send as a buffer for some reason, as the module doesn't display 
                // anything regardless of whether the divider is changed to 128 or not
                // Still faster/less flickery with the hardware SPI than software
                SPI.transfer(g_bitmap[brightnessStep][j]);
                //shiftOut(sdinPin, sclkPin, MSBFIRST, g_bitmap[i][j]);
                
                // Add a bit of randomness to reduce the visible flicker
                // The random() function potentially being executed multiple times 
                // is deliberate to increase the effect exponentially
                for (int X=0; X<random(1, 8); X++) {}
            }
            digitalWrite(scePin, HIGH);

            // HACK!
            writeRegister(LCD_C, 0x40 | 0);  // Y
        }

        inline void putPixel(int x, int y, byte amount) {
            // Logic adapted from https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library/blob/master/Adafruit_PCD8544.cpp
            for (int i=0; i<BRIGHTNESS_STEPS; i++) {
                if ((x % 2 == 0 && amount > i) || (x % 2 != 0 && amount > BRIGHTNESS_STEPS-1-i)) {
                    g_bitmap[i][x + (y / 8) * LCD_W] |= 1 << (y % 8);
                } else {
                    g_bitmap[i][x + (y / 8) * LCD_W] &= ~(1 << (y % 8));
                }
            }
        }

        /**********************************************************************
         * Mode setters
         **********************************************************************/

        void acquireSPI() {
            SPI.begin(sclkPin, -1, sdinPin, scePin);
            SPI.setClockDivider(SPI_CLOCK_DIV16);
        }

        void setContrast(int amount) {
            writeRegister(LCD_C, 0x21);          // LCD Extended Commands.
            writeRegister(LCD_C, 0x80 | amount); // Set LCD Vop (Contrast).
        }

        void setTemperatureCoefficient() {
            writeRegister(LCD_C, 0x21);          // LCD Extended Commands.
            writeRegister(LCD_C, 0x04);          // Set Temp coefficent. //0x04
        }

        void setLCDBiasMode(int amount) {
            writeRegister(LCD_C, 0x21);          // LCD Extended Commands.
            writeRegister(LCD_C, 0x10 | amount); // LCD bias mode 1:48. //0x13, 0x14
        }

        void setLCDDisplayMode(int mode) {
            writeRegister(LCD_C, 0x20);           // LCD Basic Commands
            writeRegister(LCD_C, 0x08 | mode);    // LCD in normal/inverse mode.
        }

        inline void setCursorPos(int x, int y) {
            writeRegister(LCD_C, 0x80 | x);  // X
            writeRegister(LCD_C, 0x40 | y);  // Y
        }

        inline void setCursorXPos(int x) {
            writeRegister(LCD_C, 0x80 | x);
        }

        inline void setCursorYPos(int y) {
            writeRegister(LCD_C, 0x40 | y);
        }

        /**********************************************************************
         * Low-level operations
         **********************************************************************/

        inline void writeRegister(byte dc, byte data) {
            acquireSPI();
            digitalWrite(dcPin, dc);
            digitalWrite(scePin, LOW);
            SPI.transfer(data);
            //shiftOut(sdinPin, sclkPin, MSBFIRST, data);
            digitalWrite(scePin, HIGH);
        }
};
