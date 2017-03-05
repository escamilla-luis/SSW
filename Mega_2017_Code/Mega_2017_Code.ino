// Libraries for the RFID and for the motors
#include <RFID.h>
#include <SPI.h>
#include <TimerThree.h>


/* Note: For Xbee Pro, Channel CH = C, Pan ID = 2017
 *  
 */


// Pin definitions
// Pins 0 and 1 on the Mega are used for Xbee Comms
/* Note: For RFID reader - SDA (PIN 9), SCK (PIN 52), MOSI (PIN 51),
 *  MISO (PIN 50), RST (PIN 8).
 */
#define motor1          2   // Interrupt 0
#define motor2          3   // Interrupt 1
#define RED             10  // STOP and Error status
#define GREEN           11  // GO status
#define BLUE            12  // Status of the podcar
#define motor1_enco_a   18  // Interrupt 5
#define motor1_enco_b   19  // Interrupt 4
#define motor2_enco_a   20  // Interrupt 3 
#define motor2_enco_b   21  // Interrupt 2
#define ECHO            22  // Echo pin
#define TRIG            23  // Trig pin
#define KILL_SWITCH     24  // Kill switch
#define RESET           25  // Resets the system after error status has been acknowledged
#define HALL_SENSOR1    26  // Switch servo motor using hall effect sensor mounted on left of podcar
#define HALL_SENSOR2    27  // Switch servo motor using hall effect sensor mounted on right of podcar


// For button state
#define NOT_PRESSED     LOW
#define IS_PRESSED      HIGH


// For ultrasonic and LED
#define OFF             LOW
#define ON              HIGH


/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO         9
#define RESET_DIO       8


/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO); 


// Latching button using software
boolean kill_latch_state = true;
boolean reset_latch_state = true;


// Stores the state of each button
int kill_state;
int reset_state;


// Used for the ultrasonic sensor
int maximumRange = 200;       // Maximum range needed
int minimumRange = 0;         // Minimum range needed
long duration;                // Duration used to calculate distance
int distance;                 // Stores the distance


// Used for motor encoders
// Encoders provide 12 counts per revolution of the motor shaft when 
// counting both edges of both channels
// To compute the counts/rev of gearbox output shaft, multiply gear ratio by 12
volatile unsigned int enc_count_1 = 0;
volatile unsigned int enc_count_2 = 0;
float vel1, vel2;



// Variables used to store the readings from the hall effect sensors
int hall_1_state;
int hall_2_state;


/* Note: RGB LED- Green (GO status), RED (STOP), BLUE (Hall effect sensor reading)
 *  Blinking BLUE (Station arrival), Blinkin RED (Error)
 */


// Used for LED purposes
int redState = OFF;       // Initially, all LED states should be off
int greenState = OFF;
int blueState = OFF;


unsigned long previousRed = 0;
unsigned long previousBlue = 0;
const long interval = 250;


// Stores the station number
int  StationId = 0;
long SSN = 0;


// Used to identify which podcar it is
int vehicle_number = 1;


 
void setup()
{
  // Outputs
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  

  // Inputs
  pinMode(motor1_enco_a, INPUT);
  pinMode(motor1_enco_b, INPUT);
  pinMode(motor2_enco_a, INPUT);
  pinMode(motor2_enco_b, INPUT);
  pinMode(ECHO, INPUT);
  pinMode(KILL_SWITCH, INPUT_PULLUP);
  pinMode(RESET, INPUT_PULLUP);
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);


  digitalWrite(motor1_enco_a, HIGH);       // Turn on pull-up resistors
  digitalWrite(motor1_enco_b, HIGH);       
  digitalWrite(motor2_enco_a, HIGH);     
  digitalWrite(motor2_enco_b, HIGH);     


  // Encoder pins


  // Set TimerThree to signal at 20Hz frequency (20 times/sec)
  Timer3.initialize(1000000 / 1000);

  
  // Change this to 57600 for Xbee Comm
  Serial.begin(9600);


  // Start timer3 PWM for motors
  Timer3.pwm(motor1, 0);
  Timer3.pwm(motor2, 0);


  /* Initialize the RFID reader */
  SPI.begin();


  /* Initialize the RFID reader */
  RC522.init();


  // Set initial encoder count variable
}



void loop()
{ 
  // Checks on the state of the kill button
  killState();


  // Function that will check to see if any of the hall effect sensors read a magnet
  magnetRead();


  // Retrieves the station that the podcar is at if a tag is read
  getStationNumber();


  // Retrieves the distance from the ultrasonic sensor and returns the state of the collision
  // detection system
  ultrasonic();
  isSomethingInFront();


  // Retrieves the state of the RGB LED
  getStateLED();
  

  // System controls
  controls();

  getSpeedOfMotors();
}



// Retrieves the Station ID
int getStationNumber()
{
  /* Has a card been detected? */
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();


    /* Get the Serial number of the tag */


    for (int i = 0; i < 1; i++)
    {
      SSN = RC522.serNum[i];
    }

    // For square tags, SSN = 35
    if (SSN == 85)
    {
      StationId = 1;
      return StationId;
    }
    // SSN = 110
    if (SSN == 101)
    {
      StationId = 2;
      return StationId;
    }
    // SSN = 103
    if (SSN == 149)
    {
      StationId = 3;
      return StationId;
    }
    // SSN = 132
    if (SSN == 21)
    {
      StationId = 4;
      return StationId;
    }
  }
//  Serial.println(SSN);
  delay(50);
}



// Sets the next station number
void setNextStationNumber(int station_number)
{
  
}



// Checks the status of the button
void killState()
{
  // Continuously check the status of the kill button
  kill_state  = digitalRead(KILL_SWITCH);
}



// Retrieves the readings from the hall effect sensors
void magnetRead()
{
  // Checks if any magnet is read on the left of podcar
  hall_1_state = digitalRead(HALL_SENSOR1);


  // Checks if any magnet is read on the right of podcar
  hall_2_state = digitalRead(HALL_SENSOR2);

  Serial.print("Left Hall Effect Sensor Reading: ");
  Serial.print(hall_1_state);
  Serial.println();
  Serial.println();
  Serial.print("Right Hall Effect Sensor Reading: ");
  Serial.print(hall_2_state);
  Serial.println();
  Serial.println();
}



// Function that will return the distance from the ultrasonic sensor
void ultrasonic()
{
  /* The following trigPin/echoPin cycle is used to determine the distance of 
   *  the nearest object by bouncing soundwaves off of it 
   */

   digitalWrite(TRIG, OFF);
   delayMicroseconds(2);

   digitalWrite(TRIG, ON);
   delayMicroseconds(10);

   digitalWrite(TRIG, OFF);

   // Returns the length of the pulse in microseconds
   duration = pulseIn(ECHO, ON);

   // Calculate the distance (in cm) based on the speed of sound
   distance = duration/58.2;

   Serial.print("Anti-collision distance: ");
   Serial.print(distance);
   Serial.println();
   Serial.println();
}



// Detects if anything is in front of the podcar
boolean isSomethingInFront()
{
  boolean collision_detection;
  
  if(distance <= 15)
  {
    collision_detection = true;
  }

  else
  {
    collision_detection = false; 
  }

  return collision_detection;
}




// Turns on the LED based on one of the specified states
void setStateLED(int colorState)
{
  // LED is red (Station arrival or collision prevention)
  if (colorState == 1)
  {
    digitalWrite(RED, ON);
    digitalWrite(GREEN, OFF);
    digitalWrite(BLUE, OFF);
  }

  // LED is blue (checkpoint detected)
  if (colorState == 2)
  {
    digitalWrite(RED, OFF);
    digitalWrite(GREEN, OFF);
    digitalWrite(BLUE, ON);
  }

  // LED is green (podcar in motion)
  if (colorState == 3)
  {
    digitalWrite(RED, OFF);
    digitalWrite(GREEN, ON);
    digitalWrite(BLUE, OFF);
  }

  // LED is blinking red (error)
  if (colorState == 4)
  {
    unsigned long currentRedblink = millis();

    if (currentRedblink - previousRed >= interval)
    {
      previousRed = currentRedblink;

      if (redState == OFF)
      {
        redState = ON;
        greenState = OFF;
        blueState = OFF;
      }

      else
      {
        redState = OFF;
        greenState = OFF;
        blueState = OFF;
      }

      // Set the state of the LED
      digitalWrite(RED, redState);
      digitalWrite(GREEN, greenState);
      digitalWrite(BLUE, blueState);
    }
    
    // Checks to see if the reset button has been pressed
    reset_state = digitalRead(RESET);

    // Resets the LED to green and returns the states of each button to default values
    if(reset_state < 1)
    {
      reset_latch_state = false;
      kill_latch_state = true;
    }
    else if(kill_state < 1)
    {
      kill_latch_state = false;
      reset_latch_state = true;
    }
  }
}



// Gets the state of the LED
// Used for mobile app purposes
int getStateLED()
{
  if(digitalRead(RED) == 1)            // Red LED is on
  {
    return 1;                          // 1 status is for podcar has stopped
  }

  else if(digitalRead(GREEN) == 1)     // Green LED is on
  {
    return 3;                          // 2 status is for podcar in motion
  }

  else if(digitalRead(BLUE) == 1)      // Blue LED is on
  {
    return 2;                          // 3 status if successful checkpoint reading
  }

  else 
  {
    return 4;                          // 4 status is for error (red flashing) 
  }
}



// Function that controls the functions of the system
void controls()
{
     // Changed distance from 30 cm to 15 cm due to pole obstacle
   // LED is Red when statement is true
   if (distance <= 15)
   {
    setStateLED(1);
    setSpeedOfMotors(0);
   }

   // LED is blinking red if button is pressed
   else if (kill_state < 1)
   {
    // Creates a latching button
    kill_latch_state = false;
    reset_latch_state = true;

    // Continues to blink the LED red while latching button is in effect
    // Only way to stop this cycle is to reset Arduino or add another button
    while(kill_latch_state == false && reset_latch_state == true)
    {
      setStateLED(4);
      setSpeedOfMotors(0);
    }
   }

   // Checks to see if the hall effect sensor on the left of podcar read a magnet
   else if (hall_1_state == 0 || hall_2_state == 0)
   {
    setStateLED(2);
    setSpeedOfMotors(580);
   } 

   // Checks to see if podcar is at a station
   else if (StationId == 1 || StationId == 2 || StationId == 3 || StationId == 4)
   {
    setStateLED(1); 
    setSpeedOfMotors(0);
    Serial.print("Podcar is at station: ");
    Serial.print(StationId);
    Serial.println();
    Serial.println();
   }

   // LED should be green
   else
   {
    setStateLED(3);
    setSpeedOfMotors(580);
   }
}



// Function that sets the speed of the motors
// A pwm signal of 580 corresponds to a speed of ~0.82 ft/s (~0.25 m/s)
void setSpeedOfMotors(double motor_speed)
{
  Timer3.pwm(motor1, motor_speed);
  Timer3.pwm(motor2, motor_speed);
}



// Retrieves the speed of the motor based on the encoders
double getSpeedOfMotors()
{
}



// Function to send all requested information through Xbee??
void sendXbee()
{
  
}
