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

  digitalWrite(6, LOW);

  int value;

  value = getStateLED();

  Serial.println(value);
  delay(500);

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

