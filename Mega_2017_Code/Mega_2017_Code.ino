// Libraries that will control the servo, the RFID
#include <RFID.h>
#include <SPI.h>


// Pin definitions
// Pins 0 and 1 on the Mega are used for Xbee Comms
/* Note: For RFID reader - SDA (PIN 9), SCK (PIN 52), MOSI (PIN 51),
 *  MISO (PIN 50), RST (PIN 8).
 */
#define RED             2   // STOP and Error status
#define GREEN           3   // GO status
#define BLUE            4   // Status of the podcar
#define ECHO            22  // Echo pin
#define TRIG            23  // Trig pin
#define BUTTON          24  // Kill switch
#define HALL_SENSOR1    25  // Switch servo motor using hall effect sensor mounted on left of podcar
#define HALL_SENSOR2    26  // Switch servo motor using hall effect sensor mounted on right of podcar


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
boolean switch_state = true;


// Stores the state of the button
int buttonState;


// Used for the ultrasonic sensor
int maximumRange = 200;   // Maximum range needed
int minimumRange = 0;     // Minimum range needed
long duration;            // Duration used to calculate distance
int distance;             // Stores the distance


// Variables used to store the readings from the hall effect sensors
int hall_1_state;
int hall_2_state;


/* Note: RGB LED- Green (GO status), RED (STOP), BLUE (Hall effect sensor reading)
 *  Blinking BLUE (Station arrival), Blinkin RED (Error)
 */


// Used for LED purposes
#
int redState = OFF;       // Initially, all LED states should be off
int greenState = OFF;
int blueState = OFF;


unsigned long previousTime = 0;
const long interval = 250;


// Stores the station number
long StationId = 0;


// Used to identify which podcar it is
int vehicle_number = 1;


 
void setup()
{
  // Outputs
  pinMode(TRIG, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  

  // Inputs
  pinMode(ECHO, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);

  
  // Change this to 57600 for Xbee Comm
  Serial.begin(9600);


  /* Initialize the RFID reader */
  SPI.begin();


  /* Initialize the RFID reader */
  RC522.init();
}



void loop()
{
  // Retrieves the state of the button
  killSwitch();


  // Function that will check to see if any of the hall effect sensors read a magnet
  switchLever();


  // Retrieves the station that the podcar is at if a tag is read
  getStationID();


  // Retrieves the distance from the ultrasonic sensor
  ultrasonic();


  // Sets the status of the indicator
  indicator();
}



// Retrieves the Station ID
void getStationID()
{
  /* Has a card been detected? */
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();


    /* Display the Serial number of the tag */
    Serial.println("Card detected:");


    for (int i = 0; i < 1; i++)
    {
      StationId = RC522.serNum[i];
      Serial.print(StationId, DEC);
    }
    Serial.println();
  }
  delay(50);
}



// Checks the status of the button
void killSwitch()
{
  // Continuously check the status of the button
  buttonState = digitalRead(BUTTON);
}



// Retrieves the readings from the hall effect sensors
void switchLever()
{
  // Checks if any magnet is read on the left of podcar
  hall_1_state = digitalRead(HALL_SENSOR1);


  // Checks if any magnet is read on the right of podcar
  hall_2_state = digitalRead(HALL_SENSOR2);
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
}



// Turns on the LED as solid state
void solidState(int colorState)
{
  // LED is red
  if (colorState == 1)
  {
    digitalWrite(RED, ON);
    digitalWrite(GREEN, OFF);
    digitalWrite(BLUE, OFF);
  }

  // LED is blue
  if (colorState == 2)
  {
    digitalWrite(RED, OFF);
    digitalWrite(GREEN, OFF);
    digitalWrite(BLUE, ON);
  }

  // LED is green
  if (colorState == 3)
  {
    digitalWrite(RED, OFF);
    digitalWrite(GREEN, ON);
    digitalWrite(BLUE, OFF);
  }
}
  


// Blinks the LED red
void blinkRed()
{
  unsigned long currentRedblink = millis();

  if (currentRedblink - previousTime >= interval)
  {
    previousTime = currentRedblink;

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
  if (currentRedblink > 8000)
  {
    buttonState = 1;
    switch_state = !switch_state;
  }
}



// Blinks the LED blue
void blinkBlue()
{
  unsigned long currentBlueblink = millis();

  if (currentBlueblink - previousTime >= interval)
  {
    previousTime = currentBlueblink;

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
  if (currentBlueblink > 8000)
  {
    StationId = 0;
  }
}



// Turns on the LED based on the indicator status
void indicator()
{
     // Changed distance from 30 cm to 15 cm due to pole obstacle
   // LED is Red when statement is true
   if (distance <= 15)
   {
    solidState(1);
   }

   // LED is blinking red if button is pressed
   else if (buttonState < 1)
   {
    // Creates a latching button
    switch_state = !switch_state;

    // Blinks the LED red
    do
    {
      blinkRed();
    }
    // Continues to blink while latching button is in effect
    // Only way to stop this cycle is to reset Arduino or add another button
    while(switch_state == false);
   }

   // Checks to see if the hall effect sensor on the left of podcar read a magnet
   else if (hall_1_state == 0 || hall_2_state == 0)
   {
    solidState(2);
   }

   // Checks to see if podcar is at a station
   else if (StationId == 35 || StationId == 53 || StationId == 149 || StationId == 132)
   {
    for(int i = 0; i < 10; i++)
    {
      blinkBlue();
    } 
   }

   // LED should be green
   else
   {
    solidState(3);
   }
   
   // delay 50 ms before next reading
   delay(50);  
}

