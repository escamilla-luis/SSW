<<<<<<< HEAD
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
                     ///// //////  //////  //////  //////  //////  //   //
                     //    //  //  // ///  //  //    //    //  //  ///  //
                     ///// //////  //////  //////    //    //////  // / //
                        // //      //  //  //   /    //    //  //  //  ///
                     ///// //      //  //  //   /    //    //  //  //   //

               ///// //  //  //////  ////// //////  //   //   // //////  //  //
               //    //  //  //  //  //     //  //  //  ///  //  //  //  //  //
               ///// //  //  //////  ////   //////  // /  / //   //////  //////
                  // //  //  //      //     //   /  ///   ///    //  //    //
               ///// //////  //      ////// //   /  //    //     //  //    //

                               //////  //////  //   //////
                                   //  //  //  //       //
                               //////  //  //  //       //
                               //      //  //  //       //
                               //////  //////  //       //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////// LIBRARIES ///////////////////////////////////////////////////////////

// Libraries for the RFID, pathing algorithm, and for the motors and servo
=======
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
#include <RFID.h>

// Libraries for the RFID and for the motors
//#include <AddicoreRFID.h>
#include <SPI.h>
#include <TimerThree.h>
<<<<<<< HEAD
#include <StackArray.h>
#include <Servo.h>



//////////////////////////////////// PIN DEFINITIONS ////////////////////////////////////////////////////////
/* Note: For Xbee Pro, Channel CH = C, Pan ID = 2017
 *  
 */
=======
#include <SoftwareSerial.h>


// Pin definitions
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
// Pins 0 and 1 on the Mega are used for Xbee Comms
/* Note: For RFID reader - SDA (PIN 9), SCK (PIN 52), MOSI (PIN 51),
 *  MISO (PIN 50), RST (PIN 8).
 */
<<<<<<< HEAD
#define motor1          2   // Interrupt 0
#define motor2          3   // Interrupt 1
#define SERVO           4   // Servo pin
=======
#define motor1          2
#define motor2          3
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
#define RED             10  // STOP and Error status
#define GREEN           11  // GO status
#define BLUE            12  // Status of the podcar
#define ECHO            22  // Echo pin
#define TRIG            23  // Trig pin
#define KILL_SWITCH     24  // Kill switch
#define RESET           25  // Resets the system after error status has been acknowledged
#define HALL_SENSOR1    26  // Switch servo motor using hall effect sensor mounted on left of podcar
#define HALL_SENSOR2    27  // Switch servo motor using hall effect sensor mounted on right of podcar



////////////////////////////////// SYSTEM VARIABLES /////////////////////////////////////////////////////////
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


// Stores the station number and checkpoint number
int  StationId = 0;
int  checkpoint = 0;
long SSN = 0;

// Create Array to be stored as message
int message[1];



// Variables used for pathing
#define SEND_PATH1      6
#define SEND_PATH2      7       // buttons and are temporary

StackArray <int> scheduleArray;

// 12 different paths a podcar can take from station to station
int path1to2[]={0, 0}; //0 for a 
int path1to3[]={1, 0}; //1 for a
int path1to4[]={1, 0}; //2 for a

int path2to1[]={0}; //3 for a
int path2to3[]={1, 1, 1, 0}; //4 for a
int path2to4[]={1, 1, 0}; //5 for a

int path3to1[]={1, 1, 0}; //6 for a
int path3to2[]={1, 1, 1, 0, 0}; //7 for a
int path3to4[]={0}; //8 for a

int path4to1[]={1, 0}; //9 for a
int path4to2[]={1, 1, 0, 0}; //10 for a
int path4to3[]={0, 0}; //11 for a

int a, b;



// Used to identify which podcar it is
int vehicle_number = 1;

SoftwareSerial XBee(0,1); //RX, TX

int commands[8];


// Initialize the servo
Servo leverArm;



///////////////////////////////////////// SETUP /////////////////////////////////////////////////////////////
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

  // Temporary
  pinMode(SEND_PATH1, INPUT_PULLUP);
  pinMode(SEND_PATH2, INPUT_PULLUP);


  // Set TimerThree to signal at 20Hz frequency (20 times/sec)
  Timer3.initialize(1000000 / 1000);

  
  // Change this to 57600 for Xbee Comm
  XBee.begin(57600);
  Serial.begin(57600);


  // Start timer3 PWM for motors
  Timer3.pwm(motor1, 0);
  //Timer3.pwm(motor2, 0);


  /* Initialize the RFID reader */
  SPI.begin();


  /* Initialize the RFID reader */
  RC522.init();
}


<<<<<<< HEAD
  // Setup the servo
  leverArm.attach(SERVO);

  leverArm.write(90);
}
=======
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6

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


//////////////////////////////////////// MAIN CODE //////////////////////////////////////////////////////////
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

<<<<<<< HEAD
  // Retrieves the station that the podcar is at if a tag is read
  getStationandCheckpointNumber();
=======
  switch (actionId) {
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6

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

<<<<<<< HEAD

  // Retrieve the speed of the motors
  getSpeedOfMotors();


  Signal2Path2Stack(); // Nested Functions that takes thesignal, determines the path, 
                       // then generates the stack

  IfSmartMagnetDetected();
  
  // Lever arm should switch with right arm up regardless of path destination
  IfDumbMagnetDetected();
=======
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
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
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

///////////////////////////////////// GET STATION AND CHECKPOINT NUMBER /////////////////////////////////////
// Retrieves the Station ID
<<<<<<< HEAD
int getStationandCheckpointNumber()
=======
// [ActionID = 2, StationNumber last visited between 1-4]
int getStationNumber()
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
{
  //StationId = 0;
  
    /* If so then get its serial number */
    RC522.readCardSerial();
    SSN = RC522.serNum[0];

<<<<<<< HEAD

    /* Get the Serial number of the tag */


    for (int i = 0; i < 1; i++)
    {
      SSN = RC522.serNum[i];
    }

    // For round tags, SSN = 85
    if (SSN == 35)
=======
    if (SSN == 85)
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
    {
      StationId = 1;
    }
<<<<<<< HEAD
    // SSN = 101
    if (SSN == 110)
=======

    if (SSN == 101)
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
    {
      StationId = 2;
    }
<<<<<<< HEAD
    // SSN = 149
    if (SSN == 103)
=======

    if (SSN == 149)
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
    {
      StationId = 3;
    }
<<<<<<< HEAD
    // SSN = 21
    if (SSN == 132)
=======

    if (SSN == 21)
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
    {
      StationId = 4;
    }
<<<<<<< HEAD

    // Next four statements are for checkpoint cards
    if (SSN == 107)
    {
      checkpoint = 1;
    }

    else if (SSN == 23)
    {
      checkpoint = 2;
    }

    else if (SSN == 176)
    {
      checkpoint = 3;
    }

    else if (SSN == 213)
    {
      checkpoint = 4;
    }

    else 
    {
      checkpoint = 0;
    }
  }
//  Serial.println(SSN);
  delay(50);
}



////////////////////////////////// SET NEXT STATION NUMBER //////////////////////////////////////////////////
// Sets the next station number
void setNextStationNumber(int station_number)
{
  
}



////////////////////////////////// KILL STATE ///////////////////////////////////////////////////////////////
// Checks the status of the button
void killState()
{
  // Continuously check the status of the kill button
  kill_state  = digitalRead(KILL_SWITCH);
}



//////////////////////////////////// MAGNET READ ////////////////////////////////////////////////////////////
// Retrieves the readings from the hall effect sensors
void magnetRead()
{
  // Checks if any magnet is read on the left of podcar
  hall_1_state = digitalRead(HALL_SENSOR1);


  // Checks if any magnet is read on the right of podcar
  hall_2_state = digitalRead(HALL_SENSOR2);

//  Serial.print("Left Hall Effect Sensor Reading: ");
//  Serial.print(hall_1_state);
//  Serial.println();
//  Serial.println();
//  Serial.print("Right Hall Effect Sensor Reading: ");
//  Serial.print(hall_2_state);
//  Serial.println();
//  Serial.println();
}



///////////////////////////////////////// ULTRASONIC ////////////////////////////////////////////////////////
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

//   Serial.print("Anti-collision distance: ");
//   Serial.print(distance);
//   Serial.println();
//   Serial.println();
}



//////////////////////////////////// IS SOMETHING IN FRONT //////////////////////////////////////////////////
// Detects if anything is in front of the podcar
boolean isSomethingInFront()
{
  boolean collision_detection;
=======

    // Shows the Station Number it is at
    Serial.println("The Station Id is:  ");
    Serial.println(StationId);

    RC522.isCard();
    
    return StationId;
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
  
  delay(50);
}

<<<<<<< HEAD



/////////////////////////////////////// SET STATE LED ///////////////////////////////////////////////////////
=======
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
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

<<<<<<< HEAD


///////////////////////////////////// GET STATE LED /////////////////////////////////////////////////////////
=======
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
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



<<<<<<< HEAD
//////////////////////////////////////// CONTROLS ///////////////////////////////////////////////////////////
=======
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

>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
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

   // Checks to see if any of the checkpoints have been read
   else if (checkpoint == 1 || checkpoint == 2 || checkpoint == 3 || checkpoint == 4)
   {
    setStateLED(2);
    setSpeedOfMotors(580);
//    Serial.print("Podcar passed checkpoint: ");
//    Serial.print(checkpoint);
//    Serial.println();
//    Serial.println();
//    delay(500);
    
    checkpoint = 0;
   }

   // Checks to see if podcar is at a station
   else if  (RC522.isCard())
   {
    StationId = getStationNumber();
    setStateLED(1); 
    setSpeedOfMotors(0);
<<<<<<< HEAD
//    Serial.print("Podcar is at station: ");
//    Serial.print(StationId);
//    Serial.println();
//    Serial.println();
=======
    //StationId = 0;
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
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
<<<<<<< HEAD
    setSpeedOfMotors(580);

=======
    setSpeedOfMotors(400);
>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
   }
   
   // delay 50 ms before next reading
   delay(50);  
}



<<<<<<< HEAD
///////////////////////////////////// SET SPEED OF MOTORS ///////////////////////////////////////////////////
// Function that sets the speed of the motors
// A pwm signal of 580 corresponds to a speed of ~0.82 ft/s (~0.25 m/s)
void setSpeedOfMotors(double motor_speed)
{
  Timer3.pwm(motor1, motor_speed);
 // Timer3.pwm(motor2, motor_speed);
}



//////////////////////////////////// GET SPEED OF MOTORS ////////////////////////////////////////////////////
//// Retrieves the speed of the motor based on the encoders
double getSpeedOfMotors()
{
}



///////////////////////////////////// SEND XBEE ////////////////////////////////////////////////////////////
// Function to send all requested information through Xbee??
void sendXbee()
{
  
}



///////////////////////////////// SIGNAL TO PATH TO STACK ///////////////////////////////////////////////////
void Signal2Path2Stack() //button1 and button2 represent different signals sent
{
  int button1 = digitalRead(SEND_PATH1);
  int button2 = digitalRead(SEND_PATH2);
  
  if(button1==0 & scheduleArray.isEmpty()) //If a path is sent and the array is empty 
  {
  a=4;
  WhichPath(a); 
  delay(500);
  GenerateStack(b);
  }

  if(button2==0 & scheduleArray.isEmpty()) //If a path is sent and the array is empty 
  {
  a=7;
  WhichPath(a); 
  delay(500);
  GenerateStack(b);
  }
}  



//////////////////////////////////////// WHICH PATH /////////////////////////////////////////////////////////
int WhichPath(int a)
{
  switch(a)
    {  
    case 0: 
      Serial.print("The current path is 1 to 2\n");
      a=0;
    break;
    case 1:
      Serial.print("The current path is 1 to 3\n");
      a=1;
    break;
    case 2:
      Serial.print("The current path is 1 to 4\n");
      a=2;
    break;    
    case 3:
      Serial.print("The current path is 2 to 1\n");
      a=3;
    break;    
    case 4:
      Serial.print("The current path is 2 to 3\n");
      a=4;
    break;    
    case 5:
      Serial.print("The current path is 2 to 4\n");
      a=5;
    break;
    case 6:
      Serial.print("The current path is 3 to 1\n");
      a=6;
    break;
    case 7:
      Serial.print("The current path is 3 to 2\n");
      a=7;
    break;
    case 8:
      Serial.print("The current path is 3 to 4\n");
      a=8;
    break;
    case 9:
      Serial.print("The current path is 4 to 1\n");
      a=9;
    break;
    case 10:
      Serial.print("The current path is 4 to 2\n");
      a=10;
    break;
    case 11:
      Serial.print("The current path is 4 to 3\n");
      a=11;
    break;
    }
b=a;
return b;
}



//////////////////////////////////// GENERATE STACK /////////////////////////////////////////////////////////
void GenerateStack(int b)
{
  Serial.println("The Stack Array is:");
  switch(b)
    {
     case 0:
       for(int i = (sizeof(path1to2)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path1to2[i]);
          Serial.println(scheduleArray.peek());
        }
    break;
    case 1:
        for(int i = (sizeof(path1to3)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path1to3[i]);
          Serial.println(scheduleArray.peek());
        }
     break;
     case 2:
        for(int i = (sizeof(path1to4)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path1to4[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 3:
        for(int i = (sizeof(path2to1)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path2to1[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 4:
        for(int i = (sizeof(path2to3)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path2to3[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 5:
        for(int i = (sizeof(path2to4)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path2to4[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 6:
        for(int i = (sizeof(path3to1)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path3to1[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 7:
        for(int i = (sizeof(path3to2)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path3to2[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 8:
        for(int i = (sizeof(path3to4)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path3to4[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 9:
        for(int i = (sizeof(path4to1)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path4to1[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 10:
        for(int i = (sizeof(path4to2)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path4to2[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 11:
        for(int i = (sizeof(path4to3)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path4to3[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      }
Serial.println("---------");
}



///////////////////////////////// IF SMART MAGNET DETECTED //////////////////////////////////////////////////
void IfSmartMagnetDetected()
{
    if (hall_1_state==0 & !scheduleArray.isEmpty()) //If smart sensor detects a magnet and stack is not empty
    {
    delay(500);
    
    int laststackvalue = scheduleArray.pop(); // Pop and assign last element of stack to laststackvalue
    Serial.println(laststackvalue);
 
    switch(laststackvalue) //Based on the value of the last element, switch or not don't switch
      {
      case 1: 
      leverArm.write(20);
      Serial.println("Switched");
      break;

      case 0:
      leverArm.write(130);
      Serial.println("Not Switched");
      break;
      }
    
    }
}



///////////////////////////////// IF DUMB MAGNET DETECTED ////////////////////////////////////////////////////
// Switches the lever arm if a dumb magnet is detected
void IfDumbMagnetDetected()
{
  if (hall_2_state == 0) //If dumb sensor detects a magnet
  {
    leverArm.write(130);
    Serial.print("Dumb\n");
    delay(500);
  }
}
=======


>>>>>>> c5ae3c8c223b8fc649166ae20185b0c4298c09c6
