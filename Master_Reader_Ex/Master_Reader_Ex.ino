// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" examples for use with this

// Created 29 March 2006

// This example code is in the public domain

#include <Wire.h>

char incoming_data;

void setup() 
{
  Wire.begin();         // join i2c bus (address optional for master)
  Serial.begin(9600);   // start serial for output
}

void loop() 
{
  Wire.requestFrom(8,32);

  while (Wire.available())    // slave may send less than requested
  {
    int dist = Wire.read();
    Serial.print(dist);
    
    incoming_data = Wire.read();
    Serial.println(incoming_data);
  }
  delay(500);
}

