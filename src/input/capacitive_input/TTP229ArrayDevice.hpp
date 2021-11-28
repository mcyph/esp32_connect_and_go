#include "CapacitiveInputDeviceArrayBase.hpp"

class TTP229ArrayDevice: public CapacitiveInputDeviceArrayBase {
    private:
        int sclPin;
        vector<int> sdoPins;
        vector<vector<int>> pressedButtons;

    public:
        TTP229ArrayDevice(int sclPin, vector<int> sdoPins) {
            // https://electropeak.com/learn/interfacing-16-channel-ttp229-capacitive-touch-16-key-keypad-with-arduino/
            pinMode(sclPin, OUTPUT);
            for (int sdoPin: sdoPins) {  
                pinMode(sdoPin, INPUT);
            } 
        }

        bool poll() override {
            // TODO: Test me!
            for (vector<int> iPressedButtons: pressedButtons) {
                if (!iPressedButtons.empty()) {
                    iPressedButtons.clear();
                }
            }
            for (int i=0; i<16; i++) {
                digitalWrite(sclPin, LOW); 
                for (int j=0; j<sdoPins.size(); i++) {
                    if (!digitalRead(sdoPins[i])) {
                        pressedButtons[j].push_back(i);
                    }
                }
                digitalWrite(sclPin, HIGH);
            }
            return true;
        }

        int getPressedButton() override {
            // Get the first pressed button only
            for (vector<int> iPressedButtons: pressedButtons) {
                if (iPressedButtons.size() > 0) {
                    return iPressedButtons[0];
                }
            }
            return -1;
        }

        vector<vector<int>> getPressedButtons() override {
            return this->pressedButtons;
        }
};

