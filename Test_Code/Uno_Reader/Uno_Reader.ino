#include <Wire.h>

int distance;
int LEDstate;
int event[2];
void setup() 
{
  Wire.begin(8);        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop()
{
  Wire.onReceive(receiveEvent);
  delay(50);
}

void receiveEvent(int howMany)
{
  while(Wire.available())
  {
    for(int i=1; i<=2; i++)
    {
      event[i] = Wire.read();
    }
    Serial.println(event[1]);
    Serial.println(event[2]);
  }
}

