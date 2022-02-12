#include "PinExpanderBase.hpp"
#include "digital_read/ExternalPCF8574DigitalRead.hpp"
#include "digital_write/ExternalPCF8574DigitalWrite.hpp"

class ExternalPCF8574Device: public PinExpanderBase {
    private:
        ExternalPCF8574DigitalRead digitalReadInst;
        ExternalPCF8574DigitalWrite digitalWriteInst;

    public:
        ExternalPCF8574Device() {
            this->digitalReadInst = ExternalPCF8574DigitalRead();
            this->digitalWriteInst = ExternalPCF8574DigitalWrite();
        }

        void setMode(int pin, char mode) {
            // TODO: allow for changing between different instances for devices that need it
            // ESP32 probably doesn't
            // (INPUT, INPUT_PULLUP, OUTPUT should also be added separately)
        }
};
