/*
/////////////////////////////////////////////////////////////////////////

Simple program to turn operate a 5V SPDT relay

/////////////////////////////////////////////////////////////////////////
*/

int relayPin = 12; // Pin to control relay

void setup() {
  Serial.begin(9600);  // start serial port
  pinMode(relayPin, OUTPUT);  // Set pin for output to control relay 
}

void loop() {
  // Turn relay on/off with 1 sec delay. You shoudl hear a "click"
  digitalWrite(relayPin, HIGH);
  delay(1000);
  digitalWrite(relayPin, LOW);
  delay(1000);
}
