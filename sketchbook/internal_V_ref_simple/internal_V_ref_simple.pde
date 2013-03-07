const long InternalReferenceVoltage = 1083L;  // Change this to the reading from your internal voltage reference

void setup( void )
{
  Serial.begin( 38400 );
  Serial.println( "\r\n\r\n" );

  // REFS1 REFS0          --> 0 0 AREF, Internal Vref turned off
  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)
  // ADMUX = (0<<REFS1) | (0<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);

  // or for 328 processors:
  ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
}

void loop( void )
{
  int value;

  // Start a conversion  
  ADCSRA |= _BV( ADSC );
  
  // Wait for it to complete
  while( ( (ADCSRA & (1<<ADSC)) != 0 ) );

  // Scale the value
  value = (((InternalReferenceVoltage * 1023L) / ADC) + 5L) / 10L;

  Serial.println( value );
  delay( 1000 );
}
