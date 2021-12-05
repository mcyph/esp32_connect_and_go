class ADS1115: public AnalogReadBase {
    public:
        long getNumAnalogInputPins() {
            return 4;
        }

        int* getAnalogInputPinNums() {
            static int pinNums[] = {
                0, 1, 2, 3
            };
            return pinNums;
        }

        long getAnalogInputMaxValue(int pinNum) {
            return 65535;
        };

        float analogReadFloat(int pinNum) {
            return analogReadLong(pinNum) / (float)getAnalogInputMaxValue(pinNum);
        }

        long analogReadLong(int pinNum) {
            // TODO!
        }
};
