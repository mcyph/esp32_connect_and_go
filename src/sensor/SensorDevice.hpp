#include <vector>;
#include <Wire.h>;
#include <Arduino.h>;
#include <string.h>;
#include <stdexcept>;
#include "ArduinoJson.h";

#include "Adafruit_CCS811.h";
#include "SparkFunBME280.h";
#include "SparkFunCCS811.h";

using namespace std;

class SensorDeviceBase {
    private:
        vector<String> fieldNames;

    public:
        SensorDeviceBase(vector<String> fieldNames) {
            this->fieldNames = fieldNames;
        };
        
        const vector<String> getfieldNames() {
            return this->fieldNames;
        };

        virtual bool poll(DynamicJsonDocument doc);
};

class AdafruitCCS811: public SensorDeviceBase {
    private:
        Adafruit_CCS811 ccs;

    public:
        AdafruitCCS811(int i2cAddress=90): SensorDeviceBase(vector<String>{"CO2", "TVOC_PPM"}) {
            ccs = Adafruit_CCS811();
            if (!ccs.begin(i2cAddress)) {
                throw std::runtime_error("Could not connect to Adafruit CCS811 sensor! Please check your wiring.");
            }
        };

        bool poll(DynamicJsonDocument doc) override {
            while (!ccs.available());  // CHECK ME!

            if (!ccs.readData()) {
                doc["AdafruitCCS811"]["CO2"] = ccs.geteCO2();
                doc["AdafruitCCS811"]["TVOC_PPM"] = ccs.getTVOC();
                return true;
            } else {
                return false;
            }
        };
};

class SparkFunCCS811: public SensorDeviceBase {
    private:
        CCS811 mySensor;

    public:
        // #define CCS811_ADDR 0x5B //Default I2C Address
        // #define CCS811_ADDR 0x5A //Alternate I2C Address
        SparkFunCCS811(int i2cAddress=0x5B): SensorDeviceBase(vector<String>{"CO2", "tVOC"}) {
            mySensor = CCS811(i2cAddress);
            if (!mySensor.begin()) {
                throw std::runtime_error("Could not connect to SparkFun CCS811 sensor! Please check your wiring.");
            }
        };

        bool poll(DynamicJsonDocument doc) override {
            if (mySensor.dataAvailable()) {
                mySensor.readAlgorithmResults();
                doc["SparkFunCCS811"]["CO2"] = mySensor.getCO2();
                doc["SparkFunCCS811"]["tVOC"] = mySensor.getTVOC();
                return true;
            } else {
                return false;
            }
        };
};

class SparkFunBME280: public SensorDeviceBase {
    private:
        BME280 mySensor;
    
    public:
        SparkFunBME280(): SensorDeviceBase(vector<String>{"Humidity", "Pressure", "AltitudeMeters", 
                                                          "AltitudeFeet", "TempC", "TempF"}) {
            mySensor = BME280();
            if (!mySensor.beginI2C()) {
                throw std::runtime_error("Could not connect to SparkFun BME280 sensor! Please check your wiring.");
            }
            // Adjust the sea level pressure used for altitude calculations
            mySensor.setReferencePressure(101200);
        };

        bool poll(DynamicJsonDocument doc) override {
            doc["BME280"]["Humidity"] = mySensor.readFloatHumidity();
            doc["BME280"]["Pressure"] = mySensor.readFloatPressure();
            doc["BME280"]["AltitudeMeters"] = mySensor.readFloatAltitudeMeters();
            doc["BME280"]["AltitudeFeet"] = mySensor.readFloatAltitudeFeet();
            doc["BME280"]["TempC"] = mySensor.readTempC();
            doc["BME280"]["TempF"] = mySensor.readTempF();
            return true;
        };
};

class Neo6MGPS: public SensorDeviceBase {
    private:

    public:
        Neo6MGPS(): SensorDeviceBase(vector<String>{"Latitude", "Longitude"}) {
        };

        bool poll(DynamicJsonDocument doc) override {
        };
};
