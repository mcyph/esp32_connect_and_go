#include <FS.h>;
#include <SPI.h>;
#include <stdio.h>;
#include <stdlib.h>;
#include <stdint.h>;
#include <string.h>;
#include <SPIFFS.h>;
#include "Adafruit_VS1053.h";
//#include "../../lib/ESPVS1003/ESPVS1003.h";

#define RECBUFFSIZE 128  // 64 or 128 bytes.

String getNextOggPath() {
    // Check if the file exists already
    char filename[15];
    strcpy(filename, "RECORD00.OGG");

    for (uint8_t i = 0; i < 100; i++) {
        filename[6] = '0' + i/10;
        filename[7] = '0' + i%10;
        // create if does not exist, do not open existing, write, sync after write
        if (!SD.exists(filename)) {
            break;
        }
    }
    return String(filename);
};

class AdafruitVS1053OggRecord {
    private:
        String path;
        File recording;  // the file we will save our recording to
        uint8_t recording_buffer[RECBUFFSIZE];
        Adafruit_VS1053_FilePlayer musicPlayer;

    public:
        AdafruitVS1053OggRecord(
            String path=getNextOggPath(), 
            int resetPin=9, int csPin=10, int dcsPin=8, 
            int cardCSPin=A0, int dReqPin=3, 
            bool useMic=true
        ) {
            this->path = path;
            musicPlayer = Adafruit_VS1053_FilePlayer(resetPin, csPin, dcsPin, dReqPin, cardCSPin);
            
            recording = SD.open(path.c_str(), FILE_WRITE);
            if (! recording) {
                Serial.println("Couldn't open file to record!");
                while (1);
            }
            
            // use microphone (for linein, pass in 'false')
            musicPlayer.startRecordOgg(useMic);
        };

        bool poll() {  
            return !!saveRecordedData();
        }

        uint16_t saveRecordedData() {
            uint16_t written = 0;
            uint16_t wordswaiting = musicPlayer.recordedWordsWaiting();

            // try to process 256 words (512 bytes) at a time, for best speed
            while (wordswaiting > 256) {
                // for example 128 bytes x 4 loops = 512 bytes
                for (int x=0; x < 512/RECBUFFSIZE; x++) {
                    // fill the buffer!
                    for (uint16_t addr=0; addr < RECBUFFSIZE; addr+=2) {
                        uint16_t t = musicPlayer.recordedReadWord();
                        //Serial.println(t, HEX);
                        recording_buffer[addr] = t >> 8; 
                        recording_buffer[addr+1] = t;
                    }
                    if (! recording.write(recording_buffer, RECBUFFSIZE)) {
                        Serial.print("Couldn't write "); 
                        Serial.println(RECBUFFSIZE); 
                        while (1);
                    }
                }
                // flush 512 bytes at a time
                recording.flush();
                written += 256;
                wordswaiting -= 256;
            }
            return written;
        }

        uint16_t endRecording() {
            uint16_t written = 0;
            musicPlayer.stopRecordOgg();

            // Flush all the data, saving in chunks except 
            // for the last bit to improve performance
            saveRecordedData();
            
            uint16_t addr = 0;
            uint16_t wordsWaiting = musicPlayer.recordedWordsWaiting();

            for (int x=0; x < wordsWaiting-1; x++) {
                uint16_t t = musicPlayer.recordedReadWord();
                recording_buffer[addr] = t >> 8; 
                recording_buffer[addr+1] = t;
                if (addr > RECBUFFSIZE) {
                    if (! recording.write(recording_buffer, RECBUFFSIZE)) {
                        Serial.println("Couldn't write!");
                        while (1);
                    }
                    recording.flush();
                    addr = 0;
                }
            }

            if (addr != 0) {
                if (!recording.write(recording_buffer, addr)) {
                    Serial.println("Couldn't write!"); 
                    while (1);
                }
                written += addr;
            }
            musicPlayer.sciRead(VS1053_SCI_AICTRL3);

            if (!(musicPlayer.sciRead(VS1053_SCI_AICTRL3) & (1 << 2))) {
                recording.write(musicPlayer.recordedReadWord() & 0xFF);
                written++;
            }

            recording.flush();
            recording.close();
        }
};
