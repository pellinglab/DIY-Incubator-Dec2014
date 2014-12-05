/*
/////////////////////////////////////////////////////////////////////////////////

Simple code to test the operation of TWO DS18B20 temperature sensors.

Sensors are wired parasitically and connected to Arduino 5V, GND and Pin 12.

Code reports temperature from each sensor and the average over the serial port.

http://openenergymonitor.org/emon/buildingblocks/DS18B20-temperature-sensing

http://milesburton.com/Dallas_Temperature_Control_Library

/////////////////////////////////////////////////////////////////////////////////
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 12 // Sensor on digital Pin 12 of the Arduino
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with ANY OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
float T1, T2, Tavg;

void setup()
{
  Serial.begin(9600);   // start serial port
  sensors.begin();      // Start up the OneWire library
}

void loop()
{
  sensors.requestTemperatures(); // Send the command to get temperatures
  T1=sensors.getTempCByIndex(0); // Sensor 0
  T2=sensors.getTempCByIndex(1); // Sensor 1
  Tavg = (T1+T2)/2;
  Serial.print("Temperature for Sensor 0 is: ");
  Serial.println(T1);
  Serial.print("Temperature for Sensor 1 is: ");
  Serial.println(T2);
  Serial.print("Average Temperature: ");
  Serial.println(Tavg);
  Serial.println();
  delay(500);
}

