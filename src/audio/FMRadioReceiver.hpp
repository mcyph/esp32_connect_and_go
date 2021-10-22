#include <Wire.h>
#include <radio.h>
#include <RDA5807M.h>
#include <SI4703.h>
#include <SI4705.h>
#include <TEA5767.h>
#include <RDSParser.h>
#include <functional>

typedef enum class RadioDriverType {
    RDA5807M, SI4703, SI4705, TEA5767
};

class FMRadioReceiver {
    private:
        // RDS (https://en.wikipedia.org/wiki/Radio_Data_System) parser instance
        //RDSParser rds;
        RADIO * radio;

    public:
        FMRadioReceiver(RadioDriverType radioDriverType=RadioDriverType::TEA5767) {
            if (radioDriverType == RadioDriverType::RDA5807M) {
                radio = new RDA5807M();
            } else if (radioDriverType == RadioDriverType::SI4703) {
                radio = new SI4703();
            } else if (radioDriverType == RadioDriverType::SI4705) {
                radio = new SI4705();
            } else if (radioDriverType == RadioDriverType::TEA5767) {
                radio = new TEA5767();
            }

            radio->init();
            radio->debugEnable();

            this->setBandFrequency(100.7);
            this->setMono(true);
            this->setMute(false);
            // radio._wireDebug();
            this->setVolume(8);
            this->setBassBoost(true);

            // Setup the information chain for RDS data.
            // TODO: Figure out how to pass a class instance fn!
            //radio->attachReceiveRDS(fn);
        }

        //void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
        //    rds.processData(block1, block2, block3, block4);
        //}

        void setMono(bool enabled) {
            radio->setMono(true);
        }

        void setMute(bool enabled) {
            radio->setMute(false);
        }

        void setVolume(int volume) {
            radio->setVolume(volume);
        }

        void setBassBoost(bool enabled) {
            radio->setBassBoost(enabled);
        }

        void setBandFrequency(double frequency, RADIO_BAND radioBand=RADIO_BAND_FM) {
            radio->setBandFrequency(radioBand, round(frequency * 100));
        }
};
