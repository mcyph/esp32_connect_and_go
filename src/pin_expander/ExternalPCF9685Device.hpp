#include "PinExpanderBase.hpp"
#include "analog_write/ExternalPCF9685AnalogWrite.hpp"

class ExternalPCF9685Device: public PinExpanderBase {
    private:
        ExternalESP32AnalogWrite analogWriteInst;

    public:
        ExternalPCF9685Device() {
            this->analogWriteInst = ExternalPCF9685AnalogWrite();
        }

        void setMode(int pin, byte mode) {
            // TODO: allow for changing between different instances for devices that need it
            // ESP32 probably doesn't
            // (INPUT, INPUT_PULLUP, OUTPUT should also be added separately)
        }
};
