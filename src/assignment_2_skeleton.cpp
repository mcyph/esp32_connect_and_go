
#include "Arduino.h"
#include "display/GrayscaleDisplay.hpp"
#include "audio/FMRadioReceiver.hpp"

#include <SPI.h>
#include <Adafruit_VS1053_FilePlayer.h>
#include <SD.h>

//Add the SdFat Libraries
#include <SdFat.h>

// https://forum.arduino.cc/t/pin-setup-for-vs1053-esp32-md_midifile/596502/3

SPIClass hardwareSPI = SPIClass(VSPI);

/*
Nokia5110GrayscaleDisplay * displayInst1;
Nokia5110GrayscaleDisplay * displayInst2;
FMRadioReceiver * fmRadioReceiever;

void setup() {
    pinMode(15, OUTPUT);
    pinMode(33, OUTPUT);
    digitalWrite(15, HIGH);
    digitalWrite(33, HIGH);

    displayInst1 = new Nokia5110GrayscaleDisplay(15, 2, 4, 13, 14);
    displayInst2 = new Nokia5110GrayscaleDisplay(32, 33, 4, 13, 14);
    fmRadioReceiever = new FMRadioReceiver(RadioDriverType::TEA5767);
    fmRadioReceiever->setBandFrequency(95.1);

    for (int x=0; x<84; x++) {
        for (int y=0; y<48; y++) {
            displayInst1->putPixel(x, y, y/16);
            displayInst2->putPixel(x, y, displayInst1->getBrightnessSteps()-(y/16));
        }
    }
    
}

void loop() {
    for (int i=0; i<displayInst1->getBrightnessSteps(); i++) {
        displayInst1->pollStep(i);
        displayInst2->pollStep(i);
    }
}
*/

// These are the pins used for the breakout example
#define MOSI    23
#define MISO    19
#define CLK     18
#define XRESET  25      // VS1053 reset pin (output)
#define XCS     27     // VS1053 chip select pin (output)
#define XDCS    26      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 5     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 35       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(
    XRESET, XCS, XDCS, DREQ, CARDCS, hardwareSPI
);
//Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(
//    MOSI, MISO, CLK, XRESET, XCS, XDCS, DREQ, CARDCS
//);

/// File listing helper
void printDirectory(File dir, int numTabs) {
    while(true) {
        File entry =  dir.openNextFile();
        if (! entry) {
            // no more files
            //Serial.println("**nomorefiles**");
            break;
        }
        for (uint8_t i=0; i<numTabs; i++) {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
            Serial.println("/");
            printDirectory(entry, numTabs+1);
        } else {
            // files have sizes, directories do not
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
};


SdFs sd;
FsFile file;
FsFile root;

void setup() {
    //setCpuFrequencyMhz(80);
    //hardwareSPI.setClockDivider(SPI_CLOCK_DIV16);  // -> 1Mhz: https://hackaday.com/2016/10/31/whats-new-esp-32-testing-the-arduino-esp32-library/

    Serial.begin(9600);
    pinMode(15, OUTPUT);
    pinMode(33, OUTPUT);
    pinMode(CARDCS, OUTPUT);
    pinMode(XCS, OUTPUT);
    pinMode(XDCS, OUTPUT);
    digitalWrite(15, HIGH); // Disp 1
    digitalWrite(33, HIGH); // Disp 2
    digitalWrite(CARDCS, LOW); // microsd(?) CS
    digitalWrite(XCS, HIGH); // XCS
    digitalWrite(XDCS, HIGH); // XDCS


  if (!sd.cardBegin(SdSpiConfig(CARDCS, DEDICATED_SPI, SD_SCK_MHZ(15), &hardwareSPI))) {
    sd.initErrorHalt(&Serial);
  }
  if (!root.open("/")) {
    Serial.println("Error open root");
  }
  
  sd.ls(LS_R | LS_DATE | LS_SIZE);  

    Serial.println("Adafruit VS1053 Simple Test");

    if (! musicPlayer.begin()) { // initialise the music player
        Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
        while (1);
    }
    Serial.println(F("VS1053 found"));

    // Set volume for left, right channels. lower numbers == louder volume!
    musicPlayer.setVolume(20,20);

    // Timer interrupts are not suggested, better to use DREQ interrupt!
    //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

    // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
    // audio playing
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

    // Play one file, don't return until complete
    Serial.println(F("Playing track 001"));
    //musicPlayer.playFullFile("/track001.mp3");
    // Play another file in the background, REQUIRES interrupts!
    //Serial.println(F("Playing track 002"));
    //musicPlayer.startPlayingFile("/track002.mp3");

    musicPlayer.sineTest(220, 5000);
}

void loop() {
    // File is playing in the background
    if (musicPlayer.stopped()) {
        Serial.println("Done playing music");
        while (1) {
            delay(10);  // we're done! do nothing...
        }
    }

    if (Serial.available()) {
        char c = Serial.read();

        // if we get an 's' on the serial console, stop!
        if (c == 's') {
            musicPlayer.stopPlaying();
        }

        // if we get an 'p' on the serial console, pause/unpause!
        if (c == 'p') {
            if (! musicPlayer.paused()) {
                Serial.println("Paused");
                musicPlayer.pausePlaying(true);
            } else { 
                Serial.println("Resumed");
                musicPlayer.pausePlaying(false);
            }
        }
    }

    delay(100);
}




/*
 * This program attempts to initialize an SD card and analyze its structure.
 */

//#include "SdFat.h"
//#include "sdios.h"

/*
  Set DISABLE_CS_PIN to disable a second SPI device.
  For example, with the Ethernet shield, set DISABLE_CS_PIN
  to 10 to disable the Ethernet controller.
*/

//const int8_t DISABLE_CS_PIN = -1;

/*
  Change the value of SD_CS_PIN if you are using SPI
  and your hardware does not use the default value, SS.
  Common values are:
  Arduino Ethernet shield: pin 4
  Sparkfun SD shield: pin 8
  Adafruit SD shields and modules: pin 10
*/

// SDCARD_SS_PIN is defined for the built-in SD on some boards.

/*
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else  // SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN

SPIClass vspi = SPIClass(VSPI);

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(25), &vspi)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(25))
#endif  // HAS_SDIO_CLASS

//------------------------------------------------------------------------------
SdFs sd;
cid_t m_cid;
csd_t m_csd;
uint32_t m_eraseSize;
uint32_t m_ocr;
static ArduinoOutStream cout(Serial);
//------------------------------------------------------------------------------
bool cidDmp() {
  cout << F("\nManufacturer ID: ");
  cout << uppercase << showbase << hex << int(m_cid.mid) << dec << endl;
  cout << F("OEM ID: ") << m_cid.oid[0] << m_cid.oid[1] << endl;
  cout << F("Product: ");
  for (uint8_t i = 0; i < 5; i++) {
    cout << m_cid.pnm[i];
  }
  cout << F("\nVersion: ");
  cout << int(m_cid.prv_n) << '.' << int(m_cid.prv_m) << endl;
  cout << F("Serial number: ") << hex << m_cid.psn << dec << endl;
  cout << F("Manufacturing date: ");
  cout << int(m_cid.mdt_month) << '/';
  cout << (2000 + m_cid.mdt_year_low + 10 * m_cid.mdt_year_high) << endl;
  cout << endl;
  return true;
}
//------------------------------------------------------------------------------
void clearSerialInput() {
  uint32_t m = micros();
  do {
    if (Serial.read() >= 0) {
      m = micros();
    }
  } while (micros() - m < 10000);
}
//------------------------------------------------------------------------------
bool csdDmp() {
  bool eraseSingleBlock;
  if (m_csd.v1.csd_ver == 0) {
    eraseSingleBlock = m_csd.v1.erase_blk_en;
    m_eraseSize = (m_csd.v1.sector_size_high << 1) | m_csd.v1.sector_size_low;
  } else if (m_csd.v2.csd_ver == 1) {
    eraseSingleBlock = m_csd.v2.erase_blk_en;
    m_eraseSize = (m_csd.v2.sector_size_high << 1) | m_csd.v2.sector_size_low;
  } else {
    cout << F("m_csd version error\n");
    return false;
  }
  m_eraseSize++;
  cout << F("cardSize: ") << 0.000512 * sdCardCapacity(&m_csd);
  cout << F(" MB (MB = 1,000,000 bytes)\n");

  cout << F("flashEraseSize: ") << int(m_eraseSize) << F(" blocks\n");
  cout << F("eraseSingleBlock: ");
  if (eraseSingleBlock) {
    cout << F("true\n");
  } else {
    cout << F("false\n");
  }
  return true;
}
//------------------------------------------------------------------------------
void errorPrint() {
  if (sd.sdErrorCode()) {
    cout << F("SD errorCode: ") << hex << showbase;
    printSdErrorSymbol(&Serial, sd.sdErrorCode());
    cout << F(" = ") << int(sd.sdErrorCode()) << endl;
    cout << F("SD errorData = ") << int(sd.sdErrorData()) << endl;
  }
}
//------------------------------------------------------------------------------
bool mbrDmp() {
  MbrSector_t mbr;
  bool valid = true;
  if (!sd.card()->readSector(0, (uint8_t*)&mbr)) {
    cout << F("\nread MBR failed.\n");
    errorPrint();
    return false;
  }
  cout << F("\nSD Partition Table\n");
  cout << F("part,boot,bgnCHS[3],type,endCHS[3],start,length\n");
  for (uint8_t ip = 1; ip < 5; ip++) {
    MbrPart_t *pt = &mbr.part[ip - 1];
    if ((pt->boot != 0 && pt->boot != 0X80) ||
        getLe32(pt->relativeSectors) > sdCardCapacity(&m_csd)) {
      valid = false;
    }
    cout << int(ip) << ',' << uppercase << showbase << hex;
    cout << int(pt->boot) << ',';
    for (int i = 0; i < 3; i++ ) {
      cout << int(pt->beginCHS[i]) << ',';
    }
    cout << int(pt->type) << ',';
    for (int i = 0; i < 3; i++ ) {
      cout << int(pt->endCHS[i]) << ',';
    }
    cout << dec << getLe32(pt->relativeSectors) << ',';
    cout << getLe32(pt->totalSectors) << endl;
  }
  if (!valid) {
    cout << F("\nMBR not valid, assuming Super Floppy format.\n");
  }
  return true;
}
//------------------------------------------------------------------------------
void dmpVol() {
  cout << F("\nScanning FAT, please wait.\n");
  uint32_t freeClusterCount = sd.freeClusterCount();
  if (sd.fatType() <= 32) {
    cout << F("\nVolume is FAT") << int(sd.fatType()) << endl;
  } else {
    cout << F("\nVolume is exFAT\n");
  }
  cout << F("sectorsPerCluster: ") << sd.sectorsPerCluster() << endl;
  cout << F("clusterCount:      ") << sd.clusterCount() << endl;
  cout << F("freeClusterCount:  ") << freeClusterCount << endl;
  cout << F("fatStartSector:    ") << sd.fatStartSector() << endl;
  cout << F("dataStartSector:   ") << sd.dataStartSector() << endl;
  if (sd.dataStartSector() % m_eraseSize) {
    cout << F("Data area is not aligned on flash erase boundary!\n");
    cout << F("Download and use formatter from www.sdcard.org!\n");
  }
}
//------------------------------------------------------------------------------
void printCardType() {

  cout << F("\nCard type: ");

  switch (sd.card()->type()) {
    case SD_CARD_TYPE_SD1:
      cout << F("SD1\n");
      break;

    case SD_CARD_TYPE_SD2:
      cout << F("SD2\n");
      break;

    case SD_CARD_TYPE_SDHC:
      if (sdCardCapacity(&m_csd) < 70000000) {
        cout << F("SDHC\n");
      } else {
        cout << F("SDXC\n");
      }
      break;

    default:
      cout << F("Unknown\n");
  }
}
//------------------------------------------------------------------------------
void printConfig(SdSpiConfig config) {
  if (DISABLE_CS_PIN < 0) {
    cout << F(
           "\nAssuming the SD is the only SPI device.\n"
           "Edit DISABLE_CS_PIN to disable an SPI device.\n");
  } else {
    cout << F("\nDisabling SPI device on pin ");
    cout << int(DISABLE_CS_PIN) << endl;
    pinMode(DISABLE_CS_PIN, OUTPUT);
    digitalWrite(DISABLE_CS_PIN, HIGH);
  }
  cout << F("\nAssuming the SD chip select pin is: ") << int(config.csPin);
  cout << F("\nEdit SD_CS_PIN to change the SD chip select pin.\n");
}
//------------------------------------------------------------------------------
void printConfig(SdioConfig config) {
  (void)config;
  cout << F("Assuming an SDIO interface.\n");
}
//-----------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // Wait for USB Serial
  while (!Serial) {
    SysCall::yield();
  }
  cout << F("SdFat version: ") << SD_FAT_VERSION_STR << endl;
  printConfig(SD_CONFIG);

}
//------------------------------------------------------------------------------
void loop() {
  // Read any existing Serial data.
  clearSerialInput();

  // F stores strings in flash to save RAM
  cout << F("\ntype any character to start\n");
  while (!Serial.available()) {
    SysCall::yield();
  }
  uint32_t t = millis();
  if (!sd.cardBegin(SD_CONFIG)) {
    cout << F(
           "\nSD initialization failed.\n"
           "Do not reformat the card!\n"
           "Is the card correctly inserted?\n"
           "Is there a wiring/soldering problem?\n");
    if (isSpi(SD_CONFIG)) {
      cout << F(
           "Is SD_CS_PIN set to the correct value?\n"
           "Does another SPI device need to be disabled?\n"
           );
    }
    errorPrint();
    return;
  }
  t = millis() - t;
  cout << F("init time: ") << t << " ms" << endl;

  if (!sd.card()->readCID(&m_cid) ||
      !sd.card()->readCSD(&m_csd) ||
      !sd.card()->readOCR(&m_ocr)) {
    cout << F("readInfo failed\n");
    errorPrint();
    return;
  }
  printCardType();
  cidDmp();
  csdDmp();
  cout << F("\nOCR: ") << uppercase << showbase;
  cout << hex << m_ocr << dec << endl;
  if (!mbrDmp()) {
    return;
  }
  if (!sd.volumeBegin()) {
    cout << F("\nvolumeBegin failed. Is the card formatted?\n");
    errorPrint();
    return;
  }
  dmpVol();
} 
*/