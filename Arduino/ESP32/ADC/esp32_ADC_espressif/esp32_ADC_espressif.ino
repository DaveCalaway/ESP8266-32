/*
  That example use the esp_adc_cal.h -https://goo.gl/emc9vU- to calibrate the ADC1.

  ADC Calibration dock: https://goo.gl/X8Ee7J
*/


#include <esp_adc_cal.h>
// Command to see the REF_VOLTAGE: espefuse.py --port /dev/ttyUSB0 adc_info
// or dc2_vref_to_gpio(25)
#define REF_VOLTAGE 1128
#define READPIN ADC1_CHANNEL_0

esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t;


// ======== avgAnalogRead ========
uint16_t avgAnalogRead(uint16_t samples = 8) {
  uint32_t sum = 0;
  for (int x = 0; x < samples; x++) {
    sum += adc1_get_raw(READPIN);
  }
  sum /= samples;
  return esp_adc_cal_raw_to_voltage(sum, adc_chars);
}


// ======== SETUP ========
void setup() {
  Serial.begin(115200);

  // Fix resolution for the all adc1
  adc1_config_width(ADC_WIDTH_BIT_11);

  /*
    - 0dB attenuaton (ADC_0db) gives full-scale voltage 1.1V
    - 2.5dB attenuation (ADC_2_5db) gives full-scale voltage 1.5V
    - 6dB attenuation (ADC_6db) gives full-scale voltage 2.2V
    - 11dB attenuation (ADC_11db) gives full-scale voltage 3.9V
  */
  adc1_config_channel_atten(READPIN, ADC_ATTEN_DB_11);

  // Calibration function
  esp_adc_cal_value_t val_type =
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, REF_VOLTAGE, adc_chars);
}


// ======== LOOP ========
void loop() {
  Serial.println(avgAnalogRead());
  delay(5000);
}
