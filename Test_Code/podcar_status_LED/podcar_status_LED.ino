void setup()
{

int redPin = 3;
int greenPin = 6;
int bluePin = 9;

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 
  
}

void loop()
{
//
//analogWrite(3, 255);  //turn red on
//delay(1000);
//analogWrite(3, 0);
//
//analogWrite(6, 125);  //turn green on
//delay(1000);
//analogWrite(6, 0);
//
//analogWrite(9, 125);  //turn blue on
//delay(1000);
//analogWrite(9, 0);

status_LED(3);

}

void status_LED(int status)
{
  if (status == 0)
  {
    analogWrite(3, 255);
  }

  if (status == 1)
  {
    analogWrite(6, 255);
  }

  if (status == 2)
  {
    for (int i=0; i <= 255; i++)
    {
      analogWrite(3, 255);
      delay(500);
      analogWrite(3, 0);
      delay(500);
    }
  }

  if (status == 3)


}

