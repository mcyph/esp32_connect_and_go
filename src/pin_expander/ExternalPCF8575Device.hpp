#include "PinExpanderBase.hpp"
#include "digital_read/ExternalPCF8575DigitalRead.hpp"
#include "digital_write/ExternalPCF8575DigitalWrite.hpp"

class ExternalPCF8574Device: public PinExpanderBase {
    private:
        ExternalPCF8575DigitalRead digitalReadInst;
        ExternalPCF8575DigitalWrite digitalWriteInst;

    public:
        ExternalPCF8574Device() {
            this->digitalReadInst = ExternalPCF8575DigitalRead();
            this->digitalWriteInst = ExternalPCF8575DigitalWrite();
        }

        void setMode(int pin, char mode) {
            // TODO: allow for changing between different instances for devices that need it
            // ESP32 probably doesn't
            // (INPUT, INPUT_PULLUP, OUTPUT should also be added separately)
        }
};
