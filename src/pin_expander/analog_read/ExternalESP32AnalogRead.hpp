#include "AnalogReadBase.hpp"

class ExternalESP32AnalogRead: public AnalogReadBase {
    // TODO: Provide support for pins without using an expander under a common interface
    public:
        ExternalESP32AnalogRead(int logicalPinNumberStart): AnalogReadBase(logicalPinNumberStart, std::vector<int>{
            // 36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 4, 2, 15
            0, 0, 4096, 0, 4096, 0, 0, 0, 0, 0, 0, 0, 4096, 4096, 4096, 4096, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 4096, 4096, 4096, 0, 0, 0, 0, 
            4096, 4096, 4096, 4096, 4096, 0, 0, 4096
        }) {

        }

        int getValue(int pin) {
            return FIXME(pin);
        }
};
