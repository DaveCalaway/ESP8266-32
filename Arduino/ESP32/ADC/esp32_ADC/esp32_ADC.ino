/*
   ESP32 ADC

   All time ADC pins:
   ADC1_CH0,ADC1_CH03,ADC1_CH04
   ADC1_CH05,ADC1_CH06,ADC1_CH07

   Use only without WiFi:
   ADC2_CH0,ADC2_CH01,ADC2_CH02,ADC2_CH03
   ADC2_CH04,ADC2_CH05,ADC2_CH06
   ADC2_CH07,ADC2_CH08,ADC2_CH09

   Arduino espressif doc: https://goo.gl/NpUo3Z
   Espressif doc: https://goo.gl/hcUy5U
   GPIO: https://goo.gl/k8FGGD

   Davide Gariselli
 */

#include <esp_adc_cal.h>

// Command to see the REF_VOLTAGE: espefuse.py --port /dev/ttyUSB0 adc_info
// or dc2_vref_to_gpio(25)
#define REF_VOLTAGE 1128

esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t;


// ========= analogRead_cal =========
int analogRead_cal(uint8_t channel,adc_atten_t attenuation){
        adc1_channel_t channelNum;

        /*
         * Set number of cycles per sample
         * Default is 8 and seems to do well
         * Range is 1 - 255
         * */
        // analogSetCycles(uint8_t cycles);

        /*
         * Set number of samples in the range.
         * Default is 1
         * Range is 1 - 255
         * This setting splits the range into
         * "samples" pieces, which could look
         * like the sensitivity has been multiplied
         * that many times
         * */
        // analogSetSamples(uint8_t samples);

        switch (channel) {
        case (36):
                channelNum = ADC1_CHANNEL_0;
                break;

        case (39):
                channelNum = ADC1_CHANNEL_3;
                break;

        case (34):
                channelNum = ADC1_CHANNEL_6;
                break;

        case (35):
                channelNum = ADC1_CHANNEL_7;
                break;

        case (32):
                channelNum = ADC1_CHANNEL_4;
                break;

        case (33):
                channelNum = ADC1_CHANNEL_5;
                break;
        }

        adc1_config_channel_atten(channelNum,attenuation);
        return esp_adc_cal_raw_to_voltage(analogRead(channel), adc_chars);
}


// ========= SETUP =========
void setup() {
        Serial.begin(115200);

        // -- Fixed for the all adc1 ---
        // 4095 for 12-bits -> VDD_A / 4095 = 805uV  too jittery
        // 2048 for 11-bits -> 3.9 / 2048 = 1.9mV looks fine
        /*
           Set the resolution of analogRead return values. Default is 12 bits (range from 0 to 4096).
           If between 9 and 12, it will equal the set hardware resolution, else value will be shifted.
           Range is 1 - 16
         */
        analogReadResolution(11); // https://goo.gl/qwUx2d

        // Calibration function
        esp_adc_cal_value_t val_type =
                esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, REF_VOLTAGE, adc_chars);
}


// ========= LOOP =========
void loop() {
        // ====== Read the 4 = ADC1_CHANNEL_0 = GPIO36 ======
        Serial.print("Pin 36: ");
        /*
           The maximum voltage is limited by VDD_A
           - 0dB attenuaton (ADC_ATTEN_DB_0) gives full-scale voltage 1.1V
           - 2.5dB attenuation (ADC_ATTEN_DB_2_5) gives full-scale voltage 1.5V
           - 6dB attenuation (ADC_ATTEN_DB_6) gives full-scale voltage 2.2V
           - 11dB attenuation (ADC_ATTEN_DB_11) gives full-scale voltage 3.9V
         */
        // Default is 11db, full scale
        Serial.print(analogRead_cal(36,ADC_ATTEN_DB_11));
        Serial.println(" mV");

        delay(4000);
}
