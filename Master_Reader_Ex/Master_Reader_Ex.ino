// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" examples for use with this

// Created 29 March 2006

// This example code is in the public domain

#include <Wire.h>

void setup() 
{
  Wire.begin();         // join i2c bus (address optional for master)
}

byte x = 0;

void loop() 
{
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write("x is ");       // sends five bytes
  Wire.write(x);             // sends on byte
  Wire.endTransmission();    // stop transmitting

  x++;
  delay(500);
}
