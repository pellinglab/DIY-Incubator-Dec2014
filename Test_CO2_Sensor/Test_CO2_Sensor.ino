/*
/////////////////////////////////////////////////////////////////////////

Simple program to read from a COZIR SprintIR CO2 sensor and report over serial.

Sensor powered by Arduino 5V. Tx/Rx on Arduino Pins 2/3.

CO2 Meter (SprintIR 0-20% GC-0017):
http://www.co2meter.com/collections/co2-sensors/products/sprintir-100-percent-co2-sensor

Also need the latest COZIR library, version 1.0 used for this code.
Online forum has links to library and lots of information:
http://forum.arduino.cc/index.php?topic=91467.0

/////////////////////////////////////////////////////////////////////////
*/

#include "cozir.h"
#include "SoftwareSerial.h"

SoftwareSerial nss(2, 3); // (Tx, Rx from the sensor)
COZIR czr(nss);

float c, reading = 0;
float multiplier = 0.001;  // 10/10000 (Hardware multiplier/ppm conversion)
                           // For more details see sensor specificaiton sheet  
                           
void setup()
{
  Serial.begin(9600); 
  czr.SetOperatingMode(CZR_POLLING);
  delay(100);
}

void loop()
{
  c = czr.CO2(); // read the sensor, values output as ppm
  reading = c*multiplier; // convert ppm reading to percentage
  Serial.print("CO2 Content: ");
  Serial.print(reading);
  Serial.println("%");
  Serial.println();
  delay(50);  
}
