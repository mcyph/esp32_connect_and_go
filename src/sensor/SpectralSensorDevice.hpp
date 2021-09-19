#include <vector>;
#include <Wire.h>;
#include <Arduino.h>;
#include <string.h>;
#include <stdexcept>;
#include "ArduinoJson.h";
#include "Adafruit_TCS34725.h";
#include <Adafruit_AS7341.h>;
#include "Adafruit_AS726x.h";
#include "SparkFun_AS7265X.h";
#include "SensorDevice.hpp";

class AdafruitTCS34725: public SensorDeviceBase {
    private:
        Adafruit_TCS34725 tcs;

    public:
        AdafruitTCS34725(int i2cAddress=0x29): SensorDeviceBase{vector<String>{"Color Temperature", "Lux", "Red", "Green", "Blue", "Clear"}} {
            tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
            if (!tcs.begin(i2cAddress)) {
                throw std::runtime_error("Could not connect to Adafruit TCS34725 sensor! Please check your wiring.");
            }
        };

        bool poll(DynamicJsonDocument doc) override {
            uint16_t r, g, b, c, colorTemp, lux;

            tcs.getRawData(&r, &g, &b, &c);
            colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
            lux = tcs.calculateLux(r, g, b);

            doc["TCS34725"]["Color Temperature"] = colorTemp; // In K
            doc["TCS34725"]["Lux"] = lux;
            doc["TCS34725"]["Red"] = r;
            doc["TCS34725"]["Green"] = g;
            doc["TCS34725"]["Blue"] = b;
            doc["TCS34725"]["Clear"] = c;
            return true;
        };
};

class SparkFunAS7265X: public SensorDeviceBase {
    private:
        AS7265X sensor;

    public:
        SparkFunAS7265X(): SensorDeviceBase(vector<String>{"410", "435", "460", "485", "510", "535", "560", "585", "610", 
                                                           "645", "680", "705", "730", "760", "810", "860", "900", "940"}) {
            sensor = AS7265X();
            if (sensor.begin() == false) {
                throw std::runtime_error("Could not connect to SparkFun AS7265X sensor! Please check your wiring.");
            }
        };

        bool poll(DynamicJsonDocument doc) override {
            // This is a hard wait while all 18 channels are measured
            sensor.takeMeasurements();

            doc["SparkFunAS7265X"]["410"] = sensor.getCalibratedA(); //410nm
            doc["SparkFunAS7265X"]["435"] = sensor.getCalibratedB(); //435nm
            doc["SparkFunAS7265X"]["460"] = sensor.getCalibratedC(); //460nm
            doc["SparkFunAS7265X"]["485"] = sensor.getCalibratedD(); //485nm
            doc["SparkFunAS7265X"]["510"] = sensor.getCalibratedE(); //510nm
            doc["SparkFunAS7265X"]["535"] = sensor.getCalibratedF(); //535nm

            doc["SparkFunAS7265X"]["560"] = sensor.getCalibratedG(); //560nm
            doc["SparkFunAS7265X"]["585"] = sensor.getCalibratedH(); //585nm
            doc["SparkFunAS7265X"]["610"] = sensor.getCalibratedR(); //610nm
            doc["SparkFunAS7265X"]["645"] = sensor.getCalibratedI(); //645nm
            doc["SparkFunAS7265X"]["680"] = sensor.getCalibratedS(); //680nm
            doc["SparkFunAS7265X"]["705"] = sensor.getCalibratedJ(); //705nm

            doc["SparkFunAS7265X"]["730"] = sensor.getCalibratedT(); //730nm
            doc["SparkFunAS7265X"]["760"] = sensor.getCalibratedU(); //760nm
            doc["SparkFunAS7265X"]["810"] = sensor.getCalibratedV(); //810nm
            doc["SparkFunAS7265X"]["860"] = sensor.getCalibratedW(); //860nm
            doc["SparkFunAS7265X"]["900"] = sensor.getCalibratedK(); //900nm
            doc["SparkFunAS7265X"]["940"] = sensor.getCalibratedL(); //940nm
            return true;
        };
};

class AdafruitAS7341: public SensorDeviceBase {
    private:
        Adafruit_AS7341 as7341;

    public:
        AdafruitAS7341(int i2cAddress=57, int sensorId=0): SensorDeviceBase(vector<String>{"F1 415nm", "F2 445nm", "F3 480nm", "F4 515nm", "F5 555nm", 
                                                                                           "F6 590nm", "F7 630nm", "F8 680nm", "Clear", "Near IR"}) {
            as7341 = Adafruit_AS7341();
            if (!as7341.begin(i2cAddress, &Wire, sensorId)){
                throw std::runtime_error("Could not connect to Adafruit AS7341 sensor! Please check your wiring.");
            }
            as7341.setATIME(100);
            as7341.setASTEP(999);
            as7341.setGain(AS7341_GAIN_256X);
        }

        bool poll(DynamicJsonDocument doc) override {
            // Read all channels at the same time and store in as7341 object
            if (!as7341.readAllChannels()){
                Serial.println("Error reading all channels!");
                return false;
            }

            doc["AdafruitAS7341"]["F1 415nm"] = as7341.getChannel(AS7341_CHANNEL_415nm_F1);
            doc["AdafruitAS7341"]["F2 445nm"] = as7341.getChannel(AS7341_CHANNEL_445nm_F2);
            doc["AdafruitAS7341"]["F3 480nm"] = as7341.getChannel(AS7341_CHANNEL_480nm_F3);
            doc["AdafruitAS7341"]["F4 515nm"] = as7341.getChannel(AS7341_CHANNEL_515nm_F4);
            doc["AdafruitAS7341"]["F5 555nm"] = as7341.getChannel(AS7341_CHANNEL_555nm_F5);
            doc["AdafruitAS7341"]["F6 590nm"] = as7341.getChannel(AS7341_CHANNEL_590nm_F6);
            doc["AdafruitAS7341"]["F7 630nm"] = as7341.getChannel(AS7341_CHANNEL_630nm_F7);
            doc["AdafruitAS7341"]["F8 680nm"] = as7341.getChannel(AS7341_CHANNEL_680nm_F8);
            doc["AdafruitAS7341"]["Clear"] = as7341.getChannel(AS7341_CHANNEL_CLEAR);
            doc["AdafruitAS7341"]["Near IR"] = as7341.getChannel(AS7341_CHANNEL_NIR);
            return true;
        }
};

class AdafruitAS726X: public SensorDeviceBase {
    private:
        Adafruit_AS726x ams;
        uint16_t sensorValues[AS726x_NUM_CHANNELS];

        //buffer to hold calibrated values (not used by default in this example)
        //float calibratedValues[AS726x_NUM_CHANNELS];

    public:
        AdafruitAS726X(int i2cAddress=73): SensorDeviceBase(vector<String>{"Temp", "Violet", "Blue", "Green", 
                                                                           "Yellow", "Orange", "Red"}) {
            ams = Adafruit_AS726x(i2cAddress);
            if (!ams.begin()) {
                throw std::runtime_error("Could not connect to Adafruit AS726X sensor! Please check your wiring.");
            }
        };

        bool poll(DynamicJsonDocument doc) override {
            // Enable the driver LED
            //ams.drvOn();
            ams.startMeasurement();
            while (!ams.dataReady()) delay(5);
            //ams.drvOff();

            // Read the values
            ams.readRawValues(sensorValues);
            //ams.readCalibratedValues(calibratedValues);

            doc["AdafruitAS726X"]["Temp"] = ams.readTemperature();  // device temperature
            doc["AdafruitAS726X"]["Violet"] = sensorValues[AS726x_VIOLET];
            doc["AdafruitAS726X"]["Blue"] = sensorValues[AS726x_BLUE];
            doc["AdafruitAS726X"]["Green"] = sensorValues[AS726x_GREEN];
            doc["AdafruitAS726X"]["Yellow"] = sensorValues[AS726x_YELLOW];
            doc["AdafruitAS726X"]["Orange"] = sensorValues[AS726x_ORANGE];
            doc["AdafruitAS726X"]["Red"] = sensorValues[AS726x_RED];
            return true;
        };
};
