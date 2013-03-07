int battVolts;   // made global for wider avaliblity throughout a sketch if needed, example for a low voltage alarm, etc
		     // value is volts X 100, 5 vdc = 500

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
     // For mega boards
     const long InternalReferenceVoltage = 1115L;  // Adust this value to your boards specific internal BG voltage x1000
#else
     // For 168/328 boards
     const long InternalReferenceVoltage = 1050L;  // Adust this value to your boards specific internal BG voltage x1000
#endif

void setup(void)
    {
     Serial.begin(38400);
     // Serial.print("volts X 100");
     // Serial.println( "\r\n\r\n" );
     // delay(100);
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
     // For mega boards
//     const long InternalReferenceVoltage = 1115L;  // Adust this value to your boards specific internal BG voltage x1000
	  // REFS1 REFS0	    --> 0 1, AVcc internal ref.
	  // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
#else
     // For 168/328 boards
//     const long InternalReferenceVoltage = 1050L;  // Adust this value to your boards specific internal BG voltage x1000
	  // REFS1 REFS0	    --> 0 1, AVcc internal ref.
	  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
#endif
    }

void loop(void)
    {
     for (int i=0; i <= 3; i++) battVolts=getBandgap();  //4 readings required for best stable value?
     Serial.print("Battery Vcc centivolts =  ");
     Serial.println(battVolts);
     // Serial.print("Analog pin 0 voltage = ");
     // Serial.println(map(analogRead(0), 0, 1023, 0, battVolts));
     // Serial.println();
     delay(1000);
    }

int getBandgap(void)
    {

//	#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//	     // For mega boards
//	     const long InternalReferenceVoltage = 1115L;  // Adust this value to your boards specific internal BG voltage x1000
//		  // REFS1 REFS0	    --> 0 1, AVcc internal ref.
//		  // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)
//	     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
//	#else
//	     // For 168/328 boards
//	     const long InternalReferenceVoltage = 1050L;  // Adust this value to your boards specific internal BG voltage x1000
//		  // REFS1 REFS0	    --> 0 1, AVcc internal ref.
//		  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)
//	     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
//	#endif
	  // Start a conversion
     ADCSRA |= _BV( ADSC );
	  // Wait for it to complete
     while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
	  // Scale the value
     int results = (((InternalReferenceVoltage * 1023L) / ADC) + 5L) / 10L;
     return results;
    }
