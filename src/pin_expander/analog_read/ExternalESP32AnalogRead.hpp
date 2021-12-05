class LocalESP32_AD: public AnalogWriteBase, 
                public AnalogReadBase, 
                public DigitalWriteBase,
                public DigitalReadBase {
    // TODO: Provide support for pins without using an expander under a common interface
    public:
        long getNumAnalogInputPins() {
            return 15;
        }

        int* getAnalogInputPinNums() {
            static int pinNums[] = {
                36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 4, 2, 15
            };
            return pinNums;
        }
       
        long getAnalogInputMaxValue(int pinNum) {
            return 4095;
        }

        float analogReadFloat(int pinNum) {
            return analogReadLong(pinNum) / (float)getAnalogInputMaxValue(pinNum);
        }

        long analogReadLong(int pinNum) {
            return analogRead(pinNum);
        }
};
