#include "PinExpanderBase.hpp"
#include "digital_read/ExternalMCP23016DigitalRead.hpp"
#include "digital_write/ExternalMCP23016DigitalWrite.hpp"

class ExternalMCP23016Device: public PinExpanderBase {
    private:
        ExternalMCP23016DigitalRead digitalReadInst;
        ExternalMCP23016DigitalWrite digitalWriteInst;

    public:
        ExternalMCP23016Device() {
            this->digitalReadInst = ExternalMCP23016DigitalRead();
            this->digitalWriteInst = ExternalMCP23016DigitalWrite();
        }

        void setMode(int pin, char mode) {
            // TODO: allow for changing between different instances for devices that need it
            // ESP32 probably doesn't
            // (INPUT, INPUT_PULLUP, OUTPUT should also be added separately)
        }
};
