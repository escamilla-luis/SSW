#include <Wire.h>      // This allows communication between Arduinos


// Pins A4 and A5 are connected from the Nano to the Uno respectively
#define ECHO                7      // Echo Pin
#define TRIG                8      // Trigger Pin
#define RED                 6      // STOP and error status
#define GREEN               3      // GO status 
#define BLUE                5      // Status of podcar
#define BUTTON              2      // Kill Switch
#define OBSTRUCTION         9      // Stop motors based on ultrasonic
#define ERR_BUTTON         10      // Stop motors based on button


// For button state
#define NOT_PRESSED          LOW
#define IS_PRESSED           HIGH


// For ultrasonic
#define OFF                  LOW
#define ON                   HIGH


// RGB LED: green for GO status, red for STOP, blue for hall effect sensor status, blinking red for error
// Blinking blue for station arrival  

int maximumRange = 200;  // Maximum range needed
int minimumRange = 0;    // Minimum range needed
long duration;           // Duration used to calculate distance
int distance;            // Stores the distance


// Latching button using software
boolean switch_state = true;


// Array that stores receiving values from Uno
int hall_station_state[3];   // status update of hall effect sensor and stationID


// Stores the state of the button
int buttonState;


void setup() 
{
 Wire.begin(8);     // join the i2c bus with address #8 

 // Outputs 
 pinMode(TRIG, OUTPUT);
 pinMode(RED, OUTPUT);
 pinMode(GREEN, OUTPUT);
 pinMode(BLUE, OUTPUT);
 pinMode(ERR_BUTTON, OUTPUT);
 pinMode(OBSTRUCTION, OUTPUT);


 // Inputs
 pinMode(ECHO, INPUT);
 pinMode(BUTTON,INPUT_PULLUP);


 // change this to 57600 for XBEE Comm
 Serial.begin (9600);
}

void loop()
{
 // Continuously check the status of the button
 buttonState = digitalRead(BUTTON); 
 delay(50);


 // Receives values from the Uno
 Wire.onReceive(hall_effect_state);
 delay(50);


/* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(TRIG, OFF); 
 delayMicroseconds(2); 

 digitalWrite(TRIG, ON);
 delayMicroseconds(10); 
 
 digitalWrite(TRIG, OFF);
 
 // returns the length of the pulse in microseconds
 duration = pulseIn(ECHO, ON);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;

  
 // changed distance from 30 cm to 15 cm due to pole obstacle
 // LED is Red when statement is true
 if (distance <= 15) 
 {
  analogWrite(RED, 255);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);

  // Turns off the motors, podcar stops
  digitalWrite(OBSTRUCTION, ON);
 }
 
 // LED is blinking red if button is pressed and podcar stops
 else if (buttonState < 1)
 {
  // Creates a latching button
  switch_state = !switch_state;

  // Stops the motors, podcar stops
  digitalWrite(ERR_BUTTON, IS_PRESSED);
  delay(250);

  // Blinks the LED red
  do
  {   
   analogWrite(RED, 255);
   analogWrite(GREEN, 0);
   analogWrite(BLUE, 0);
   delay(250);
  
   analogWrite(RED, 0);
   analogWrite(GREEN, 0);
   analogWrite(BLUE, 0);
   delay(250);  
  }
  // Continues to blink while latching button is in effect
  // Only way to stop this cycle is to reset Arduino or add another button
  while(switch_state == false);
 }
 
 // LED is blue if any hall effect sensor reads a magnet
 else if(hall_station_state[1] == 0 || hall_station_state[2] == 0)
 {
  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 255);
 }

 // LED should be green and podcar still running
 else
 {
  analogWrite(RED, 0);
  analogWrite(GREEN, 255);
  analogWrite(BLUE, 0);

  // Makes sure that the button and ultrasonic do not affect the motion
  // of the podcar
  digitalWrite(ERR_BUTTON, NOT_PRESSED);
  digitalWrite(OBSTRUCTION, OFF);
 }
 
 // Delay 50ms before next reading.
 delay(50);
}

// Will check how many values are being received
void hall_effect_state(int howMany)
{
  while(Wire.available())
  {
    for(int i=1; i<=3; i++)
    {
     // Receives values in order
     // hall_station_state = {hall sensor 1, hall sensor 2, RFID}
     hall_station_state[i] = Wire.read(); 
    }
    // Optional if one wishes to see the values and for debugging purposes
    Serial.println(hall_station_state[1]);
    Serial.println(hall_station_state[2]);
    Serial.println(hall_station_state[3]);
  }
}

