#include <vector>;
#include <Arduino.h>;

using namespace std;

class CapacitiveInputDeviceArrayBase {
    public:
        virtual bool poll();
        virtual int getPressedButton();
        virtual vector<vector<int>> getPressedButtons();
};
