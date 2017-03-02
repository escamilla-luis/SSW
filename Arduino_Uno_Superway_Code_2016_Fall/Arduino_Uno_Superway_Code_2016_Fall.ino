// Libraries that will control the servo, the RFID, and allow I2C communication
#include <AddicoreRFID.h>
#include <Wire.h>
#include <Servo.h>


// Pin Definitions
                                // Pins A4 and A5 are used for I2C comm between Micro and Uno
                                // Pins 0 and 1 are used for Xbee comms
#define HALL_SENSOR1  2         // Switch servo motor using hall effect sensor mounted on left of podcar
#define HALL_SENSOR2  4         // Switch servo motor using hall effect sensor mounted on right of podcar
#define MOTOR1_DIR    A0        // Encoder mounted on motor 1
#define MOTOR2_DIR    A1        // Encoder mounted on motor 2
#define MOTOR1        5         // PWM pin to control speed of motor 1
#define MOTOR2        6         // PWM pin to control speed of motor 2
#define SERVO_PIN     3         // PWM pin to control the angle of the servo
#define OBSTRUCTION   8         // Stops motors based on ultrasonic reading
#define ERR_BUTTON    7         // Stops motors based on button reading


// RFID data type definition
#define uchar   unsigned char
#define uint    unsigned int
uchar fifobytes;
uchar fifoValue;


// Names the RFID reader
AddicoreRFID myRFID;      // create AddicoreRFID object to control the RFID module

////////////////////////////////////////////////////////////////////////////////////
// set the pins
////////////////////////////////////////////////////////////////////////////////////

// Pins that SDA (PIN 10) and RESET (PIN 9) are connected to on Uno
const int chipSelectPin = 10;
const int NRSTPD = 9;


//Maximum length of the array
#define MAX_LEN 16


// Variables that store the state of the button and ultrasonic to make
// the podcar stop
int button;
int ultrasonic;


// Values to store the readings from the hall effect sensors
int hall_1_state;
int hall_2_state;


// For button state
#define NOT_PRESSED          LOW
#define IS_PRESSED           HIGH


// For ultrasonic
#define OFF                  LOW
#define ON                   HIGH


// Stores the station number
int StationId;            


// Declares and recognizes the servo
Servo myservo;


void setup() 
{
  // Inputs
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);
  pinMode(ERR_BUTTON, INPUT);
  pinMode(OBSTRUCTION, INPUT);


  // Outputs
  pinMode(chipSelectPin, OUTPUT);     // Set digital pin 10 as OUTPUT to connect it to the RFID/ENABLE pin
  pinMode(NRSTPD, OUTPUT);            // Set digital pin 10, Not Reset and Power-down


  digitalWrite(NRSTPD, HIGH);
  digitalWrite(chipSelectPin, LOW);   // Activate the RFID reader


  // Begins communication between the RFID reader and the Uno
  myRFID.AddicoreRFID_Init();


  // Recognizes the servo
  myservo.attach(SERVO_PIN);
  

  // Makes sure that at startup, the motors are off and the servo starts at 30 degrees
  myservo.write(30);
  analogWrite(MOTOR1, 0);
  analogWrite(MOTOR2, 0);

  
  // Serial must be initialized for Xbee comm and Arduino comm
  Wire.begin(8);        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop()
{
  // Checks the state of the button continuously
  button = digitalRead(ERR_BUTTON);


  // Checks the distance from the ultrasonic sensor
  ultrasonic = digitalRead(OBSTRUCTION);


  // Checks if any magnet is read on the left of podcar
  hall_1_state = digitalRead(HALL_SENSOR1);
  delay(50);


  // Checks if any magnet is read on the right of podcar
  hall_2_state = digitalRead(HALL_SENSOR2);
  delay(50);


  // Retrieves the station that the podcar is at if a tag is read
  getStationID();


  // Checks to see if the button is pressed
  if(button == IS_PRESSED && ultrasonic == OFF)
  {
    // Motors are off if button is pressed
    analogWrite(MOTOR1, 0);
    analogWrite(MOTOR2, 0);
  }

  // Checks to see if there is any obstruction
  if (button == NOT_PRESSED && ultrasonic == ON)
  {
    // Motors are off if distance is below or at 15 cm
    analogWrite(MOTOR1, 0);
    analogWrite(MOTOR2, 0);
  }

  // Checks to see if the hall effect sensor on the left of podcar read a magnet
  if (hall_1_state == 0)
  {
    // Switches the lever arm so that left arm is up
    myservo.write(30);
    delay(250);
  }

  // Checks to see if the hall effect sensor on the right of podcar read a magnet
  else if(hall_2_state == 0)
  {
    // Switches the lever arm so that right arm is up
    myservo.write(150);
    delay(250);
  }
}


// Retrieves the Station ID
void getStationID()
{
  // Variables used to retrieve the ID of a tag
  uchar i, tmp, checksum1;
  uchar status;
  uchar str[MAX_LEN];
  uchar RC_size;
  uchar blockAddr;  //Selection operation block address 0 to 63
  String mynum = "";

  str[1] = 0x4400;

        
  //Find tags, return tag type
  status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str); 
  
  if (status == MI_OK)
  {
   uint tagType = str[0] << 8;
   tagType = tagType + str[1];
   switch (tagType) 
   {
    case 0x4400:
    //Serial.println("Mifare UltraLight");
    break;
         
    case 0x400:
    //Serial.println("Mifare One (S50)");
    break;
         
    case 0x200:
    //Serial.println("Mifare One (S70)");
    break;
    
    case 0x800:
    //Serial.println("Mifare Pro (X)");
    break;
    
    case 0x4403:
    //Serial.println("Mifare DESFire");
    break;
    
    default:
    //Serial.println("Unknown");
     break;
   }
  }


  //Anti-collision, return tag serial number 4 bytes
  status = myRFID.AddicoreRFID_Anticoll(str);

  if (status == MI_OK)
  {
   checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
   //Serial.print("The tag's number is:\t");
   //Serial.print(str[0]);
   //Serial.print(" , ");
   //Serial.print(str[1]);
   //Serial.print(" , ");
   //Serial.print(str[2]);
   //Serial.print(" , ");
   //Serial.println(str[3]);

   //Serial.print("Read Checksum:\t\t");
   //Serial.println(str[4]);
   //Serial.print("Calculated Checksum:\t");
   //Serial.println(checksum1);

            
   //Checks STATION ID based on RFID TAG ID
   if (checksum1==50)
   {
    // If the tag that is read returns a checksum of 50, podcar is at Station 1
    StationId=1;
    Serial.print("StationId: \t\t");
    Serial.print(StationId);
    Serial.println();


    // Podcar is in transit from Station 1 to Station 2
    analogWrite(MOTOR1, 150);
    analogWrite(MOTOR2, 150);
   }

   //Checks STATION ID based on RFID TAG ID         
   if(checksum1==94)
   {
    // If the tag that is read returns a checksum of 94, podcar is at Station 2
    StationId=2;
    Serial.print("StationId: \t\t");
    Serial.print(StationId);
    Serial.println();


    // Podcar has arrived at Station 2 and has stopped
    analogWrite(MOTOR1, 0);
    analogWrite(MOTOR2, 0);
   }
            
            
   // Should really check all pairs, but for now we'll just use the first
   if(str[0] == 197)   //You can change this to the first byte of your tag by finding the card's ID through the Serial Monitor
   {
    //Serial.println("\nHello Craig!\n");
   } 
   else if(str[0] == 244) //You can change this to the first byte of your tag by finding the card's ID through the Serial Monitor
   {             
    //Serial.println("\nHello Erin!\n");
   }
   //Serial.println();
   delay(1000);
  }
  
  myRFID.AddicoreRFID_Halt();      //Command tag into hibernation 
}
