#include "AnalogReadBase.hpp"
#include <Adafruit_ADS1015.h>

// 32767 max - but can give negative voltage values(!)

class ExternalADS1115AnalogRead: public AnalogReadBase {
    private:
        Adafruit_ADS1115 ads;
    
    public:
        ExternalADS1115AnalogRead(int logicalPinNumberStart, 
                                  int gain=GAIN_ONE, 
                                  int address=0x48): 
            AnalogReadBase(logicalPinNumberStart, 
                           vector<int>{32767, 32767, 32767, 32767},
                           vector<int>{-32768, -32768, -32768, -32768}) {
            ads = Adafruit_ADS1115(address);
            ads.setGain(gain);
            ads.begin();
        }

        int getValue(int pin) override {
            return analogRead(pin);
        } {
            return ads.readADC_SingleEnded(); // CHECK ME!!
        }
};
