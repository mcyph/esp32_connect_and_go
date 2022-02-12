#include "PinExpanderBase.hpp"
#include "analog_write/ExternalADS1115AnalogWrite.hpp"

class ExternalADS1115Device: public PinExpanderBase {
    private:
        ExternalADS1115AnalogWrite analogWriteInst;

    public:
        ExternalADS1115Device() {
            this->analogWriteInst = ExternalADS1115AnalogWrite();
        }

        void setMode(int pin, byte mode) {
            // TODO: allow for changing between different instances for devices that need it
            // ESP32 probably doesn't
            // (INPUT, INPUT_PULLUP, OUTPUT should also be added separately)
        }
};
