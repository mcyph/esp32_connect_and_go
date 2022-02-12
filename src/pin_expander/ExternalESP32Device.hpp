#include "PinExpanderBase.hpp"
#include "analog_read/ExternalESP32AnalogRead.hpp"
#include "analog_write/ExternalESP32AnalogWrite.hpp"
#include "digital_read/ExternalESP32DigitalRead.hpp"
#include "digital_write/ExternalESP32DigitalWrite.hpp"

class ExternalESP32Device: public PinExpanderBase {
    private:
        ExternalESP32AnalogRead analogReadInst;
        ExternalESP32DigitalRead digitalReadInst;
        ExternalESP32AnalogWrite analogWriteInst;
        ExternalESP32DigitalWrite digitalWriteInst;

    public:
        ExternalESP32Device() {
            this->analogReadInst = ExternalESP32AnalogRead();
            this->digitalReadInst = ExternalESP32DigitalRead();
            this->analogWriteInst = ExternalESP32AnalogWrite();
            this->digitalWriteInst = ExternalESP32DigitalWrite();
        }

        void setMode(int pin, byte mode) {
            // TODO: allow for changing between different instances for devices that need it
            // ESP32 probably doesn't
            // (INPUT, INPUT_PULLUP, OUTPUT should also be added separately)
        }
};
