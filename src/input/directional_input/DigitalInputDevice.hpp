#include <vector>
#include <string.h>
#include <Arduino.h>
#include "../../pin_expander/PinExpander.hpp"
#include "DirectionalInputDeviceBase.hpp"

class DigitalInputDevice: public DirectionalInputDeviceBase {
    private:
        PinExpanderBase pinExpander;
        int upPin;
        int downPin;
        int leftPin;
        int rightPin;
        int centerPin;

    public:
        DigitalInputDevice(
            PinExpanderBase pinExpander, 
            int upPin, int downPin, int leftPin, int rightPin, int centerPin
        ) {
            this->pinExpander = pinExpander;
            this->upPin = upPin;
            this->downPin = downPin;
            this->leftPin = leftPin;
            this->rightPin = rightPin;
            this->centerPin = centerPin;
        };

        bool poll() {
            // TODO!
        }
};

