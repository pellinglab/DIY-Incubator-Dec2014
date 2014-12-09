/*
DIY CO2 INCUBATOR FOR MAMMALIAN CELL CULUTRE (ARDUINO 1.06 and UNO R3)

DECEMBER 2014. 
WRITTEN BY ANDREW E. PELLING 
http://www.pellinglab.net 

Simple incubator with 12V resistive heaters, 12V normally closed solenoid and CO2 Sensor. 
VERY simple relay control over heating and solenoid valve on CO2 tank. 

CO2 Meter (SprintIR 0-20% GC-0017):
http://www.co2meter.com/collections/co2-sensors/products/sprintir-100-percent-co2-sensor

Also need the latest COZIR library, download from this thread (Using Cozir 0-1-01.zip in this code):
http://forum.arduino.cc/index.php?topic=91467.0

Temperature Sensors:
DS18B20 Digital Temp Sensors wired parasitically.
Useful for having more than one sensor on a single Arduino Pin.
http://openenergymonitor.org/emon/buildingblocks/DS18B20-temperature-sensing

Sensors require DallasTemperature.h Library here:
http://milesburton.com/Dallas_Temperature_Control_Library

Possible improvements could be to utilize PID library and PWM control the heaters.

CO2 supplied by BOC Gas, SodaStream or Paintball CO2 tanks. 

Arduino UNO Pin Connections:
Pin 2,3: Tx/Rx from CO2 Sensor
Pin #5: Inidicator LED
Pin #6: Temperature Sensors
Pin #7: Heater Relay
Pin #8: Solenoid Relay
Pin #9: PWM Fan Control
*/

///////// TEMP AND CO2 CONTROL ///////// 
// Control works by reading a sensor (temperature or CO2) 3 times and averaging (to flatten out noise)
// If the temperature is below the setpoint, the heaters turn on otherwise they are off
// If the CO2 is below the setpoint, the solenoid opens for a given DURATION and then closes
// The cycle repeats until the setpoints are reached. 

// Setpoints and Durations MUST be set according to your own system. TRIAL AND ERROR.
// May need to increase/decrease depending on size/shape of incubator, CO2 flow rate, etc.
// Ideal is 37.0 and 5.0 (Temp and CO2).
float TSetpoint = 36.9; // Temperature in Celsius
float CO2Setpoint = 4.8; // CO2 level in %
float SolenoidOnTime = 200; // Time in milliseconds

////////* LIBRARIES  *////////
// DS18B20 Temp Sensors
#include "OneWire.h"
#include "DallasTemperature.h"

// CO2 Sensors
#include "SoftwareSerial.h"
#include "cozir.h"

// Adafruit 7-segment Display for updating CO2 and Temperature
#include "Wire.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

////////* RELAY and FAN CONTROL *////////
int HeatingIndicatorLED = 5; // indicator LED for heaters
int Heater = 7;  // Relay for controlling power to heaters
int Solenoid = 8; // Relay for controlling 12V solenoid valve
int fan = 9; // PWM control over 12V fan

////////* ONE WIRE DS18B20 TEMP SENSORS  *////////
#define ONE_WIRE_BUS 6 // Sensors on digital pin 4 of the Arduino
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with ANY OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
float T1, T2, AvgT, SingleT  = 0;

////////* SPRINTIR COZIR CO2 SENSOR  *////////
SoftwareSerial nss(2,3); // Rx,Tx - Pin 5,7 on sensor
COZIR czr(nss);
float SingleCO2, CO2 = 0;
float multiplier = 0.001;  // 10/10000 (Hardware multiplier/ppm conversion).  
float reading = 0;

///////// MATRIX LED DISPLAY SETUP ///////// 
Adafruit_7segment matrix = Adafruit_7segment();
long previousMillis = 0;        // will store last time LED was updated
int update = 0;

void setup()
{
  Serial.begin(9600);   // Start serial port
  //czr.SetOperatingMode(CZR_POLLING);  // Start the CO2 sensor and put into POLLING mode
  sensors.begin();      // Start up the temperature sensor library
  matrix.begin(0x70);   // Start Matrix
  matrix.print(8888);   // Print '8888' on matrix
  matrix.writeDisplay();
  pinMode(fan, OUTPUT);          // Set pin for output to control TIP120 Base pin
  pinMode(Heater, OUTPUT);       // Sets pin for controlling heater relay
  digitalWrite(Heater, LOW);     // Set LOW (heater off)
  pinMode(Solenoid, OUTPUT);     // Sets pin for controlling solenoid relay
  digitalWrite(Solenoid, LOW);   // Set LOW (solenoid closed off)
  pinMode(HeatingIndicatorLED, OUTPUT);    // LED for indicating heating is on
  digitalWrite(HeatingIndicatorLED, LOW);  // Turn LED off
  analogWrite(fan, 255);  // Turn on the Fan
  delay(2000);  // Wait
}

void loop()
{
  // Read the average temperature over both sensors 3 times, then find the average.
  for (int i = 0; i<3; i++) {
    sensors.requestTemperatures();
    T1=sensors.getTempCByIndex(0);
    T2=sensors.getTempCByIndex(1);
    SingleT += (T1+T2)/2;
  }
  
  AvgT = SingleT/3;
  SingleT = 0;
  
  // Turn on/off heater based on temperature reading
  // Heater turns on for a duration of 'TempOnTime' and then turns off
  if (AvgT < TSetpoint) {
    digitalWrite(Heater, HIGH);
    analogWrite(HeatingIndicatorLED, 255);
  } else if (AvgT > TSetpoint) {
    digitalWrite(Heater, LOW);
    analogWrite(HeatingIndicatorLED, 0);
  } 

  // Read CO2 sensor 3 times and determine the average.
  for (int i = 0; i<3; i++) {
    SingleCO2 += czr.CO2()*multiplier;
  }
  CO2 = SingleCO2/3;
  SingleCO2 = 0;
    
  // Open/close the solenoid valve based on the current CO2 reading 
  // Solenoid  opens for a duration of 'SolenoidOnTime' and then closes
  if (CO2 < CO2Setpoint) {
    digitalWrite(Solenoid, HIGH);
    delay(SolenoidOnTime);
    digitalWrite(Solenoid, LOW);
  } else if (CO2 > CO2Setpoint) {
    digitalWrite(Solenoid, LOW);
  } 

  updateMatrix(); // Update the Matrix display, 
  
  // SerialPrintResults(); // UNCOMMENT to print results over Serial port for debugging
}

void updateMatrix() {
  unsigned long currentMillis = millis();
  
  // Alternate the display between Temperature and CO2 every 2 seconds
  if (currentMillis - previousMillis > 2000) {
    previousMillis = currentMillis;
    if (update == 0) {
      matrix.print(AvgT, 1);
      matrix.writeDisplay();
      update = 1;
    } else if (update == 1) {
      matrix.print(CO2, 1);
      matrix.writeDisplay();
      update = 0;
    }
  }
}

// For debugging only, uncomment line 151
void SerialPrintResults()
{
  Serial.println("-----------------------------------------------");
  Serial.println();
  Serial.print("Temperature for Device 0 is: ");
  Serial.print(T1);
  Serial.println("*C");
  Serial.print("Temperature for Device 1 is: ");
  Serial.print(T2);
  Serial.println("*C");
  Serial.println();
  Serial.print("Average Temperature is: ");
  Serial.print(AvgT);
  Serial.println("*C");
  Serial.println();
  Serial.print("CO2 Content is: ");
  Serial.print(CO2);
  Serial.println("%");
  Serial.println();
  Serial.println("-----------------------------------------------");
  Serial.println();  
}



