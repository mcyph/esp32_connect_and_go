// https://circuitdigest.com/microcontroller-projects/esp32-real-time-clock-using-ds3231-module
#include "RTClib.h";

// SparkFun RV-8803

class RTCBase {
    public:
        virtual DateTime getDateTime();
        virtual FIXME getUnixTime();
        virtual int getTemperature();
};

class DS3231: RTCBase { // (HW84)
    private:
        // Date and time functions using a DS3231 RTC connected via I2C and Wire lib
        RTC_DS3231 rtc;

    public:
        DS3231() {
            Serial.begin(57600);

            #ifndef ESP8266
                while (!Serial); // wait for serial port to connect. Needed for native USB
            #endif

            if (! rtc.begin()) {
                Serial.println("Couldn't find RTC");
                Serial.flush();
                abort();
            }

            if (rtc.lostPower()) {
                Serial.println("RTC lost power, let's set the time!");
                // When time needs to be set on a new device, or after a power loss, the
                // following line sets the RTC to the date & time this sketch was compiled
                rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
                // This line sets the RTC with an explicit date & time, for example to set
                // January 21, 2014 at 3am you would call:
                // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
            }

            // When time needs to be re-set on a previously configured device, the
            // following line sets the RTC to the date & time this sketch was compiled
            // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            // This line sets the RTC with an explicit date & time, for example to set
            // January 21, 2014 at 3am you would call:
            // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
        };

        DateTime getDateTime() override {
            DateTime now = rtc.now();
            return now;
        };

        FIXME getUnixTime() override {
            return getDateTime().unixtime();
        }

        int getTemperature() override {
            return rtc.getTemperature();
        }
};
