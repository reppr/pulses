/*
  boards_layout/G15-T01-esp32_dev.h
*/

#undef GPIO_PINS
#define GPIO_PINS	15

gpio_pin_t gpio_pins[GPIO_PINS] =				//  ESP32_G15_T01
  {23, 19, 18, 5, 17, 16, 4, 0, 2, 15, 32, 33, 27, 14, 13};

/* ****************************************************************
[|- "WIFI" side] left

3.3V    +3.3
EN	RESET				(pu)
GPIO36				ADC0			SVP
GPIO39				ADC3			SVN
GPIO34				ADC6
GPIO35				ADC7
GPIO32	G10			ADC4	TOUCH9			(CS MICROSD)
GPIO33	G11			ADC5	TOUCH8
GPIO25	 DAC			ADC18			DAC1	(I2S codec LRCK)
GPIO26	 DAC			ADC19			DAC2	(I2S codec BCK)
GPIO27	G12			ADC17	TOUCH7			(SCK MICROSD)
GPIO14	G13	 HSPI SCK ADC16	TOUCH6	TMS		(MOSI MICROSD)
GPIO12	  T	 HSPI MISO ADC15	TOUCH5	TDI	(pd)	(MISO MICROSD)
GND	-
GPIO13	G14	 HSPI MOSI ADC14	TOUCH4	TCK
GPIO09 FLASH D2
GPIO10 FLASH D3
GPIO11 FLASH CMD
5V
[|- "USB" side]

-------------------------------------------------------------
["WIFI" side -|] right

GND	-
GPIO23	G0	  VSPI MOSI				SPI MOSI
GPIO22	   i2c SCL					Wire SCL	(I2S codec DATA)
GPIO01	       TX0					Serial TX
GPIO03	       RX0					Serial RX
GPIO21	   i2c SDA							Wire SDA
GND	-
GPIO19	G1		VSPI MISO				SPI MISO
GPIO18	G2		VSPI SCK				SPI SCK
GPIO05	G3		VSPI SS				(pu)	SPI SS
GPIO17	G4
GPIO16	G5
GPIO04	G6			ADC10	TOUCH0		(pd)
GPIO00	G7	BOOT		ADC11	TOUCH1		(pu)
GPIO02	G8	LED	ADC12	TOUCH2		(pd)
GPIO15	G9		HSPI SS	ADC13	TOUCH3	TDO	(pu)
GPIO08 FLASH D1
GPIO07 FLASH D0
GPIO06 FLASH SCK
["USB" side -|]

-------------------------------------------------------------
25 audio	26 audio
22 i2c SCL	21 i2c SDA
*/

/* **************************************************************** */
