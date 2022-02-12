class PinExpanderBase {
    public:
        virtual bool isDigitalWriteDevice();
        virtual bool isDigitalReadDevice();
        virtual bool isAnalogWriteDevice();
        virtual bool isAnalogReadDevice();

        virtual bool isDigitalWritePin(int pin);
        virtual bool isDigitalReadPin(int pin);
        virtual bool isDigitalInterruptPin(int pin);
        virtual bool isAnalogWritePin(int pin);
        virtual bool isAnalogReadPin(int pin);
};
