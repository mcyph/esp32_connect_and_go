class PinExpanderBase {
    public:
        virtual bool isDigitalWriteDevice() {}
        virtual bool isDigitalReadDevice() {}
        virtual bool isAnalogReadDevice() {}
        virtual bool isDigitalReadDevice() {}

        virtual bool isDigitalWritePin(int pin) {}
        virtual bool isDigitalReadPin(int pin) {}
        virtual bool isAnalogReadPin(int pin) {}
        virtual bool isDigitalReadPin(int pin) {}
};
