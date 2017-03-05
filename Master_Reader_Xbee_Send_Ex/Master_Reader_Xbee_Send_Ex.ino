// Wire Master Reader + Xbee Transmit

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 13 November 2016

#include <Wire.h>

void setup() 
{
  Wire.begin();         // join i2c bus (address optional for master)
  Serial.begin(9600);   // start serial for output and input
}

void loop() 
{
  Wire.requestFrom(8, 23);       // request 6 bytes from slave device #8

  while (Wire.available())      // slave may send less than requested
  {
    char c = Wire.read();       // receive a byte as character
    Serial.print(c);            // print the character
  }

  delay(500);
}
