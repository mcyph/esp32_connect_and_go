#include <vector>
#include <Arduino.h>

using std::vector;

class DigitalReadBase {
    private:
        vector<int> pollingPins;
        vector<int> pollingPinValues;
        vector<void*> pollingPinCallbacks;

        vector<int> interruptPins;
        vector<int> interruptPinValues;
        vector<void*> interruptPinCallbacks;

        vector<int> possibleInterruptPins;

    protected:
        virtual void registerInterrupt(int pin);
        virtual void unregisterInterrupt(int pin);

    public:
        DigitalReadBase(vector<int> possibleInterruptPins) {
          this->pollingPins = vector<int>();
          this->pollingPinValues = vector<int>();
          this->pollingPinCallbacks = vector<void*>();

          this->interruptPins = vector<int>();
          this->interruptPinValues = vector<int>();
          this->interruptPinCallbacks = vector<void*>();
          
          this->possibleInterruptPins = possibleInterruptPins;
        }

        //==============================================================
        // Pin Information
        //==============================================================

        //==============================================================
        // Interrupt Callbacks
        //==============================================================

        bool addInterruptCallback(int pin, void* callback) {
            interruptPins.push_back(pin);
            interruptPinValues.push_back(getValue(pin));
            interruptPinCallbacks.push_back(callback);
        }

        bool removeInterruptCallback(int pin, void* callback) {
            for (int i=interruptPins.size()-1; i >= 0; i--) {
                if (pin == interruptPins[i] && callback == interruptPinCallbacks[i]) {
                    interruptPins.erase(interruptPins.begin() + i);
                    interruptPinValues.erase(interruptPinValues.begin() + i);
                    interruptPinCallbacks.erase(interruptPinCallbacks.begin() + i);
                }
            }
        }

        bool removeInterruptCallback(int pin) {
            for (int i=interruptPins.size()-1; i >= 0; i--) {
                if (pin == interruptPins[i]) {
                    interruptPins.erase(interruptPins.begin() + i);
                    interruptPinValues.erase(interruptPinValues.begin() + i);
                    interruptPinCallbacks.erase(interruptPinCallbacks.begin() + i);
                }
            }
        }

        //==============================================================
        // Polling Callbacks
        //==============================================================

        bool addPollingCallback(int pin, void* callback) {
            pollingPins.push_back(pin);
            pollingPinValues.push_back(getValue(pin));
            pollingPinCallbacks.push_back(callback);
        }

        bool removePollingCallback(int pin, void* callback) {
            for (int i=pollingPins.size()-1; i >= 0; i--) {
                if (pin == pollingPins[i] && callback == pollingPinCallbacks[i]) {
                    pollingPins.erase(pollingPins.begin() + i);
                    pollingPinValues.erase(pollingPinValues.begin() + i);
                    pollingPinCallbacks.erase(pollingPinCallbacks.begin() + i);
                }
            }
        }

        bool removePollingCallback(int pin) {
            for (int i=pollingPins.size()-1; i >= 0; i--) {
                if (pin == pollingPins[i]) {
                    pollingPins.erase(pollingPins.begin() + i);
                    pollingPinValues.erase(pollingPinValues.begin() + i);
                    pollingPinCallbacks.erase(pollingPinCallbacks.begin() + i);
                }
            }
        }

        bool poll() {
            for (int i=0; i<pollingPins.size(); i++) {
                int prevValue = pollingPinValues[i];
                pollingPinValues[i] = getValue(pollingPins[i]);
                if (pollingPinValues[i] != prevValue) {
                    pollingPinCallbacks[i]();
                }
            }
        }

        //==============================================================
        // Get/Set Pin Values
        //==============================================================

        virtual bool getValue(int pin);
        virtual bool setValue(int pin, int value);
};

