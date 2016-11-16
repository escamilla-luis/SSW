#include <Wire.h>

#define echoPin 7      // Echo Pin
#define trigPin 8      // Trigger Pin
#define RED     6      // STOP and error status
#define GREEN   5      // GO status 
#define BLUE    3      // status of podcar
#define COMMON_ANODE // Type of RGB LED
#define BUTTON  2      // Kill Switch

#define IS_PRESSED  LOW
#define OFF         LOW
#define ON          HIGH

// RGB LED: green for GO status, red for STOP, blue for status, blinking red for error  

int maximumRange = 200;  // Maximum range needed
int minimumRange = 0;    // Minimum range needed
long duration, distance; // Duration used to calculate distance
int stop_counter = 0;
int restar_counter = 0;
String state = "";

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

 Serial.print("Duration (ms)\t  Distance \t Error Message");
 Serial.println();
}

void loop()
{
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
 
 Wire.onRequest(requestDistance);       // distance requested by master device
 delay(100);

 Wire.onRequest(requestStatus);
 delay(100);
 
 Serial.print(duration);
 Serial.print("\t\t\t");
 Serial.print(distance); 
 Serial.println();
 
 // changed distance from 30 cm to 15 cm due to pole obstacle
 if (distance <= 15) 
 {
  setColor(255, 0, 0);
  delay(100);
  setColor(255, 0, 0);
  delay(100);
  setColor(255, 0, 0);
  delay(100);
  
  stop_counter++;
 }
 else if (digitalRead(BUTTON) == IS_PRESSED)
 {
  setColor(255, 0, 0);
  
  state = "Emergency Stop";
 }
 else
 {
  stop_counter = 0;
  setColor(0, 255, 0);
 }
   //Serial.println(stop_counter);
   if (stop_counter == 10)
   {
    restar_counter = 0;
    
    state = "Warning: Out of range!";
    Serial.print("\t\t\t\t");
    Serial.println();
    
    stop_counter = 0;
    delay(1000);
    
    }
 //Delay 50ms before next reading.
 delay(50);
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);  
}

void requestDistance()
{
  Wire.write(distance);   // distance is sent to master device for further evaluation
}

void requestStatus()
{
  
}

