#include <Arduino.h>;
#include "DirectionalInputDeviceBase.hpp"

class AnalogInputDevice: public DirectionalInputDeviceBase {
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
