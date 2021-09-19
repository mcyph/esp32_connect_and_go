#include <SPI.h>;
#include "SdFat.h";
#include "sdios.h";
#include <stdexcept>;
#include <string.h>;
#include <Arduino.h>;
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h";

using namespace std;

typedef enum class BlockDeviceType {
    SD_CARD, INTERNAL_FLASH, NETWORK
};
typedef enum class RelativeTo {
    CURRENT_POSITION, START, END
};

class FileObjectBase {
    public:
        FileObjectBase() {};
        virtual void seek(long amount, RelativeTo relativeTo=RelativeTo::CURRENT_POSITION);
        virtual String read(long amount);
        virtual long tell();
        virtual void flush();
        virtual void close();
};

class FileObjectCache: public FileObjectBase {
    public:
        FileObjectCache(FileObjectBase * blockDevice, int blockSize=256, int numBlocks=1024) {
            // TODO!
        };

        void seek(long amount, RelativeTo relativeTo=RelativeTo::CURRENT_POSITION) {};
        String read(long amount) {};
        long tell() {};
        void flush() {};
        void close() {
            this->flush();
        };

    private:
        char ** blockCache;
};

class StringWithLength: public String {
    public:
        StringWithLength(char * c_str, long n): String("") {
            this->concat(c_str, n);
        };
};

class SPISDCardFile: public FileObjectBase {
    private:
        String path;
        File myFile;

    public:
        SPISDCardFile(String path, char mode): FileObjectBase() {
            this->myFile = SD.open(path.c_str(), mode);
            if (myFile) {
                Serial.print("Writing to test.txt...");
                myFile.println("This is a test file :)");
                myFile.println("testing 1, 2, 3.");
                for (int i = 0; i < 20; i++) {
                    myFile.println(i);
                }
                myFile.close();
                Serial.println("done.");
            }

            myFile = SD.open("test.txt");
            if (myFile) {
                Serial.println("test.txt:");
                while (myFile.available()) {
                    Serial.write(myFile.read());
                }
                myFile.close();
            } else {
                Serial.println("error opening test.txt");
            }
        }

        void seek(long amount, RelativeTo relativeTo=RelativeTo::CURRENT_POSITION) {
            if (relativeTo == RelativeTo::CURRENT_POSITION) {
                myFile.seek(amount, SeekCur);
            } else if (relativeTo == RelativeTo::START) {
                myFile.seek(amount, SeekSet);
            } else if (relativeTo == RelativeTo::END) {
                myFile.seek(amount, SeekEnd);
            }
        }

        String read(long amount) {
            char buffer[amount + 1];
            long amountRead = myFile.readBytes(buffer, amount);
            return StringWithLength(buffer, amountRead);
        }

        String readLine() {
            // TODO: Increase efficiency!
            char c;
            String out = "";
            while (true) {
                c = myFile.read();
                if (c) {
                    out += c;
                }
                if (!c || c != '\n') {
                    break;
                }
            }
            return out;
        }
        
        long tell() {
            return myFile.position();
        }
        
        void flush() {
            myFile.flush();
        }
        
        void close() {
            this->flush();
        }
};

class QwiicOpenLogFile: public FileObjectBase {
    private:
        String path;
        OpenLog * myLog;
        long sizeOfFile;

    public:
        QwiicOpenLogFile(OpenLog * myLog, String path, String mode): FileObjectBase() {
            this->myLog = myLog;
            this->path = path;
            sizeOfFile = myLog->size(path);

            if (sizeOfFile <= -1) {
                Serial.println("Size error: File not found");
            }
        }

        String read(int amount=1024) {
            //Read the contents of myFile by passing a buffer into .read()
            //Then printing the contents of that buffer
            byte myBufferSize = 200;
            byte myBuffer[myBufferSize];
            //myLog.read(myBuffer, myBufferSize, myFile, 4); //Doesn't yet work
            myLog->read(myBuffer, myBufferSize, path); //Load myBuffer with contents of myFile

            for (int x = 0 ; x < myBufferSize ; x++) {
                Serial.write(myBuffer[x]);
            }
            Serial.println("\nDone with file contents");
        }

        void write(String data) {
            myLog->writeString(data);
        } 

        long tell() {

        }

        void flush() {
            myLog->syncFile();
        }

        String readLine() {
            
        }
};

// Test with reduced SPI speed for breadboards.  SD_SCK_MHZ(4) will select
// the highest speed supported by the board that is not over 4 MHz.
// Change SPI_SPEED to SD_SCK_MHZ(50) for best performance.
#define SPI_SPEED SD_SCK_MHZ(4)

// File system object.
SdFat sd;

// SD card chip select
int CHIP_SELECT_PIN = FIXME;

void setup() {
    if (!sd.begin(CHIP_SELECT_PIN, SPI_SPEED)) {
        if (sd.card()->errorCode()) {
            throw std::runtime_error((
                String(
                    "SD initialization failed. Do not reformat the card!\n"
                    "* Is the card correctly inserted?\n"
                    "* Is chipSelect set to the correct value?\n"
                    "* Does another SPI device need to be disabled?\n"
                    "* Is there a wiring/soldering problem?\n"
                    "errorCode: "
                ) + 
                String(sd.card()->errorCode()) +
                String(", errorData: ") + 
                String(sd.card()->errorData())
            ).c_str());
            return;
        }
    
        if (sd.vol()->fatType() == 0) {
          throw std::runtime_error(
              "Can't find a valid FAT16/FAT32 partition.\n"
              "Try reformatting the card. For best results use\n"
              "the SdFormatter program in SdFat/examples or download\n"
              "and use SDFormatter from www.sdcard.org/downloads."
          );
          return;
        }
        throw std::runtime_error("begin failed, can't determine error type");
        return;
    }
    throw std::runtime_error("Card successfully initialized.");
  
    uint32_t size = sd.card()->cardSize();
    if (size == 0) {
        throw std::runtime_error(
            "Can't determine the card size.\n"
            "Try another SD card or reduce the SPI bus speed.\n"
            "Edit SPI_SPEED in this program to change it."
        );
        return;
    }
  
    uint32_t sizeMB = 0.000512 * size + 0.5;
    Serial.println("Card size: %d", sizeMB);
    Serial.println(" MB (MB = 1,000,000 bytes)");
    Serial.println("Volume is FAT%d", int(sd.vol()->fatType());
    Serial.println(", Cluster size (bytes): %d", 512L * sd.vol()->blocksPerCluster());
    Serial.println("Files found (date time size name):");
    sd.ls(LS_R | LS_DATE | LS_SIZE);
}

void loop() {
    
}
