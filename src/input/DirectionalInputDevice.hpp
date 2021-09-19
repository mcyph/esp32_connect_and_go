#include <vector>;
#include <string.h>;
#include <Arduino.h>;
#include "../pin_expander/PinExpander.hpp";

typedef enum class InputDirection {
    NONE, 
    NORTH, NORTHEAST, 
    EAST, SOUTHEAST, 
    SOUTH, SOUTHWEST, 
    WEST, NORTHWEST
};

class DirectionalInputDevice {
    public:
        virtual bool poll();
        virtual float getXValue();
        virtual float getYValue();
        virtual InputDirection getDirection();
};

class DigitalInputButtons: public DirectionalInputDevice {
    private:
        PinExpanderBase pinExpander;
        int upPin;
        int downPin;
        int leftPin;
        int rightPin;
        int centerPin;

    public:
        DigitalInputButtons(
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

class AnalogInputDevice: public DirectionalInputDevice {
    private:
        int xPin;
        int yPin;

        int minValue;
        int maxValue;
        int thresholdValue;

        float xValue;
        float yValue;

    public:
        AnalogInputDevice(
            int xPin, 
            int yPin, 
            int minValue=0, 
            int maxValue=1024, 
            int thresholdValue=20
        ) {
            this->minValue = minValue;
            this->maxValue = maxValue;
            this->thresholdValue = thresholdValue;
        };

        bool poll() {
            int midValue = (maxValue - minValue) / 2;
            int xValue = analogRead(xPin) - minValue;
            int yValue = analogRead(yPin) - minValue;
            
            if (abs(midValue-xValue) <= thresholdValue) {
                xValue = midValue;
            }
            if (abs(midValue-yValue) <= thresholdValue) {
                yValue = midValue;
            }

            this->xValue = xValue;
            this->yValue = yValue;
        };
};
