#include <FS.h>;
#include <SPI.h>;
#include <stdio.h>;
#include <stdlib.h>;
#include <stdint.h>;
#include <string.h>;
#include <SPIFFS.h>;
#include "Adafruit_VS1053.h";
#include "ESPVS1003/ESPVS1003/ESPVS1003.h";

class VS1053AudioPlay {
    private:
        VS1003 player;
    
    public:
        VS1053AudioPlay(
            int csPin=27, int dcsPin=0, int dreqPin=25, int resetPin=26,
            int sckPin=14, int misoPin=12, int mosiPin=13
        ) {
            player = VS1003(csPin, dcsPin, dreqPin, resetPin);
            SPI.begin(sckPin, misoPin, mosiPin);
            player.begin();
            player.setVolume(0x10);
            SPIFFS.begin();
        };

        void playFile(String path, int bufferSize=128) {
            File fp = SPIFFS.open(path);
            if (!fp) {
                Serial.println("Open file error.");
                delay(100);
                return;
            }

            player.startSong();
            uint8_t buffer[bufferSize];

            while (fp.position() < fp.size()) {
                uint8_t nread = fp.readBytes((char *)buffer, bufferSize);
                player.playChunk(buffer, nread);
            }

            player.stopSong();
            fp.close();
        }

        void playRadio(String channelId) {
            // TODO!
        }

        void scanChannels() {
            // TODO!
        }
};
