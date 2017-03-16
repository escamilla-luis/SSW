#include <Wire.h>

void setup() 
{
  Wire.begin(8);
  Serial.begin(9600);
}

boolean object = false;
int distance = Wire.read();

void loop() 
{
Wire.requestFrom(8, 1);
while (Wire.available())
{
  int distance = Wire.read();
  Serial.print("Distance is ");
  Serial.print(distance);
  Serial.print(": ");
  
  if (distance <= 15)
{
  object != object;
  Serial.print("Alert! Object in front!" "\n");
 }
 else 
 {
  object = object;
  Serial.print("Track is Clear!" "\n");
 }
 delay(500);
}

}

