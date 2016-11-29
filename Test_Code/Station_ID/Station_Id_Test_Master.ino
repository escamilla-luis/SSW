#include <Wire.h>

void setup(){
  Wire.begin(8);
  Serial.begin(9600);
  
}

void loop(){
Wire.requestFrom(8, 1);
while (Wire.available())
{
  int StationId = Wire.read();
  Serial.print("Station Id: ");
  Serial.print(StationId);
  Serial.print("\n");
  Wire.endTransmission();
}
delay(1000);
}


