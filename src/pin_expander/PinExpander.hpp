#include <Arduino.h>

class AnalogWriteBase {

};

class AnalogReadBase {

};

class DigitalWriteBase {

};

class DigitalReadBase {

};



class MCP23016: public PinExpanderBase {

};

class MCP23017: public PinExpanderBase {
    public:
        const long analogInputNumPins = 8;
        const int analogPingNums[]
};

class PCA9685: public PinExpanderBase {

};

class PCF8574: public PinExpanderBase {

};

class PCF8575: public PinExpanderBase {

};

