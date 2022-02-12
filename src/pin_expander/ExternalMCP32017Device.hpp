#include "PinExpanderBase.hpp"
#include "digital_read/ExternalMCP23017DigitalRead.hpp"
#include "digital_write/ExternalMCP23017DigitalWrite.hpp"

class ExternalMCP23017Device: public PinExpanderBase {
    private:
        ExternalMCP23017DigitalRead digitalReadInst;
        ExternalMCP23017DigitalWrite digitalWriteInst;

    public:
        ExternalMCP23017Device() {
            this->digitalReadInst = ExternalMCP23017DigitalRead();
            this->digitalWriteInst = ExternalMCP23017DigitalWrite();
        }

        void setMode(int pin, char mode) {
            // TODO: allow for changing between different instances for devices that need it
            // ESP32 probably doesn't
            // (INPUT, INPUT_PULLUP, OUTPUT should also be added separately)
        }
};
