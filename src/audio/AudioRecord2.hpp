// https://haberocean.com/2020/02/vs1003-voice-recording-and-storing-in-sd-card-for-indefinite-time-using-arduino-mega-circuit-and-program/
#include <SPI.h>
#include <SD.h>
//#include "SdFat.h"
//SdFat SD;

SPIClass mySPI;

// Size of the "wav" file set to 100*512 bytes including the header files
const unsigned char RIFFHeader0fn[] = {
    'R' , 'I' , 'F' , 'F' , // Chunk ID (RIFF)
    0xF8, 0xC7, 0x00, 0x00, // Chunk payload size (calculate after rec!)
    'W' , 'A' , 'V' , 'E' , // RIFF resource format type

    'f' , 'm' , 't' , ' ' , // Chunk ID (fmt )
    0x14, 0x00, 0x00, 0x00, // Chunk payload size (0x14 = 20 bytes)
    0x11, 0x00,             // Format Tag (IMA ADPCM)
    0x01, 0x00,             // Channels (1)
    0x40, 0x1f, 0x00, 0x00, // Sample Rate, 0x3e80 = 16.0kHz
    0x75, 0x12, 0x00, 0x00, // Average Bytes Per Second
    0x00, 0x01,             // Data Block Size (256 bytes) 
    0x04, 0x00,             // ADPCM encoded bits per sample (4 bits)
    0x02, 0x00,             // Extra data (2 bytes)
    0xf9, 0x01,             // Samples per Block (505 samples)

    'f' , 'a' , 'c' , 't' , // Chunk ID (fact)
    0xc8, 0x01, 0x00, 0x00, // Chunk payload size (456 bytes (zeropad!))
    0x96, 0x86, 0x01, 0x00  // Number of Samples (calculate after rec!)
};

const unsigned char RIFFHeader504fn[] = {
    'd' , 'a' , 't' , 'a' , // Chunk ID (data)
    0x00, 0xC6, 0x00, 0x00, // Chunk payload size (calculate after rec!)
};

const int xCs = A3;
const int xDcs = A2;
const int xDreq = 4;
const int xReset = A0;

const int sdCs = 10;
const int ctrlBtn = 2;
bool ctrlBtnState = false;
bool ctrlBtnPressed = false;
bool ctrlBtnReleased = false;

// SCI Registers

typedef enum SCI_REGISTER: uint8_t {
  MODE = 0x0,
  STATUS = 0x1,
  BASS = 0x2,
  CLOCKF = 0x3,
  DECODE_TIME = 0x4,
  AUDATA = 0x5,
  WRAM = 0x6,
  WRAMADDR = 0x7,
  HDAT0 = 0x8,
  HDAT1 = 0x9,
  AIADDR = 0xa,
  VOL = 0xb,
  AICTRL0 = 0xc,
  AICTRL1 = 0xd,
  AICTRL2 = 0xe,
  AICTRL3 = 0xf,
  num_registers = 0xf
};

// SCI_MODE bits
typedef enum SCI_MODE: uint8_t {
  DIFF = 0,
  LAYER12 = 1,
  RESET = 2,
  OUTOFWAV = 3,
  EARSPEAKER_LO = 4,
  TESTS = 5,
  STREAM = 6,
  EARSPEAKER_HI = 7,
  DACT = 8,
  SDIORD = 9,
  SDISHARE = 10,
  SDINEW = 11,
  ADPCM = 12,
  ADCPM_HP = 13,
  LINE_IN = 14,
};

uint16_t t, w;
bool recordModeOn;

int iCount = 0;
long int loopCount = 0;

char fileName[] = "wavFile6.WAV";
File myFile;
byte data[4];

unsigned char db[512];

void control_mode_on(void) {
  digitalWrite(xDcs, HIGH);
  digitalWrite(xCs,LOW);
}

void await_data_request(void) {
  while (!digitalRead(xDreq));
}

void control_mode_off(void) {
  digitalWrite(xCs, HIGH);
}

uint16_t read_register_my_own(SCI_REGISTER _reg) {
  // VS1003 SCI Read COmmand byte is 0x03
  while(!digitalRead(xDreq));
  digitalWrite(xCs, LOW);
  mySPI.transfer(0x03);
  mySPI.transfer(_reg);
  unsigned char response1 = mySPI.transfer(0xFF);
  unsigned char response2 = mySPI.transfer(0xFF);
  digitalWrite(xCs, HIGH);
  return ((unsigned int) response1 << 8) | (response2 & 0xFF);
}

void write_register(uint8_t _reg, uint16_t _value) {
  // VS1003 SCI Write Command byte is 0x02
  control_mode_on();
  delayMicroseconds(1); // tXCSS
  mySPI.transfer(0x02); // Write operation
  mySPI.transfer(_reg); // Which register
  mySPI.transfer(_value >> 8); // Send hi byte
  mySPI.transfer(_value & 0xff); // Send lo byte
  delayMicroseconds(1); // tXCSH
  await_data_request();
  control_mode_off();
}

void data_mode_on(void) {
  digitalWrite(xCs, HIGH);
  digitalWrite(xDcs, LOW);
}

void data_mode_off(void) {
  digitalWrite(xDcs, HIGH);
}

void setVolume(uint8_t vol) {
  uint16_t value = vol;
  value <<= 8;
  value |= vol;

  write_register(SCI_REGISTER::VOL, value); // VOL
}

void setup() {
  // put your setup code here, to run once:

  // initiate a serial port at 57600
  Serial.begin(115200);
  
  // Keep the chip in reset until we are ready
  pinMode(xReset, OUTPUT);
  pinMode(xCs, OUTPUT);
  pinMode(xDcs, OUTPUT);
  pinMode(xDreq, INPUT); 
  pinMode(sdCs, OUTPUT);
  pinMode(ctrlBtn, INPUT);

  digitalWrite(xReset, LOW);
  digitalWrite(xCs, HIGH);
  digitalWrite(xDcs, HIGH);
  digitalWrite(xReset, HIGH);

  digitalWrite(sdCs, HIGH);

  while(!SD.begin(sdCs)) {
    Serial.println("SD initialization failed..");
  }

  Serial.println("SD initialization successfull..");

  if(SD.exists(fileName)) {
    SD.remove(fileName);
  }

  (myFile = SD.open(fileName, FILE_WRITE)) ? Serial.println("SD opening successfull") : Serial.println("SD opening failed");

  myFile.write(RIFFHeader0fn, sizeof(RIFFHeader0fn));

  // Write '0' (0x30) from address 51 to 503 
  for (int i = 0; i<452; i++) {
    myFile.write('0');
  }

  // write second RIFF header from 504 to 511
  myFile.write(RIFFHeader504fn, sizeof(RIFFHeader504fn));

  // initiate SPI
  mySPI.begin();
  mySPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));

  write_register(SCI_REGISTER::VOL, 0x0000);
  write_register(SCI_REGISTER::BASS, 0);
  write_register(SCI_REGISTER::CLOCKF, 0x4430);

  write_register(SCI_REGISTER::AICTRL0, 12);
  delay(100);

  write_register(SCI_REGISTER::AICTRL1, 1024);
  delay(100);

  write_register(SCI_REGISTER::MODE, 0x1804);

  mySPI.endTransaction();

  //myFile.close();

}

void updateAndCloseAudioFile() {
  Serial.println(myFile.size());
  myFile.close();
  Serial.println("Recording off...");
  Serial.println("Closed the Audio file...");

  if (!(myFile = SD.open(fileName, O_WRITE))) {
    Serial.println("Failed to open record file in O_WRITE mode");
  }

  unsigned long int paySize1 = myFile.size() - 8;
  unsigned long int numSamp = ((myFile.size() - 512) / 256) * 505;
  unsigned long int paySize2 = myFile.size() - 512;

  data[3] = paySize1 >> 24; // shift it over so only last byte exists
  data[2] = paySize1 >> 16;
  data[1] = paySize1 >> 8;
  data[0] = paySize1;

  //    Update "RIFF" header chunk at byte 4
  myFile.seek(4);
  myFile.write(data, sizeof(data));

  data[3] = numSamp >> 24;  // shift it over so only last byte exists
  data[2] = numSamp >> 16;
  data[1] = numSamp >> 8;
  data[0] = numSamp;
  
  //    Update "FACT" header chunk at byte 48
  myFile.seek(48);
  myFile.write(data, sizeof(data));
  
  data[3] = paySize2 >> 24; // shift it over so only last byte exists
  data[2] = paySize2 >> 16;
  data[1] = paySize2 >> 8;
  data[0] = paySize2;
  
  //    Update "DATA" header chunk at byte 508
  myFile.seek(508);
  myFile.write(data, sizeof(data));

  Serial.println(myFile.size());
    
  myFile.close();
}

void loop() {
  if (digitalRead(ctrlBtn)) {
    ctrlBtnPressed = true;
    delay(50);
  }
  
  else if(ctrlBtnPressed && !digitalRead(ctrlBtn)) {
    ctrlBtnReleased = true;
  }

  if (ctrlBtnPressed && ctrlBtnReleased) {
    ctrlBtnPressed = false;
    ctrlBtnReleased = false;

    if (recordModeOn) {
      recordModeOn = false;

      updateAndCloseAudioFile();
    } else {
      recordModeOn = true;
      Serial.println("Recording on...");
    }

    // Serial.println(recordModeOn);
  }

  if (recordModeOn) {
    mySPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
    t = read_register_my_own(SCI_REGISTER::HDAT1);

    while (t >= 256 && t < 896) {
      for (int i=0;i<512;i=i+2) {
        w = read_register_my_own(SCI_REGISTER::HDAT0);
        db[i] = w >> 8;
        db[i+1] = w & 0xFF;
      }
  
      mySPI.endTransaction();
  
      myFile.write(db, sizeof(db));

      t -= 256;
    }    
  } else {
    //Serial.println("Record mode off");
  }
}