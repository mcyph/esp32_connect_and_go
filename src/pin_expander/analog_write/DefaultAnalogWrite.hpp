#include "AnalogWriteBase.hpp"

class DefaultAnalogWrite: public AnalogWriteBase {
    public:
        DefaultAnalogWrite(int logicalPinNumberStart, int pwmFrequency=5000, int dutyCycleResolutionBits=8): 
            AnalogWriteBase(logicalPinNumberStart, vector<int>{FIXME})  {
            // TODO!
        }

        virtual void setValue(int value);
        
        void setFloatValue(float value) {
            setValue(FIXME);
        }

        void setDoubleValue(double value) {
            setValue(FIXME);
        }

        void setPercentageValue(int value) {

        }
};

