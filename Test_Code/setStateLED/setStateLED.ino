int greenPin = 3;              // pin 4, CC LED
int bluePin = 5;               // pin 3, CC LED
int redPin = 6;                // pin 1, CC LED

void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 
  
}

void loop()
{

setStateLED(0);     // this is used for testing purposes to set the LED

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
