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

#include <Arduino.h>

#define LCD_C LOW
#define LCD_D HIGH

#define LCD_W 84
#define LCD_H 48

class Nokia5110GrayscaleDisplay {
    private:
        int scePin; // Chip Select
        int resetPin; // Reset
        int dcPin; // Mode Select
        int sdinPin; // Data In
        int sclkPin; // Clock

        byte g_bitmap[504 * 32];
        size_t g_bitmap_len = 5544;

    public:
        Nokia5110GrayscaleDisplay(int scePin=7, int resetPin=6, int dcPin=5, int sdinPin=4, int sclkPin=3) {
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

            digitalWrite(resetPin, LOW);
            delay(10);
            digitalWrite(resetPin, HIGH);

            // See page 14 of https://sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
            writeRegister(LCD_C, 0b100001);   // LCD Extended Commands.
            writeRegister(LCD_C, 0b10111000); // Set LCD Vop (Contrast).
            writeRegister(LCD_C, 0b110);      // Set Temp coefficent. //0x04
            writeRegister(LCD_C, 0x14);       // LCD bias mode 1:48. //0x13
            writeRegister(LCD_C, 0b100010);   // LCD Basic Commands
            writeRegister(LCD_C, 0b1101);     // LCD in normal mode.
        }

        bool poll() {
            // Move cursor to 0,0
            writeRegister(LCD_C, 0b01000000);  // X
            writeRegister(LCD_C, 0b10000000);  // Y

            // Write data to LCD
            digitalWrite(dcPin, LCD_D);
            digitalWrite(scePin, LOW);
            for (size_t i = 0; i < g_bitmap_len; i += 504) {
                for (int j = 0; j < 504; ++j) {
                    shiftOut(sdinPin, sclkPin, MSBFIRST, g_bitmap[i+j]);
                }
            }
            digitalWrite(scePin, HIGH);
            return true;
        }

        inline void writeRegister(byte dc, byte data) {
            digitalWrite(dcPin, dc);
            digitalWrite(scePin, LOW);
            shiftOut(sdinPin, sclkPin, MSBFIRST, data);
            digitalWrite(scePin, HIGH);
        }

        inline void putPixel(int x, int y, byte amount) {
            int count = 0;
            long j = (y * LCD_H) + x;
            for (size_t i = 0; i < g_bitmap_len; i += 504) {
                g_bitmap[i + j] = count >= amount ? 1 : 0; 
                count++;
            }
        }
};
