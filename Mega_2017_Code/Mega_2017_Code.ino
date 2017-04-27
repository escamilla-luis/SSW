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
#include <RFID.h>
#include <SPI.h>
#include <TimerThree.h>
#include <StackArray.h>
#include <Servo.h>
#include <SoftwareSerial.h>

//////////////////////////////////// PIN DEFINITIONS ////////////////////////////////////////////////////////
/* Note: For Xbee Pro, Channel CH = C, Pan ID = 2017
 *  
// */
//// Pins 0 and 1 on the Mega are used for Xbee Comms
///* Note: For RFID reader - SDA (PIN 9), SCK (PIN 52), MOSI (PIN 51),
// *  MISO (PIN 50), RST (PIN 8).
// */
//#define motor1          2   // Interrupt 0
//#define motor2          3   // Interrupt 1
//#define SERVO           5   // Servo pin
//#define RED             10  // STOP and Error status
//#define GREEN           11  // GO status
//#define BLUE            12  // Status of the podcar
//#define motor1_enco_a   18  // Interrupt 5
//#define motor1_enco_b   19  // Interrupt 4
//int     POS_SENSOR1 =   A0; // Post Sensor
//int     POS_SENSOR2 =   A1; // Post Sensor
//#define ECHO            22  // Echo pin
//#define TRIG            23  // Trig pin
//#define KILL_SWITCH     24  // Kill switch
//#define RESET           25  // Resets the system after error status has been acknowledged
//#define HALL_SENSOR1    26  // Switch servo motor using hall effect sensor mounted on left of podcar
//#define HALL_SENSOR2    27  // Switch servo motor using hall effect sensor mounted on right of podcar
//#define motor2_enco_a   28  // Interrupt 3 
//#define motor2_enco_b   29  // Interrupt 2

// Pins 0 and 1 on the Mega are used for Xbee Comms
/* Note: For RFID reader - SDA (PIN 9), SCK (PIN 52), MOSI (PIN 51),
 *  MISO (PIN 50), RST (PIN 8).
 */
#define motor1          2   // Interrupt 0
#define motor2          3   // Interrupt 1
#define SERVO           5   // Servo pin
#define RED             10  // STOP and Error status
#define GREEN           11  // GO status
#define BLUE            12  // Status of the podcar
#define KILL_SWITCH     18  // Interrupt 5
#define motor1_enco_b   19  // Interrupt 4
int     POS_SENSOR1 =   A4; // Post Sensor
int     POS_SENSOR2 =   A5; // Post Sensor
#define ECHO            22  // Echo pin
#define TRIG            23  // Trig pin
#define ECHO2           49  // Second Echo Pin
#define TRIG2           48  // Second Trig Pin
//#define KILL_SWITCH     24  // Kill switch
#define RESET           25  // Resets the system after error status has been acknowledged
#define HALL_SENSOR1    20  // Switch servo motor using hall effect sensor mounted on left of podcar
#define HALL_SENSOR2    21  // Switch servo motor using hall effect sensor mounted on right of podcar
#define motor2_enco_a   28  // Interrupt 3 
#define motor2_enco_b   29  // Interrupt 2

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
int reset_state;
int killStatePressedTwice;

// Additional values for the two switches
bool checkForSecondKillPress;
bool needToFlush;
volatile bool killStateNotRefreshed;


// Used for the ultrasonic sensor
int maximumRange = 200;       // Maximum range needed
int minimumRange = 0;         // Minimum range needed
long duration;                // Duration used to calculate distance
int distance;                 // Stores the distance
int countForUltrasonic;       // Count the time to trigger ultrasonic
bool stillInRangeUltrasonic;  // Checks if the sensor is still in range after 2 seconds


// Used for motor encoders
// Encoders provide 12 counts per revolution of the motor shaft when 
// counting both edges of both channels
// To compute the counts/rev of gearbox output shaft, multiply gear ratio by 12
volatile unsigned int enc_count_1 = 0;
volatile unsigned int enc_count_2 = 0;
float vel1, vel2;



// Variables used to store the readings from the hall effect sensors
volatile int hall_1_state = 1;
volatile int hall_2_state = 1;


/* Note: RGB LED- Green (GO status), RED (STOP), BLUE (Hall effect sensor reading)
 *  Blinking BLUE (Station arrival), Blinkin RED (Error)
 */

// Used for LED purposes
int redState = OFF;       // Initially, all LED states should be off
int greenState = OFF;
int blueState = OFF;
unsigned long currentBlink = 8;
unsigned long previousBlink = 4;
const long interval = 10;

// Stores the station number and checkpoint number
int  STATIONID = 0;
int  CHECKPOINT = 0;
long SSN = 0;
int howLongToStayAtStation;
bool stillAtStation;
bool serverGoFromStation;

// Station and Checkpoint Numbers
int STATIONONE;
int STATIONTWO;
int STATIONTHREE;
int STATIONFOUR;

int CHECKPOINTONE;
int CHECKPOINTTWO;
int CHECKPOINTTHREE;
int CHECKPOINTFOUR;

// To tell if a packet is sent or not
bool didSendStationPacket = true;

// The isCard result value
bool RFIDisCard;

// Check if the magnets continue to be true
bool isMagnetNotRead;

// Global set values
long SPEED;
int maximumSpeed;
int STATEOFCAR;
int STATEOFCARFROMSERVER;
volatile int SERVOINT;
int incrementByFifty;

// Positioning Variables
const int peakgap = 7;
int LastRFID;
int Peak_Num;
int Position;
int tickCounterStation = 0;
bool isFirstPosTickThrough;
bool isSecondPosTickThrough;

// Create Array to be stored as message
int message[1];

// Allows the XBee to communicate
SoftwareSerial xbee(0,1); //RX, TX

// The Packet int array that will be used for communication
int commands[8];

// Variables used for pathing
#define SEND_PATH1      6
#define SEND_PATH2      7       // buttons and are temporary

// The array for the 12 paths 
volatile StackArray <int> scheduleArray;

// 8 different paths a podcar can take from anywhere to a station
int exactlyAfterStationOneFour[] = {1,1,1,0};
int exactlyBeforeStationTwoThree[] = {0,0};

int afterStationOneFourToTwoThree[] = {1,1,0,0};
int passMiddleIntoStationOneFour[] = {1,0};

int fromMiddleIntoStationOneFour[] = {1,1,0};
int fullCircleIntoStationTwoThree[] = {1,1,1,0,0};

int halfCircleIntoStationTwoThree[] = {1,0,0};
int switchIntoClosestStation[] = {0};

// Before or After magnet value
volatile bool isBeforeMagnet;

// 12 different paths a podcar can take from station to station
int path1to2[]={0, 0}; //0 for a 
int path1to3[]={1, 1, 0, 0}; //1 for a
int path1to4[]={1, 0}; //2 for a

int path2to1[]={0}; //3 for a
int path2to3[]={1, 1, 1, 0, 0}; //4 for a
int path2to4[]={1, 1, 0}; //5 for a

int path3to1[]={1, 1, 0}; //6 for a
int path3to2[]={1, 1, 1, 0, 0}; //7 for a
int path3to4[]={0}; //8 for a

int path4to1[]={1, 0}; //9 for a
int path4to2[]={1, 1, 0, 0}; //10 for a
int path4to3[]={0, 0}; //11 for a

bool firstChoiceForInitialPath;
bool isNotAtLastStationInDestination;
int lastStationInDestination;

// Initialize the servo
Servo leverArm;

// Podcar number
int podNumber = 6;
bool testStack;

volatile bool checkingInter = false;

///////////////////////////////////////// SETUP /////////////////////////////////////////////////////////////
void setup()
{
  // Outputs
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  

  // Inputs
  //pinMode(motor1_enco_a, INPUT);
  pinMode(motor1_enco_b, INPUT);
  pinMode(motor2_enco_a, INPUT);
  pinMode(motor2_enco_b, INPUT);
  pinMode(ECHO, INPUT);
  pinMode(ECHO2, INPUT);
  pinMode(KILL_SWITCH, INPUT_PULLUP);
  pinMode(RESET, INPUT_PULLUP);
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);
  pinMode(POS_SENSOR1, INPUT);
  pinMode(POS_SENSOR2, INPUT);

  // Temporary
  pinMode(SEND_PATH1, INPUT_PULLUP);
  pinMode(SEND_PATH2, INPUT_PULLUP);

  // Turn on pull-up resistors
  //digitalWrite(motor1_enco_a, HIGH);       
  digitalWrite(motor1_enco_b, HIGH);       
  digitalWrite(motor2_enco_a, HIGH);     
  digitalWrite(motor2_enco_b, HIGH);     

  // Set TimerThree to signal at 20Hz frequency (20 times/sec)
  Timer3.initialize(1000000 / 1000);

  // Change this to 57600 for Xbee Comm
  Serial.begin(57600);
  xbee.begin(57600);


  // Start timer3 PWM for motors
  Timer3.pwm(motor1, 0);
  Timer3.pwm(motor2, 0);


  /* Initialize the RFID reader */
  SPI.begin();
  RC522.init();

  killStatePressedTwice = 0;
  checkForSecondKillPress = false;

  // initialize SPEED and set initial STATE
  SPEED = 600;
  maximumSpeed = 700;
  STATEOFCAR = 5;
  SERVOINT = 140;
  incrementByFifty = 50;
  howLongToStayAtStation = 0;
  stillAtStation = false;
  //serverGoFromStation = true; // change false when using the server
  serverGoFromStation = false; // change false when using the server

  // For the track
  STATIONONE = 35;
  STATIONTWO = 110;
  STATIONTHREE = 103;
  STATIONFOUR = 132;

  CHECKPOINTONE = 107;
  CHECKPOINTTWO = 23;
  CHECKPOINTTHREE = 176;
  CHECKPOINTFOUR = 213;

  // For testing
//  STATIONONE = 35;
//  STATIONTWO = 110;
//  STATIONTHREE = 103;
//  STATIONFOUR = 132;
//
//  CHECKPOINTONE = 107;
//  CHECKPOINTTWO = 23;
//  CHECKPOINTTHREE = 176;
//  CHECKPOINTFOUR = 213;

  // change to bool function later
  // initialize the before/afer magnet value
  isBeforeMagnet = true;
  isMagnetNotRead = true;

  // initialize the ultrasonic values
  countForUltrasonic = 0;
  stillInRangeUltrasonic = false;

  // Setup the servo
  leverArm.attach(SERVO);
  leverArm.write(SERVOINT);  

  // Interrupts
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR1), IfSmartMagnetDetected, FALLING);
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR2), IfDumbMagnetDetected, FALLING);
  //attachInterrupt(digitalPinToInterrupt(KILL_SWITCH), inKilledState, FALLING);

  // Needed to flush the initial XBee Serial so it does not get interrupted when turned on.
  // Only set true in the initial kill state.
  needToFlush = true;

  // This is how the vehicle performs the killed state
  inKilledState();

  if (needToFlush) {
    while (Serial.available() > 0) {
      Serial.read();
    }
  }

  killStateNotRefreshed = true;

  // Set false after initial kill state.
  needToFlush = false;

  isNotAtLastStationInDestination = false;
  lastStationInDestination = 0;

  testStack = true; // To check the schedule

  isFirstPosTickThrough = true;
  isSecondPosTickThrough = true;

  
}

//////////////////////////////////////// MAIN CODE //////////////////////////////////////////////////////////
void loop()
{ 
  // reattach kill switch interrupt if it was detached inside inKilledState method
//  if(killStateNotRefreshed) {
//    attachInterrupt(digitalPinToInterrupt(KILL_SWITCH), inKilledState, FALLING);
//  }
  
  // Makes the packets of information do something when it is full
  if ((String(commands[2]) + String(commands[3])).toInt() > 0){
    resolveReceivedPacketData();
  }

  // System controls
  controls();

  // Catches the information sent from the server
  receiveXbeeMessage();

//  if(checkingInter) {
//    Serial.println("yo nice interrupt");
//  }

  //delay(10000);

//  if (testStack) {
//    setStackArray(2,3);
//    testStack = false;
//  }
}

//////////////////////////////////////// CONTROLS ///////////////////////////////////////////////////////////
// Function that controls the functions of the system
void controls()
{ 
    //testRFID();
    RFIDisCard = RC522.isCard();
  
    if (!RFIDisCard) {
      RFIDisCard = RC522.isCard();
    }
    
    // Read the magnets
    //magnetRead();
    
    // LED is blinking red if button is pressed
    if (killState())
    {
      // This is how the vehicle performs the killed state
      inKilledState();
    }

     // Changed distance from 30 cm to 15 cm due to pole obstacle
    // LED is Red when statement is true
    if (isSomethingInFront())
    {
      STATEOFCAR = 1;
      setStateLED(STATEOFCAR);
      SPEED = 0;
      setSpeedOfMotors(SPEED);
    }

    // Checks to see if the hall effect sensor on the left of podcar read a magnet
//    else if (hall_1_state == 0 || hall_2_state == 0)
//    {
//      // Checks the Magnets
//      if (isMagnetNotRead) {
//        IfSmartMagnetDetected();
//        IfDumbMagnetDetected();
//        isMagnetNotRead = false;         
//      }
//
//      STATEOFCAR = 3;
//      setStateLED(STATEOFCAR);
//      speedUpFromStop();
//      
//    } 

    // Checks to see if podcar passes an RFID card
    else if (RFIDisCard)
    {
      
      STATEOFCAR = 3;
      setStateLED(STATEOFCAR); 
      
      // Checks if the RFID is a station or not
      if (isAtStation()) {        
        STATIONID = getStationNumber(); 

        if (lastStationInDestination == STATIONID) {
          isNotAtLastStationInDestination = false;
        } 
        
        else {
          isNotAtLastStationInDestination = true;
        }

        // Send Packet to Server first time
        if (didSendStationPacket) {
          if (isNotAtLastStationInDestination) {
            createPacket(0,podNumber,1,0,2,0,0,getStationNumber());
            sendXbee();
            commands[3] = 0;
            didSendStationPacket = false;
          }
          else {
            createPacket(0,podNumber,1,0,4,0,0,getStationNumber());
            sendXbee();
            commands[3] = 0;
            didSendStationPacket = false;
          }
        }            

        // Decide to stay or go
        if (isStoppedAtStation()) {
          speedUpFromStop();
        }

        else {
          setSpeedOfMotors(0);
        } 
      }
      else {
        CHECKPOINT = getCheckpointNumber();
        isBeforeMagnet = true;
        speedUpFromStop(); 
      }
    }

    // LED should be green
    else
    {
      // Checks if the pod is going from destination to destination
      if (!isNotAtLastStationInDestination) {
        // The default state for the car
        STATEOFCAR = 5;
        setStateLED(STATEOFCAR);
      }
      else {
        // The state when the car is going from station to station
        setStateLED(STATEOFCAR);
      }
//      STATEOFCAR = 5;
//      setStateLED(STATEOFCAR);
      isMagnetNotRead = true;
      //setSpeedOfMotors(SPEED);
      speedUpFromStop();
      leverArm.write(SERVOINT);
      howLongToStayAtStation = 0;
      getTicks();

      didSendStationPacket = true;  
      //checkingInter = false;
//      Serial.println("testing ticks: ");  
//      Serial.println(getTicks());
      serverGoFromStation = false; // uncomment when using the server
    }
}

void testInter(){
  checkingInter = true;
}

///////////////////////////////////// RESOLVE PACKET DATA ////////////////////////////////////////////////////////////
// Does action to the packet received from the XBee
void resolveReceivedPacketData() {

  // Checks if the Podcar number is the correct one that the server sent it to  
  if (podNumber == commands[1]) {

    // Initialize the ActionId
    int actionId;
  
    // initialization of ActionId
    if ((String(commands[2]) + String(commands[3])).toInt() > 0) {
      actionId = (String(commands[2]) + String(commands[3])).toInt();
    }
  
    // initialization of variables for the last 4 digits in the int array when needed
    int firstPos;
    int secondPos;
    int thirdPos;
    String ticks;

    int closestStationNumberToThisCar;
  
    // switch based on which command was received from server
    switch (actionId) {
  
      // getStationNumber function
      case 1:
      
        // Create a packet to send to the server
        createPacket(commands[0], commands[1], commands[2], commands[3], 
                    0, 0, 0, getStationNumber());
  
        // Sends the packet to the server
        sendXbee();
        break;
  
      // setStateLED function
      case 2: 
  
        // Sets the LED color of the Pod. Mainly for testing purposes as there is no way to lock it down to one color at the moment
        STATEOFCAR = (String(commands[6]) + String(commands[7])).toInt();
        setStateLED(STATEOFCAR);
        break;
  
      // getStateLED function
      case 3:
  
        // Create a packet to send to the server
        createPacket(commands[0], commands[1], commands[2], commands[3], 
                    0, 0, 0, getStateLED());
  
        // Sends the packet to the server
        sendXbee();
        break;
  
      // setSpeed function
      case 4: 
  
        // Sets the Speed of the Podcars. Might be useful for future iterations of this code
        SPEED = (String(commands[5]) + String(commands[6]) + String(commands[7])).toInt();
        setSpeedOfMotors(SPEED);
        break;
  
      //getSpeed function
      case 5: 
  
        // First, we have to break up each spot of the speed to send to the server
        firstPos = String(SPEED).substring(0,1).toInt();
        secondPos = String(SPEED).substring(1,2).toInt();
        thirdPos = String(SPEED).substring(2).toInt();
  
        // Create a packet to send to the server
        createPacket(commands[0], commands[1], commands[2], commands[3], 
                    0, firstPos, secondPos, thirdPos);
  
        // Sends the packet to the server
        sendXbee();
        break;
  
      // setDestination function
      case 6: 
  
        // Use the values from the packet to create a set track from one station to the next
        setStackArray(commands[5], commands[7]);
        toStation(commands[5]);
        lastStationInDestination = commands[7];
        isNotAtLastStationInDestination = true;
        break;
  
      // getLocation function
      case 7:
  
        // Tick Value
        ticks = String(Peak_Num);
  
        // Location in ticks away from each checkpoint
        firstPos = ticks.substring(0,1).toInt();
        secondPos = ticks.substring(1).toInt();
  
        // Create a packet to send to the server
        createPacket(commands[0], commands[1], commands[2], commands[3], 
                    0, getCheckpointNumber(), firstPos, secondPos);
  
        // Sends the packet to the server
        sendXbee();
        break;
  
      //setNextStation
      case 8:
  
        //Serial.println("not sure yet");
        break;

      case 9:

        // Get the closest station number
        closestStationNumberToThisCar = getClosestStationNumber();

        switch (closestStationNumberToThisCar) {
          case 1:
            createPacket(commands[0], commands[1], commands[2], commands[3], 
                    1, 2, 4, 3);
            break;
            
          case 2:
            createPacket(commands[0], commands[1], commands[2], commands[3], 
                    2, 4, 3, 1);
            break;
            
          case 3:
            createPacket(commands[0], commands[1], commands[2], commands[3], 
                    3, 1, 2, 4);
            break;
            
          case 4:
            createPacket(commands[0], commands[1], commands[2], commands[3], 
                    4, 3, 1, 2);
            break;

        }

        sendXbee();

        break;

      case 10:

        // reserved actionId for sending stationNumber to server
        break;

      case 11:
  
        // Tell car to go from server when stopped
        serverGoFromStation = true;
  
        break;

        // Create packet to send to the server
//        createPacket(commands[0], commands[1], commands[2], commands[3], 
//                    0, 0, 0, closestStationNumberToThisCar);

    }
  
    // Resets the ActionId to zero to let the pod know to accept other commands from the server

    commands[2] = 0;
    commands[3] = 0;   
    
    
  }

  // Reset the ActionId to zero if the pod number is incorrect so this can accept a new job
  else {
    commands[2] = 0;
    commands[3] = 0;
  }
}

///////////////////////////////////// RECEIVE XBEE MESSAGE ////////////////////////////////////////////////////////////
// Receives the message from the server
void receiveXbeeMessage() 
{

  char in;
  int additive;
  int counterForCommands = 0;

  // Fill the array with the list of commands.
  while(Serial.available()) {

    //delay(2);  //small delay to allow input buffer to fill

    // Read and translate message to int in order to store the values into an int array
    in = (char)Serial.read();

    //Serial.println(in);
    
    // Translates the read message to something readable
    additive = (int)in - 48;

    // Stores the message to the int array and increments to receive a new message
    commands[counterForCommands] = additive;
    counterForCommands++;

    // Testing to see if less of the controls methods will be blocked by receiving messages
    controls();
  }

  // Checks if command was sent to correct XBee
  if (podNumber != (String(commands[0]) + String(commands[1])).toInt()) {
    commands[3] = 0;
  }
}

///////////////////////////////////// SEND XBEE ////////////////////////////////////////////////////////////
// Function to send all requested information through Xbee??
void sendXbee()
{

  // Initialize the complete message
  String completeMSG;

  // Check the ActionId value whether to accept values or not
  if((String(commands[2]) + String(commands[3])).toInt() > 0) {

      // Since the int array has 8 values, simply for loop through 8 times.
      for (int checkcounter = 0; checkcounter < 8; checkcounter++) {

          // Store the whole array into one String to send 
          completeMSG += String(commands[checkcounter]);          
          
          //Serial.print(commands[checkcounter]);
      }

      // Send the int array as a String
      Serial.print(completeMSG + 'e');
      
    }

    // Set the ActionId to zero to accept new information
    commands[3] = 0;
  
}

///////////////////////////////////// CREATE PACKET ////////////////////////////////////////////////////////////
void createPacket(int podNumOne, int podNumTwo, int aIdPosOne, int aIdPosTwo,
                  int contentOne, int contentTwo, int contentThree, int contentFour) 
{

  // Fill the array to send data back to the server
  commands[0] = podNumOne;
  commands[1] = podNumTwo;
  commands[2] = aIdPosOne;
  commands[3] = aIdPosTwo;
  commands[4] = contentOne;
  commands[5] = contentTwo;
  commands[6] = contentThree;
  commands[7] = contentFour;
}

// Sets the next station number
// [ActionId = 1, lengthOfArray = 3, the next station number between 1-4]
void setNextStation(int station_number)
{
    STATIONID = station_number;
    SSN = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////START GET FUNCTIONS/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int getClosestStationNumber() {

  int lastCheckpoint = getCheckpointNumber();
  bool magnetCheck = isBeforeMagnet;

  switch(lastCheckpoint) {
    case 1:
      if (magnetCheck) {
        return 2;
      }
      else {
        return 4;
      }
      break;
      
    case 2:
      if (magnetCheck) {
        return 4;
      }
      else {
        return 3;
      }
      break;
      
    case 3:
      if (magnetCheck) {
        return 3;
      }
      else {
        return 1;
      }
      break;
    
    case 4:
      if (magnetCheck) {
        return 1;
      }
      else {
        return 2;
      }
      break;
  }
  
}

///////////////////////////////////// GET CHECKPOINT ////////////////////////////////////////////////////////////
int getCheckpointNumber() 
{

  /* If so then get its serial number */
    RC522.readCardSerial();

    /* Get the Serial number of the tag */
    SSN = RC522.serNum[0];

  // Next four statements are for checkpoint cards
    if (SSN == CHECKPOINTONE)
    {
      CHECKPOINT = 1;
    }

    if (SSN == CHECKPOINTTWO)
    {
      CHECKPOINT = 2;
    }

    if (SSN == CHECKPOINTTHREE)
    {
      CHECKPOINT = 3;
    }

    if (SSN == CHECKPOINTFOUR)
    {
      CHECKPOINT = 4;
    }

    return CHECKPOINT;
}

///////////////////////////////////// GET STATION NUMBER /////////////////////////////////////
// Retrieves the Station ID
int getStationNumber()
{
    /* If so then get its serial number */
    RC522.readCardSerial();

    /* Get the Serial number of the tag */
    SSN = RC522.serNum[0];
    
    // For round tags SSN = 85, For Cards SSN = 35
    if (SSN == STATIONONE)
    {
      STATIONID = 1;
    }
    // For round tags SSN = 101, For Cards SSN = 110
    if (SSN == STATIONTWO)
    {
      STATIONID = 2;
    }
    // For round tags SSN = 149, For Cards SSN = 103
    if (SSN == STATIONTHREE)
    {
      STATIONID = 3;
    }
    // For round tags SSN = 21, For Cards SSN = 132
    if (SSN == STATIONFOUR)
    {
      STATIONID = 4;
    }

    return STATIONID;
}

///////////////////////////////////// GET TICKS /////////////////////////////////////////////////////////
int getTicks()
{
  
  if (!RFIDisCard) {

    int pos1 = analogRead(POS_SENSOR1);    
    int pos2 = analogRead(POS_SENSOR2);

    //Serial.println("pos1");
    //Serial.println(pos1); //265 avg no metal strip/ 255 avg metal strip
//    Serial.println("pos2");
    //Serial.println(pos2); // 265 no strip/ strip is 257
    
    if(pos1 < 257 && isFirstPosTickThrough)
    {
      Peak_Num++;
      //Serial.println(Peak_Num);
      isFirstPosTickThrough = false;
    }

    else if (pos1 < 257) {
      // do nothing      
    }

    else {      
      isFirstPosTickThrough = true;
    }

    if(pos2 < 260 && isSecondPosTickThrough)
    {
      Peak_Num++;      
      //Serial.println(Peak_Num);
      isSecondPosTickThrough = false;
    }

    else if (pos2 < 260) {
      // do nothing
    }

    else {      
      isSecondPosTickThrough = true;
    }
    
  } else {
    
    if (getCheckpointNumber() != tickCounterStation) {
      tickCounterStation = getCheckpointNumber();
      Peak_Num = 0;
    }
  }

  return Peak_Num;
}

///////////////////////////////////// GET POSITION /////////////////////////////////////////////////////////
int getPosition()
{
  // In cm
  return Peak_Num * peakgap / 10;
}

///////////////////////////////////// GET LOCATION /////////////////////////////////////////////////////////
void getLocation()
{
  // Don't know what to do with this just yet
  sendXbee();
}

///////////////////////////////////// GET STATE LED /////////////////////////////////////////////////////////
// Gets the state of the LED
// Used for mobile app purposes
int getStateLED()
{
  return STATEOFCAR;
}

//////////////////////////////////// GET SPEED OF MOTORS ////////////////////////////////////////////////////
//// Retrieves the speed of the motor based on the encoders
double getSpeedOfMotors()
{
  return SPEED;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////START SET FUNCTIONS/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// SET SPEED OF MOTORS ///////////////////////////////////////////////////
// Function that sets the speed of the motors
// A pwm signal of 580 corresponds to a speed of ~0.82 ft/s (~0.25 m/s)
void setSpeedOfMotors(double motor_speed)
{
  Timer3.pwm(motor1, motor_speed);
  Timer3.pwm(motor2, motor_speed);
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

  // LED is blinking red (error)
  if (colorState == 2)
  {
    //unsigned long currentBlink = millis();

    if (currentBlink - previousBlink >= interval)
    {
      
      previousBlink = currentBlink;

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

  // LED is blue (checkpoint detected)
  if (colorState == 3)
  {
    digitalWrite(RED, OFF);
    digitalWrite(GREEN, OFF);
    digitalWrite(BLUE, ON);
  }

  if (colorState == 4)
  {
    //unsigned long currentBlink = millis();

    if (currentBlink - previousBlink >= interval)
    {
     
      previousBlink = currentBlink;

      if (blueState == OFF)
      {
        redState = OFF;
        greenState = OFF;
        blueState = ON;
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
  }

  // LED is green (podcar in motion)
  if (colorState == 5)
  {
    digitalWrite(RED, OFF);
    digitalWrite(GREEN, ON);
    digitalWrite(BLUE, OFF);
  }

  if (colorState == 6)
  {
    //unsigned long currentBlink = millis();

    if (currentBlink - previousBlink >= interval)
    {
      previousBlink = currentBlink;

      if (greenState == OFF)
      {
        redState = OFF;
        greenState = ON;
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
  }

  // LED is purple (podcar in motion)
  if (colorState == 7)
  {
    digitalWrite(RED, ON);
    digitalWrite(GREEN, OFF);
    digitalWrite(BLUE, ON);
  }

  if (colorState == 8)
  {
    //unsigned long currentBlink = millis();

    if (currentBlink - previousBlink >= interval)
    {
      previousBlink = currentBlink;

      if (redState == OFF && blueState == OFF)
      {
        redState = ON;
        greenState = OFF;
        blueState = ON;
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
  }

  // LED is yellow (podcar in motion)
  if (colorState == 9)
  {
    digitalWrite(RED, ON);
    digitalWrite(GREEN, ON);
    digitalWrite(BLUE, OFF);
  }

  if (colorState == 10)
  {
    //unsigned long currentBlink = millis();

    if (currentBlink - previousBlink >= interval)
    {
      previousBlink = currentBlink;

      if (redState == OFF && greenState == OFF)
      {
        redState = ON;
        greenState = ON;
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
  }

  checkBlinks();
}

void toStation(int startingDestination){

  switch (startingDestination) {

    case 1:
      goToStationOne();
      break;

    case 2:
      goToStationTwo();
      break;

    case 3:
      goToStationThree();
      break;

    case 4:
      goToStationFour();
      break;
    
  }
  
}

void goToStationOne() {

  int checkpoint = getCheckpointNumber();
  int magnetCheck = isBeforeMagnet;

  switch (checkpoint) {

    case 1:

      if(magnetCheck) {
        for(int i = (sizeof(exactlyAfterStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(exactlyAfterStationOneFour[i]);
        }
      }
      
      else {
        for(int i = (sizeof(fromMiddleIntoStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(fromMiddleIntoStationOneFour[i]);
        }
      }
      break;

    case 2:

      if(magnetCheck) {
        for(int i = (sizeof(fromMiddleIntoStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(fromMiddleIntoStationOneFour[i]);
        }
      }
      
      else {
        for(int i = (sizeof(passMiddleIntoStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(passMiddleIntoStationOneFour[i]);
        }
      }
      break;

    case 3:

      if(magnetCheck) {
        for(int i = (sizeof(passMiddleIntoStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(passMiddleIntoStationOneFour[i]);
        }
      }
      
      else {
        for(int i = (sizeof(switchIntoClosestStation)/2)-1; i > -1 ; i--) {
          scheduleArray.push(switchIntoClosestStation[i]);
        }
      }
      break;

    case 4:

      if(magnetCheck) {
        for(int i = (sizeof(switchIntoClosestStation)/2)-1; i > -1 ; i--) {
          scheduleArray.push(switchIntoClosestStation[i]);
        }
      }
      
      else {
        for(int i = (sizeof(exactlyAfterStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(exactlyAfterStationOneFour[i]);
        }
      }
      break;
  }
}

void goToStationTwo() {

  int checkpoint = getCheckpointNumber();
  int magnetCheck = isBeforeMagnet;

  switch (checkpoint) {

    case 1:

      if(magnetCheck) {
        for(int i = (sizeof(exactlyBeforeStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(exactlyBeforeStationTwoThree[i]);
        }
      }
      
      else {
        for(int i = (sizeof(fullCircleIntoStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(fullCircleIntoStationTwoThree[i]);
        }
      }
      break;

    case 2:

      if(magnetCheck) {
        for(int i = (sizeof(fullCircleIntoStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(fullCircleIntoStationTwoThree[i]);
        }
      }
      
      else {
        for(int i = (sizeof(afterStationOneFourToTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(afterStationOneFourToTwoThree[i]);
        }
      }
      break;

    case 3:

      if(magnetCheck) {
        for(int i = (sizeof(afterStationOneFourToTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(afterStationOneFourToTwoThree[i]);
        }
      }
      
      else {
        for(int i = (sizeof(halfCircleIntoStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(halfCircleIntoStationTwoThree[i]);
        }
      }
      break;

    case 4:

      if(magnetCheck) {
        for(int i = (sizeof(halfCircleIntoStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(halfCircleIntoStationTwoThree[i]);
        }
      }
      
      else {
        for(int i = (sizeof(exactlyBeforeStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(exactlyBeforeStationTwoThree[i]);
        }
      }
      break;
  }
}

void goToStationThree() {

  int checkpoint = getCheckpointNumber();
  int magnetCheck = isBeforeMagnet;

  switch (checkpoint) {

    case 1:

      if(magnetCheck) {
        for(int i = (sizeof(afterStationOneFourToTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(afterStationOneFourToTwoThree[i]);
        }
      }
      
      else {
        for(int i = (sizeof(halfCircleIntoStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(halfCircleIntoStationTwoThree[i]);
        }
      }
      break;

    case 2:

      if(magnetCheck) {
        for(int i = (sizeof(halfCircleIntoStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(halfCircleIntoStationTwoThree[i]);
        }
      }
      
      else {
        for(int i = (sizeof(exactlyBeforeStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(exactlyBeforeStationTwoThree[i]);
        }
      }
      break;

    case 3:

      if(magnetCheck) {
        for(int i = (sizeof(exactlyBeforeStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(exactlyBeforeStationTwoThree[i]);
        }
      }
      
      else {
        for(int i = (sizeof(fullCircleIntoStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(fullCircleIntoStationTwoThree[i]);
        }
      }
      break;

    case 4:

      if(magnetCheck) {
        for(int i = (sizeof(fullCircleIntoStationTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(fullCircleIntoStationTwoThree[i]);
        }
      }
      
      else {
        for(int i = (sizeof(afterStationOneFourToTwoThree)/2)-1; i > -1 ; i--) {
          scheduleArray.push(afterStationOneFourToTwoThree[i]);
        }
      }
      break;
  }
}

void goToStationFour() {

  int checkpoint = getCheckpointNumber();
  int magnetCheck = isBeforeMagnet;

  switch (checkpoint) {

    case 1:

      if(magnetCheck) {
        for(int i = (sizeof(passMiddleIntoStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(passMiddleIntoStationOneFour[i]);
        }
      }
      
      else {
        for(int i = (sizeof(switchIntoClosestStation)/2)-1; i > -1 ; i--) {
          scheduleArray.push(switchIntoClosestStation[i]);
        }
      }
      break;

    case 2:

      if(magnetCheck) {
        for(int i = (sizeof(switchIntoClosestStation)/2)-1; i > -1 ; i--) {
          scheduleArray.push(switchIntoClosestStation[i]);
        }
      }
      
      else {
        for(int i = (sizeof(exactlyAfterStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(exactlyAfterStationOneFour[i]);
        }
      }
      break;

    case 3:

      if(magnetCheck) {
        for(int i = (sizeof(exactlyAfterStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(exactlyAfterStationOneFour[i]);
        }
      }
      
      else {
        for(int i = (sizeof(fromMiddleIntoStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(fromMiddleIntoStationOneFour[i]);
        }
      }
      break;

    case 4:

      if(magnetCheck) {
        for(int i = (sizeof(fromMiddleIntoStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(fromMiddleIntoStationOneFour[i]);
        }
      }
      
      else {
        for(int i = (sizeof(passMiddleIntoStationOneFour)/2)-1; i > -1 ; i--) {
          scheduleArray.push(passMiddleIntoStationOneFour[i]);
        }
      }
      break;
  }
}

//////////////////////////////////// SET STACK ARRAY /////////////////////////////////////////////////////////
void setStackArray(int from, int to)
{

  // Generate the value to compare to each of these
  int fromTo = (String(from) + String(to)).toInt();
  
  // 
  if (scheduleArray.isEmpty()) {

    // Create the stack array to process the course that the pod car has to take
    switch(fromTo)
      {
       case 12:
         for(int i = (sizeof(path1to2)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path1to2[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
      case 13:
          for(int i = (sizeof(path1to3)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path1to3[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
       case 14:
          for(int i = (sizeof(path1to4)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path1to4[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 21:
          for(int i = (sizeof(path2to1)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path2to1[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 23:
          for(int i = (sizeof(path2to3)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path2to3[i]);
            //Serial.println(scheduleArray.peek());
          }
          //helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 24:
          for(int i = (sizeof(path2to4)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path2to4[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 31:
          for(int i = (sizeof(path3to1)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path3to1[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 32:
          for(int i = (sizeof(path3to2)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path3to2[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 34:
          for(int i = (sizeof(path3to4)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path3to4[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 41:
          for(int i = (sizeof(path4to1)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path4to1[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 42:
          for(int i = (sizeof(path4to2)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path4to2[i]);
            //Serial.println(scheduleArray.peek());
          }
          //helpSetStackArray(firstChoiceForInitialPath);
          break;
  
        case 43:
          for(int i = (sizeof(path4to3)/2)-1; i > -1 ; i--) 
          {
            scheduleArray.push(path4to3[i]);
            //Serial.println(scheduleArray.peek());
          }
          helpSetStackArray(firstChoiceForInitialPath);
          break;
        }
        
    }
}

////////////////////////////////// HELP SET STACK ARRAY//////////////////////////////////////////////////
//Helps the setStackArray function
void helpSetStackArray(bool firstChoice) {
//  if (firstChoice) {
//    scheduleArray.push(1);
//  }
//  else {
//    scheduleArray.push(0);
//    scheduleArray.push(1);
//  }
}

////////////////////////////////// SET NEXT STATION NUMBER //////////////////////////////////////////////////
// Sets the next station number
void setNextStationNumber(int station_number)
{
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////START HELPER FUNCTIONS//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// CHECK BLINKS //////////////////////////////////////////////////
void checkBlinks() 
{
    
  if (currentBlink > 20) {
    currentBlink = 10;
    previousBlink = 5;
  }

  currentBlink++;
}

///////////////////////////////////// IS AT STATION ////////////////////////////////////////////////////////////
// Checks if the RFID card is at a station or not
bool isAtStation() {

  /* If so then get its serial number */
  RC522.readCardSerial();

  /* Get the Serial number of the tag */
  SSN = RC522.serNum[0];

  //Serial.println(SSN);

  // Checks for each Station Number to see if the Podcar is at a Station
  if (SSN == STATIONONE || SSN == STATIONTWO || SSN == STATIONTHREE || SSN == STATIONFOUR) {
    return true;
  }
  else {
    return false;
  }
}

//////////////////////////////////// IS SOMETHING IN FRONT //////////////////////////////////////////////////
// Detects if anything is in front of the podcar
bool isSomethingInFront()
{
  ultrasonic();
  ultrasonic2();

  bool collision_detection;
  
  if(distance <= 15)
  {
    countForUltrasonic++;

    if (countForUltrasonic > 6 || stillInRangeUltrasonic) {
      countForUltrasonic = 0;
      stillInRangeUltrasonic = true;
      collision_detection = true;
    }
    
  }

  else
  {
    countForUltrasonic = 0;
    stillInRangeUltrasonic = false;
    collision_detection = false; 
  }

  return collision_detection;
}

////////////////////////////////// KILL STATE ///////////////////////////////////////////////////////////////
// Checks the status of the button
bool killState()
{
  // Continuously check the status of the kill button
  if (!digitalRead(KILL_SWITCH) && !checkForSecondKillPress) {
      killStatePressedTwice++;
      checkForSecondKillPress = true;
      return true;
  }

  else if (!digitalRead(KILL_SWITCH)) {
      return true;
  }
  
  else {
      checkForSecondKillPress = false;
      return false;
  }
}

//////////////////////////////////// MAGNET READ ////////////////////////////////////////////////////////////
// Retrieves the readings from the hall effect sensors
void magnetRead()
{
  // Checks if any magnet is read on the left of podcar
  hall_1_state = digitalRead(HALL_SENSOR1);


  // Checks if any magnet is read on the right of podcar
  hall_2_state = digitalRead(HALL_SENSOR2);
}

///////////////////////////////////////// ULTRASONIC ////////////////////////////////////////////////////////
// Function that will return the distance from the ultrasonic sensor in the front of the car
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
}

///////////////////////////////////////// ULTRASONIC2 ////////////////////////////////////////////////////////
// Function that will return the distance from the ultrasonic sensoron the side of the car
void ultrasonic2()
{
  /* The following trigPin/echoPin cycle is used to determine the distance of 
   *  the nearest object by bouncing soundwaves off of it 
   */

   digitalWrite(TRIG2, OFF);
   delayMicroseconds(2);

   digitalWrite(TRIG2, ON);
   delayMicroseconds(10);

   digitalWrite(TRIG2, OFF);

   // Returns the length of the pulse in microseconds
   duration = pulseIn(ECHO2, ON);

   // Calculate the distance (in cm) based on the speed of sound
   distance = duration/58.2;
}

///////////////////////////////// IF SMART MAGNET DETECTED //////////////////////////////////////////////////
void IfSmartMagnetDetected()
{

  magnetRead();

//  if (hall_1_state == 0) //If smart sensor detects a magnet
//  {
//    //Serial.println("smart");
//    SERVOINT = 20;
//    leverArm.write(SERVOINT);
//    //Serial.print(hall_2_state);
//  }

    if (hall_1_state==0) {

      // change to bool function later
      isBeforeMagnet = false;

      if (!scheduleArray.isEmpty()) //If smart sensor detects a magnet and stack is not empty
      {
        int laststackvalue = scheduleArray.pop(); // Pop and assign last element of stack to laststackvalue
     
        switch(laststackvalue) //Based on the value of the last element, switch or not don't switch
        {
          case 1: 
            SERVOINT = 20;
            leverArm.write(SERVOINT);
            //Serial.println("Switched");
            break;
    
          case 0:
            SERVOINT = 140;
            leverArm.write(SERVOINT);
            //Serial.println("Not Switched");
            break;
        }
      }

      else {
        SERVOINT = 20;
        leverArm.write(SERVOINT);
      }

    }
}

///////////////////////////////// IF DUMB MAGNET DETECTED ////////////////////////////////////////////////////
// Switches the lever arm if a dumb magnet is detected
void IfDumbMagnetDetected()
{
  magnetRead();
  
  if (hall_2_state == 0) //If dumb sensor detects a magnet
  {
    SERVOINT = 140;
    leverArm.write(SERVOINT);
    //Serial.print(hall_2_state);
  }
}

///////////////////////////////// SPEED UP FROM STOP ////////////////////////////////////////////////////
// Makes the Car accelerate to a certain maximum speed
void speedUpFromStop() {
  
  if (SPEED < maximumSpeed) {
    SPEED += incrementByFifty;
  }

  setSpeedOfMotors(SPEED);
}

// example of interrupt kill state
void kindaKillState() {

  int stay = 1;

  while (stay == 1) {
    STATEOFCAR = 2;
    setStateLED(STATEOFCAR);
    SPEED = 0;
    setSpeedOfMotors(SPEED);

    if (digitalRead(RESET) < 1) {
      stay = 0;
    }
  }
  
}

///////////////////////////////// INSIDE THE KILLED STATE ////////////////////////////////////////////////////
// This is how the vehicle performs the killed state
//void inKilledState() {
//
//  // Creates a latching button
//  int stayInKill = 1;
//  bool killPressedOnce = true;
//
//  // Make it so that the kill switch interrupt pin is reattached later.
//  killStateNotRefreshed = true;
//
//  // detach the kill switch for the kill state
//  detachInterrupt(digitalPinToInterrupt(KILL_SWITCH));
//
//  // Continues to blink the LED red while latching button is in effect
//  // Only way to stop this cycle is to reset Arduino or add another button
//  while(stayInKill == 1)
//  {
//    // This if/else block toggles the angle of the servo arm based on how many times the kill switch has been pressed.
//    if ((killStatePressedTwice % 2 == 0 && digitalRead(KILL_SWITCH) < 1) && killPressedOnce) {
//      SERVOINT = 20;
//      leverArm.write(SERVOINT);  
//      killStatePressedTwice++;
//      killPressedOnce = false;
//    }
//    else if (killStatePressedTwice % 2 == 0 && digitalRead(KILL_SWITCH) < 1) {
//      // Only pressed once.
//    }
//    else {
//      SERVOINT = 140;
//      leverArm.write(SERVOINT);
//      killStatePressedTwice++;
//      killPressedOnce = true;
//    }  
//    
//    STATEOFCAR = 2;
//    setStateLED(STATEOFCAR);
//    SPEED = 0;
//    setSpeedOfMotors(SPEED);
//    
//    if (digitalRead(RESET) < 1) {
//      stayInKill = 0;
//    }
//    //delay(35);
//  }
//  
//  killStatePressedTwice = 0;
//  
//}

///////////////////////////////// STOP AT A STATION ////////////////////////////////////////////////////
// Stops the car at a station for a bit
bool isStoppedAtStation() {

  if (howLongToStayAtStation > 50 && serverGoFromStation) {
    return true;
  }

  else if (howLongToStayAtStation > 50 ) {
    // waiting for command to go from station
    return false;
  }

  else {
    howLongToStayAtStation++;
    return false;
  }
}

// This is how the vehicle performs the killed state
void inKilledState() {

  //killState();
  
  // Creates a latching button
  kill_latch_state = false;
  reset_latch_state = true;

  // Continues to blink the LED red while latching button is in effect
  // Only way to stop this cycle is to reset Arduino or add another button
  while(kill_latch_state == false && reset_latch_state == true)
  {
    // This if/else block toggles the angle of the servo arm based on how many times the kill switch has been pressed.
    if (killStatePressedTwice % 2 == 0 && killStatePressedTwice != 0) {
      SERVOINT = 20;
      leverArm.write(SERVOINT);  
    }
    else {
      SERVOINT = 140;
      leverArm.write(SERVOINT);
    }  
    
    STATEOFCAR = 2;
    setStateLED(STATEOFCAR);
    SPEED = 0;
    setSpeedOfMotors(SPEED);
    if (needToFlush) {
      while (Serial.available() > 0) {
        Serial.read();
      }
    }
    delay(35);
  }

  //Serial.println("after killed state");

  killStatePressedTwice = 0;
  
}

//int getClosestStationNumber() {
///**
//  to Station 1 from each Checkpoint's forward magnet:
//    CH4 = 100 ticks
//    CH3 = 246 ticks
//
//  to Station 2 from each Checkpoint's forward magnet:
//    CH1 = 20 ticks
//    CH4 = after 100 ticks
//
//  to Station 3 from each Checkpoint's forward magnet:
//    CH3 = 20 ticks
//    CH2 = after 100 ticks
//
//  to Station 4 from each Checkpoint's forward magnet:
//    CH2 = 100 ticks
//    CH1 = 246 ticks
//
//  Order of closest Checkpoint starting from Station 1:
//    4, 3, 2, 1
//
//  Order of closest Checkpoint starting from Station 2:
//    1, 4, 3, 2
//
//  Order of closest Checkpoint starting from Station 3:
//    3, 2, 1, 4
//
//  Order of closest Checkpoint starting from Station 4:
//    2, 1, 4, 3
//  */
//
//  int currentTicks = getTicks();
//  int currentCheckpoint = tickCounterStation;
//
//  switch (currentCheckpoint) {
//    case 1:
//
//      if (currentTicks < 20) {
//        firstChoiceForInitialPath = true;
//        return 2;
//      }
//
//      else {
//        firstChoiceForInitialPath = false;
//        return 4;
//      }
//      
//    break;
//    
//    case 2:
//
//      if (currentTicks < 100) {
//        firstChoiceForInitialPath = true;
//        return 4;
//      }      
//
//      else {
//        firstChoiceForInitialPath = false;
//        return 3;
//      }
//
//    break;
//    
//    case 3:
//
//      if(currentTicks < 20) {
//        firstChoiceForInitialPath = true;
//        return 3;
//      }
//
//      else {
//        firstChoiceForInitialPath = false;
//        return 1;
//      }
//      
//    break;
//    
//    case 4:
//
//      if (currentTicks < 100) {
//        firstChoiceForInitialPath = true;
//        return 1;
//      }
//
//      else {
//        firstChoiceForInitialPath = false;
//        return 2;
//      }
//
//    break;
//    
//  }
//}




