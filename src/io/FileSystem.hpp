#include <SD.h>;
#include <SPI.h>;
#include <Wire.h>;
#include <string.h>;
#include <Arduino.h>;
#include "FileObject.hpp";
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h";

using namespace std;

typedef struct {
    long size;
    long isDir;
    long permissions;
} StatInfo;

class FileSystemBase {
    public:
        virtual FileObjectBase * openFile(String path, String mode);
        virtual bool deleteFile();
        virtual bool createDirectory(String path);
        virtual bool deleteDirectory(String path);
        virtual vector<String> listDirectory(String path);
        virtual StatInfo stat(String path);

        FileObjectBase * getCachedFile(String path, String mode) {
            return new FileObjectCache(this->openFile(path, mode));
        };
};

class SDCardFileSystem: public FileSystemBase {
    public:
        SDCardFileSystem(
            uint8_t ssPin = (uint8_t)5U, 
            SPIClass &spi = SPI,
            uint32_t frequency = 4000000U,
            const char *mountpoint = "/sd",
            uint8_t max_files = (uint8_t)5U,
            bool format_if_empty = false
        ) {
            if (!SD.begin(10)) {
                // FIXME!!
                Serial.println("initialization failed!");
                while (1);
            }
        };

        ~SDCardFileSystem() { SD.end(); }

        FileObjectBase * openFile(String path, String mode) {
            // TODO!
        };

        bool deleteFile(String path) { SD.remove(path); };
        bool createDirectory(String path) { SD.mkdir(path); };
        bool deleteDirectory(String path) { SD.rmdir(path); };
        bool exists(String path) { return SD.exists(path); };
        bool rename(const String& pathFrom, const String& pathTo) { return SD.rename(pathFrom, pathTo); };

        vector<String> listDirectory(String path) {
            // https://www.arduino.cc/en/Tutorial/LibraryExamples/Listfiles
            File dir = SD.open(path);

            while (true) {
                File entry = dir.openNextFile();
                if (!entry) {
                    break;
                }

                for (uint8_t i = 0; i < numTabs; i++) {
                    Serial.print('\t');
                }
                Serial.print(entry.name());

                if (entry.isDirectory()) {
                    Serial.println("/");
                    printDirectory(entry, numTabs + 1);
                } else {
                    // files have sizes, directories do not
                    Serial.print("\t\t");
                    Serial.println(entry.size(), DEC);
                }

                entry.close();
            }
        };

        StatInfo stat(String path) {
            // TODO!
        };
};

class QwiicOpenLogFileSystem: public FileSystemBase {
    private:
        OpenLog myLog;

    public:
        QwiicOpenLogFileSystem() {
            Wire.begin();
            Wire.setClock(400000); // Go super fast - CHECK ME!
            myLog.begin();
        }

        FileObjectBase * openFile(String path, String mode) override {
            return new QwiicOpenLogFile(myLog, path, mode);
        };

        bool createDirectory(String path) override {
            // Make sure we're in the root directory - CHECK / is supported!
            myLog.changeDirectory("/"); 
            myLog.makeDirectory(path);
        };

        bool deleteDirectory(String path) override {
            // Make sure we're in the root directory - CHECK / is supported!
            myLog.changeDirectory("/"); 
            int numFilesDeleted = myLog.removeDirectory(path);
        };

        vector<String> listDirectory(String path) override {
            vector<String> r;
            myLog.searchDirectory(path + "/*");  // CHECK ME!
            String fileName = myLog.getNextDirectoryItem();
            while (fileName != "") {
                r.push_back(fileName);
                fileName = myLog.getNextDirectoryItem();
            }
            return r;
        };

        StatInfo stat(String path) override {
            long sizeOfFile = myLog.size(path);
            return StatInfo {
                .size = sizeOfFile,
                .isDir = FIXME,
                .permissions = 0
            };
        };
};
