#include <Arduino.h>;

typedef enum class InputDirection {
    NONE, 
    NORTH, NORTHEAST, 
    EAST, SOUTHEAST, 
    SOUTH, SOUTHWEST, 
    WEST, NORTHWEST
};

class DirectionalInputDeviceBase {
    public:
        virtual bool poll();
        virtual float getXValue();
        virtual float getYValue();
        virtual InputDirection getDirection();
};

