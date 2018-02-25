/*
  esp32_gpio_output_test.ino
  try to configure a couple of difficult gpio PINS to output
*/

#include "driver/gpio.h"

// number of hardware pins  connected to anything, like piezzos to click ;)
#define CLICK_PULSES	12

uint8_t click_pin[CLICK_PULSES] =
  {23, 5, 17, 16, 4, 2, 15,     32, 33, 27, 14, 13, };
  // pins 2, 14,32, 33 need configuration FIXME!

void init_click_pins_OutLow() {		// make them GPIO, OUTPUT, LOW
  // see http://wiki.ai-thinker.com/_media/esp32/docs/esp32_chip_pin_list_en.pdf

  int pin;
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    pin=click_pin[pulse];
    Serial.write("gpio init "); Serial.print(pin); Serial.write('\t');

    if (GPIO_IS_VALID_OUTPUT_GPIO(pin) && (pin < 6 || pin > 11)) {
      Serial.write("GPIO_IS_VALID_OUTPUT_GPIO\t");

      Serial.write("PIN_FUNC_SELECT(..., PIN_FUNC_GPIO)\t");
      PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin], PIN_FUNC_GPIO);

/* Some other code fragments I have tried
   no one helped on pins 2, 32, 33, 14

    // Serial.write("gpio_set_direction\t");
    // gpio_set_direction((gpio_num_t) pin, GPIO_MODE_OUTPUT);

    // Serial.write(gpio_pad_select_gpio );
    // gpio_pad_select_gpio(pin);

    switch (pin) {	// code fragments from tests on individual pins
    case 2:
      pinMode(pin, FUNCTION_3); 					// does not help
      // PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_2], PIN_FUNC_GPIO);	// does not help
      break;
    case 14:
      // PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_14], PIN_FUNC_GPIO);	// does not help
      pinMode(pin, FUNCTION_3); 	// does not help on ESP32, but does on *ESP8266*
      break;
    case 32:
      // https://github.com/espressif/esp-idf/issues/143
      // gpio32 route to digital io_mux
      REG_CLR_BIT(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_MUX_SEL);	// does not help
      // PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_32], PIN_FUNC_GPIO);	// does not help
      // pinMode(pin, FUNCTION_3);				 	// does not help
      break;
    case 33:
      // https://github.com/espressif/esp-idf/issues/143
      REG_CLR_BIT(RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_MUX_SEL);	// does not help
      // PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_33], PIN_FUNC_GPIO);	// does not help
      // pinMode(pin, FUNCTION_3);				 	// does not help
      break;
    }
*/

    } else {
      Serial.write("invalid gpio for output\t");
      Serial.write(pin);
    }

    Serial.write("pinMode(pin, OUTPUT)\t");
    pinMode(pin, OUTPUT);

    Serial.println("digitalWrite(pin, LOW)");
    digitalWrite(pin, LOW);
  }

  gpio_config_t gpioConfig;
  gpioConfig.pin_bit_mask = (1 << 2) | (1 << 14) | (1 << 32) | (1 << 33);
  gpioConfig.mode = GPIO_MODE_OUTPUT;
  gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
  gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpioConfig.intr_type = GPIO_INTR_DISABLE;

  Serial.println("gpio_config(&gpioConfig);");
  Serial.println();
  gpio_config(&gpioConfig);
}


void setup() {
  Serial.begin(115200);
  while (!Serial) { yield(); }		// wait for Serial to open

  init_click_pins_OutLow();
}


void loop() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++)
    digitalWrite(click_pin[pulse],HIGH);
  delay(1234);

  for (int pulse=0; pulse<CLICK_PULSES; pulse++)
    digitalWrite(click_pin[pulse],LOW);
  delay(1234);

  if (Serial.available()) {
    init_click_pins_OutLow();	// show output again
    while(Serial.available())
      Serial.read();
  }
}
