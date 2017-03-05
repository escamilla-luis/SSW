 #include <TimerOne.h>
// Pin Definitions
#define motor1_dir          A0  //
#define motor2_dir          A1  //
#define motor_enco_1a       A2  // Interrupt pin using custom interrupt setup
#define motor_enco_1b       A3  // (Motor 2 encoder on pins A4, A5 - not used)
#define motor1              9   // PWM pin
#define motor2              10  // PWM pin

// Control System Variables
int sense_rpm = 0, pwm_out = 0, rpm_err;
int acc_err, enc_count, set_rpm;
const float v_Kp = 0.01, v_Ki = 0.1;
boolean enc_prestate;
int motor1_speed, motor2_speed;

// Speed Controller Variables
unsigned long last_update, ping;
long period = 1000 / 20;
int control_rpm;

// Status variables
int value;

void setup() 
{
  Serial.println(motor1_speed);
  Serial.println(motor2_speed);
}

void loop() 
{
  // put your main code here, to run repeatedly:

}

void getSpeedOfMotor()
{
  if(value == 0 || value == 2)
  {
    motor1_speed = 0;    // podcar has stopped or there is an error
    motor2_speed = 0;
  }
  else if(value == 1)
  {
    motor1_speed = set_rpm; // podcar is moving at predefined rpm 
    motor2_speed = set_rpm;
  }
  // outputs an int value for the current speed of the motor
}

void setSpeedOfMotor(int pwm_speed)
{
  Timer1.setPwmDuty(motor1, pwm_speed);
  Timer1.setPwmDuty(motor2, pwm_speed);
  // takes an int as input to set the speed of the motor
}

