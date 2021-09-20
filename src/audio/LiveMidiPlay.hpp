#include <FS.h>;
#include <SPI.h>;
#include <stdio.h>;
#include <stdlib.h>;
#include <stdint.h>;
#include <string.h>;
#include <SPIFFS.h>;
#include "ESPVS1003/ESPVS1003/ESPVS1003.h";
#include "Adafruit_VS1053.h";

// If you're using the VS1053 breakout:
// Don't forget to connect the GPIO #0 to GROUND and GPIO #1 pin to 3.3V
// If you're using the Music Maker shield:
// Don't forget to connect the GPIO #1 pin to 3.3V and the RX pin to digital #2

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79
#define VS1053_GM1_OCARINA 80

#define MIDI_CHAN_MSG 0xB0

class VS1053MidiPlay {
    private:
        #if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
            #include <SoftwareSerial.h>
            SoftwareSerial VS1053_MIDI(0, 2); // TX only, do not use the 'rx' side
        #else
            // on a Mega/Leonardo you may have to change the pin to one that 
            // software serial support uses OR use a hardware serial port!
            HardwareSerial VS1053_MIDI = Serial1;
        #endif
        
    public:
        VS1053MidiPlay(int rxPin=2, int resetPin=9) {
            VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'

            pinMode(resetPin, OUTPUT);
            digitalWrite(resetPin, LOW);
            delay(10);
            digitalWrite(resetPin, HIGH);
            delay(10);

            midiSetChannelBank(0, VS1053_BANK_MELODY);
            midiSetInstrument(0, VS1053_GM1_OCARINA);
            midiSetChannelVolume(0, 127);
        }
        
        void midiSetInstrument(uint8_t chan, uint8_t inst) {
            if (chan > 15) return;
            inst --; // page 32 has instruments starting with 1 not 0 :(
            if (inst > 127) return;

            VS1053_MIDI.write(0xC0 | chan);  
            VS1053_MIDI.write(inst);
        }

        void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
            if (chan > 15) return;
            if (vol > 127) return;

            VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
            VS1053_MIDI.write(0x07);
            VS1053_MIDI.write(vol);
        }

        void midiSetChannelBank(uint8_t chan, uint8_t bank) {
            if (chan > 15) return;
            if (bank > 127) return;

            VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
            VS1053_MIDI.write((uint8_t)0x00);
            VS1053_MIDI.write(bank);
        }

        void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
            if (chan > 15) return;
            if (n > 127) return;
            if (vel > 127) return;

            VS1053_MIDI.write(0x90 | chan);
            VS1053_MIDI.write(n);
            VS1053_MIDI.write(vel);
        }

        void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
            if (chan > 15) return;
            if (n > 127) return;
            if (vel > 127) return;

            VS1053_MIDI.write(0x80 | chan);
            VS1053_MIDI.write(n);
            VS1053_MIDI.write(vel);
        }

        void loop() {  
            for (uint8_t i=60; i<69; i++) {
                midiNoteOn(0, i, 127);
                delay(100);
                midiNoteOff(0, i, 127);
            }
            
            delay(1000);
        }
};
