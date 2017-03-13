#include <RFID.h>

// Libraries for the RFID and for the motors
//#include <AddicoreRFID.h>
#include <SPI.h>
#include <TimerThree.h>
#include <SoftwareSerial.h>


// Pin definitions
// Pins 0 and 1 on the Mega are used for Xbee Comms
/* Note: For RFID reader - SDA (PIN 9), SCK (PIN 52), MOSI (PIN 51),
 *  MISO (PIN 50), RST (PIN 8).
 */
#define motor1          2
#define motor2          3
#define RED             10  // STOP and Error status
#define GREEN           11  // GO status
#define BLUE            12  // Status of the podcar
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

// Create Array to be stored as message
int message[1];


// Used to identify which podcar it is
int vehicle_number = 1;

SoftwareSerial XBee(0,1); //RX, TX

int commands[8];

 
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
  pinMode(ECHO, INPUT);
  pinMode(KILL_SWITCH, INPUT_PULLUP);
  pinMode(RESET, INPUT_PULLUP);
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);


  // Set TimerThree to signal at 20Hz frequency (20 times/sec)
  Timer3.initialize(1000000 / 1000);

  
  // Change this to 57600 for Xbee Comm
  XBee.begin(57600);
  Serial.begin(57600);


  // Start timer3 PWM for motors
  Timer3.pwm(motor1, 0);
  Timer3.pwm(motor2, 0);


  /* Initialize the RFID reader */
  SPI.begin();


  /* Initialize the RFID reader */
  RC522.init();
}



String readString = "";

 byte checksum = 0x00;
  bool string_enable = false;
  String string_complete = "";
  int result = 0;

  
/**
  * Xbee communication will have a certain protocol in the form of an int array.
  * The int array will signal the Arduino to perform an action. The protocol will be in the following form:
  * [ActionId, ....(actions for each signal)]
  * Above every function, there will be an explanation of what each array will do based on the given number input between 0-9 for each index.
*/


void loop()
{ 
  //Serial.write('a');
  //Check for XBee
  

  receiveXbeeMessage();

  if (commands[0] > 0){
    resolvePacketData();
  }

  
  // Checks on the state of the kill button
  //killState();


  // Function that will check to see if any of the hall effect sensors read a magnet
  //magnetRead();
  

  // System controls
  //controls();
}

void resolvePacketData() {

  // initialization of important variables
  int actionId = commands[0];
  int speedOfMotors;

  switch (actionId) {

    // setNextStation
    case 1: Serial.println("setstation");
    setNextStation(commands[1]);
    break;

    // getNextStation
    case 2: Serial.println("getstation");
    //sendxbee(commands[1], commands[2],...);
    break;

    // SetLED
    case 3: Serial.println("setLED");
    setStateLED(commands[1]);
    break;

    //getLED
    case 4: Serial.println("getLED");
    //sendxbee(commands[1], commands[2],...);
    break;

    //setSpeed
    case 5: Serial.println("setSpeed");
    speedOfMotors = commands[1] * 100;
    setSpeedOfMotors(speedOfMotors);
    break;

    //getSpeed
    case 6: Serial.println("getSpeed");
    //sendxbee(commands[1], commands[2],...);
    break;
  }

  commands[0] = 0;
  
}

void receiveXbeeMessage() {

  char in;
  int additive;
  int counterForCommands = 0;

  // Next, fill the array with the list of commands.
  while(Serial.available()) {
    delay(10);  //small delay to allow input buffer to fill

    in = (char)Serial.read();
    additive = (int)in - 48;
    
    commands[counterForCommands] = additive;
    counterForCommands++;
  }

//  if (commands[0] > 0) {
//    Serial.println(commands[0]);
//    Serial.println(commands[1]);
//    Serial.println(commands[2]);
//    Serial.println(commands[3]);
//    Serial.println(commands[4]);
//    Serial.println(commands[5]);
//    Serial.println(commands[6]);
//    Serial.println(commands[7]);
//    commands[0] = 0;
//  }
}

// Function to send all requested information through Xbee??
void sendXbee()
{
  
}

// Sets the next station number
// [ActionId = 1, lengthOfArray = 3, the next station number between 1-4]
void setNextStation(int station_number)
{
  Serial.println("in setStation");
}

// Retrieves the Station ID
// [ActionID = 2, StationNumber last visited between 1-4]
int getStationNumber()
{
  //StationId = 0;
  
    /* If so then get its serial number */
    RC522.readCardSerial();
    SSN = RC522.serNum[0];

    if (SSN == 85)
    {
      StationId = 1;
    }

    if (SSN == 101)
    {
      StationId = 2;
    }

    if (SSN == 149)
    {
      StationId = 3;
    }

    if (SSN == 21)
    {
      StationId = 4;
    }

    // Shows the Station Number it is at
    Serial.println("The Station Id is:  ");
    Serial.println(StationId);

    RC522.isCard();
    
    return StationId;
  
  delay(50);
}

// Turns on the LED based on one of the specified states
// [ActionId = 3, sets the state between 1-4 atm or 0 if not on]
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
    else if(killState())
    {
      kill_latch_state = false;
      reset_latch_state = true;
    }
  }
}

// Gets the state of the LED
// [ActionId = 4, the state between 1-4 atm or 0 if not on]
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

// Function that sets the speed of the motors
// [ActionId = 5, setCurrentSpeed]
void setSpeedOfMotors(int motor_speed)
{
  Timer3.pwm(motor1, motor_speed);
  Timer3.pwm(motor2, motor_speed);
}



// Retrieves the speed of the motor based on the encoders
// [ActionId = 6, currentSpeedOfMotor]
int getSpeedOfMotors()
{
  return 0;
}




// Checks the status of the button
bool killState()
{
  // Continuously check the status of the kill button
  kill_state  = digitalRead(KILL_SWITCH);

  if(kill_state < 1) {
    return true;
  }
  else {
    return false;
  }
}



// Retrieves the readings from the hall effect sensors
bool magnetRead()
{
  // Checks if any magnet is read on the left of podcar
  hall_1_state = digitalRead(HALL_SENSOR1);


  // Checks if any magnet is read on the right of podcar
  hall_2_state = digitalRead(HALL_SENSOR2);

  if (hall_1_state == 0 || hall_2_state == 0) {
    return true;
  }

  else {
    return false;
  }
}



// Function that will return the distance from the ultrasonic sensor
int ultrasonic()
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

   return distance;
}



// Detects if anything is in front of the podcar
boolean isSomethingInFront()
{
  boolean collision_detection;
  
  if(ultrasonic() <= 15)
  {
    collision_detection = true;
  }

  else
  {
    collision_detection = false; 
  }

  return collision_detection;
}

// Function that controls the functions of the system
void controls()
{

  // LED is blinking red if button is pressed
   if (killState())
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
  
     // Changed distance from 30 cm to 15 cm due to pole obstacle
   // LED is Red when statement is true
   else if (isSomethingInFront())
   {
    setStateLED(1);
    setSpeedOfMotors(0);
   }

   // Checks to see if podcar is at a station
   else if  (RC522.isCard())
   {
    StationId = getStationNumber();
    setStateLED(1); 
    setSpeedOfMotors(0);
    //StationId = 0;
   }

   // Checks to see if the hall effect sensor on the left of podcar read a magnet
   else if (magnetRead())
   {
    setStateLED(2);
    setSpeedOfMotors(800);
   } 

   // LED should be green 
   else
   {
    setStateLED(3);
    setSpeedOfMotors(400);
   }
   
   // delay 50 ms before next reading
   delay(50);  
}





