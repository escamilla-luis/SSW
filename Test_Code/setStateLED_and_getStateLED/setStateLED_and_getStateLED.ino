int greenPin = 3;              // pin 4, CC LED
int bluePin = 5;               // pin 3, CC LED
int redPin = 6;                // pin 1, CC LED

void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 

  Serial.begin(9600); 
  
}

void loop()
{

  setStateLED(0);     // this is used for testing purposes to set the LED

  int value;      // error state by default

  value = getStateLED();

  Serial.println(value);
  delay(500);

}

void setStateLED(int status)
{
  if (status == 0)                  // 0 status is for stopped (red solid)
  {
    digitalWrite(redPin, HIGH);
  }

  if (status == 1)                  // 1 status is for healthy (green solid)
  {
    digitalWrite(greenPin, HIGH);
  }

  if (status == 2)                  // 2 status is for error (red flashing)
  {
    for (int i=0; i < 5; i++)
    {
      digitalWrite(redPin, HIGH);
      delay(500);
      digitalWrite(redPin, LOW);
      delay(500);
    }
  }

  if (status == 3)                  // 3 status is for checkpoint reached (blue solid)
  {
    digitalWrite(bluePin, HIGH);
  }


}

int getStateLED()
{  
  if (digitalRead(6) == 1)                 // if red pin is on solid
  {
    return 0;                              // 0 status is for stopped (red solid)
  }

  if (digitalRead(3) == 1)                 // if green pin is on solid
  {
    return 1;                              // 1 status is for healthy (green solid)
  }

  if (digitalRead(3) == 0 && digitalRead(5) == 0 && digitalRead(6) == 0)                 // if red pin is flashing 
  {
    return 4;                              // 4 status is for LEDs off 
  }

  if (digitalRead(5) == 1)                 // if blue pin is on solid 
  {
    return 3;                              // 3 status is for checkpoint (blue solid) 
  }

  else
  {
    return 2;                              // 2 status is for error (red flashing)
  }
  
}

