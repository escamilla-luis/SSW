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

status_LED(3);


}

void status_LED(int status)
{
  if (status == 0)                  // 0 status is for stopped (red solid)
  {
    analogWrite(redPin, 255);
  }

  if (status == 1)                  // 1 status is for healthy (green solid)
  {
    analogWrite(greenPin, 255);
  }

  if (status == 2)                  // 2 status is for error (red flashing)
  {
    for (int i=0; i <= 255; i++)
    {
      analogWrite(redPin, 255);
      delay(500);
      analogWrite(redPin, 0);
      delay(500);
    }
  }

  if (status == 3)                  // 3 status is for checkpoint reached (blue solid)
  {
    analogWrite(bluePin, 255);
  }


}

