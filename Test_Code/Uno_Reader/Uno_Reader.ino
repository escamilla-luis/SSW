#include <Wire.h>

#define hall_sensor1  2
#define hall_sensor2  4

int distance;
int LEDstate;
int hall_1_state;
int hall_2_state;
int hall_state[]= {hall_1_state, hall_2_state};
int event[2];

void setup() 
{
  pinMode(hall_sensor1, INPUT);
  pinMode(hall_sensor2, INPUT);
  
  Wire.begin(8);        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop()
{
  Wire.onReceive(receiveEvent);
  delay(50);
  
  hall_1_state = digitalRead(hall_sensor1);
  hall_state[1] = hall_1_state;
  delay(50);

  hall_2_state = digitalRead(hall_sensor2);
  hall_state[2] = hall_2_state;
  delay(50);
  
  Wire.beginTransmission(8);
  Wire.write(hall_state[1]);
  Wire.endTransmission();
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

