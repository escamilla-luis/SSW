#include <Servo.h>
#define HALL_SENSOR1  2  // Switch servo motor using hall effect sensor mounted on left of podcar
#define HALL_SENSOR2  4  // Switch servo motor using hall effect sensor mounted on right of podcar
#define SERVO_PIN     3  // PWM pin to control the angle of the servo

int hall_1_state;
int hall_2_state;
Servo myservo;
void setup()
{
  myservo.attach(SERVO_PIN);
  Serial.begin(9600);
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);

}

void loop()
{ 
 // Checks if any magnet is read on the left of podcar
  hall_1_state = digitalRead(HALL_SENSOR1);
  delay(50);
  
 // Checks if any magnet is read on the right of podcar
  hall_2_state = digitalRead(HALL_SENSOR2);
  delay(50);

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
    Serial.print(hall_2_state);
    delay(250);
  }

    Serial.print(hall_1_state);
  Serial.print(hall_2_state);
}
