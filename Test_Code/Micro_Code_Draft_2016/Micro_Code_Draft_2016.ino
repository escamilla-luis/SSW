#include <Wire.h>

#define echoPin 7      // Echo Pin
#define trigPin 8      // Trigger Pin
#define RED     6      // STOP and error status
#define GREEN   3      // GO status 
#define BLUE    5      // status of podcar
#define BUTTON  2      // Kill Switch

#define OFF         LOW
#define ON          HIGH

// RGB LED: green for GO status, red for STOP, blue for status, blinking red for error  

int maximumRange = 200;  // Maximum range needed
int minimumRange = 0;    // Minimum range needed
long duration;           // Duration used to calculate distance

// Transmitting values to the Uno
int distance;
int LEDstate;
int event[]= {distance, LEDstate}; // stores LED state from 0 to 3

// Latching button using software
boolean switch_state = true;

// Receiving values from Uno
int hall_state[2];   // status update of hall effect sensor

void setup() 
{
 Wire.begin(8);     // join the i2c bus with address #8 
  
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 
 pinMode(RED, OUTPUT);
 pinMode(GREEN, OUTPUT);
 pinMode(BLUE, OUTPUT);
 
 pinMode(BUTTON,INPUT_PULLUP);

 // change this to 57600 for XBEE comm
 Serial.begin (9600);

 Serial.print("Duration (ms)\t  Distance");
 Serial.println();
}

void loop()
{
 int buttonState = digitalRead(BUTTON); 
 delay(50);

 Wire.onReceive(hall_effect_state);
 delay(50);

/* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin, OFF); 
 delayMicroseconds(2); 

 digitalWrite(trigPin, ON);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin, OFF);
 
 // returns the length of the pulse in microseconds
 duration = pulseIn(echoPin, ON);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;

 event[1]= distance;
 
 //Serial.print(duration);
 //Serial.print("\t\t\t");
 //Serial.print(distance); 
 //Serial.println();
  
 // changed distance from 30 cm to 15 cm due to pole obstacle
 if (distance <= 15) 
 {
  LEDstate =1;
  event[2] = LEDstate;
  analogWrite(RED, 255);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);
 }
 else if (buttonState < 1)
 {
  LEDstate =3;
  event[2] = LEDstate; 
  switch_state = !switch_state;
  delay(250);
  
  do
  {
   Wire.beginTransmission(8);
   Wire.write(event[1]);
   Wire.write(event[2]);
   Wire.endTransmission();
   delay(50);
   
   analogWrite(RED, 255);
   analogWrite(GREEN, 0);
   analogWrite(BLUE, 0);
   delay(250);
  
   analogWrite(RED, 0);
   analogWrite(GREEN, 0);
   analogWrite(BLUE, 0);
   delay(250);  
  }
  while(switch_state == false);
 }
 else if(hall_state[1] == 0) || hall_state[2] == 0)
 {
  LEDstate =2;
  event[2] = LEDstate;
  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 255);
 }
 else
 {
  LEDstate =0;
  event[2] = LEDstate;
  analogWrite(RED, 0);
  analogWrite(GREEN, 255);
  analogWrite(BLUE, 0);
 }
 // expect numbers like 0,1,2,3 for LED status
 // 0 = Green, 1 = Red, 2 = Blue, 3 = Blinking Red
 // This function call will also include distance
 // Delay 50ms before next reading.
 Wire.beginTransmission(8);
 Wire.write(event[1]);
 Wire.write(event[2]);
 
 Wire.endTransmission();
 // Serial.println(event[1]);
 // Serial.println(event[2]);
 delay(50);
}

void hall_effect_state(int howMany)
{
  while(Wire.available())
  {
    for(int i=1; i<=2; i++)
    {
     hall_state[i] = Wire.read(); 
    }
  }
}

