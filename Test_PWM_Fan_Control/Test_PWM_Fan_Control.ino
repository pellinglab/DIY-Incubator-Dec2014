/*
/////////////////////////////////////////////////////////////////////////

Simple program to turn on/off 12V DC fan and control speed with PWM

/////////////////////////////////////////////////////////////////////////
*/

int TIP120pin = 11; // Pin to control fan speed

void setup()
{
  Serial.begin(9600);   // start serial port
  pinMode(TIP120pin, OUTPUT); // Set pin for output to control TIP120 base pin
}

void loop()
{
  // PWM control varies from 0, 127, 254 to control Fan.
  // Fan off (5sec), Fan at half speed (5sec), Fan at full speed (5sec).
  for (int i = 0; i<3; i++) {
    analogWrite(TIP120pin, 127*i);
    delay(5000);
  }
}


