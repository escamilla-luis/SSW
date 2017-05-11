#define POS_SENSOR1 2
#define POS_SENSOR2 3

const int peakgap = 7;
int LastRFID;
int Peak_Num;
float Position;


void setup()
{
  pinMode(POS_SENSOR1, INPUT);
  pinMode(POS_SENSOR2, INPUT);
  Serial.begin(9600);
}

void loop()
{
  getPosition;
}

void getPosition()
{
  if(digitalRead(POS_SENSOR1) == LOW)
  {
    if(digitalRead(POS_SENSOR1) == HIGH)
      Peak_Num++;
  }
  
if(digitalRead(POS_SENSOR2) == LOW)
  {
    if(digitalRead(POS_SENSOR2) == HIGH)
      Peak_Num++;
  }
  getTicks;
  getLastCheckPoint;
  Position  = Peak_Num * peakgap/10.0;
  Serial.print("The vehicle is ");
  Serial.print(Position);
  Serial.print(" cm away from Station ");
  Serial.print(StationId);
}
int getTicks() {
  numberOfTicksFromLastCheckpoint = Peak_Num;
    return numberOfTicksFromLastCheckpoint;
  }
int getLastCheckpoint() {
  LastCheckpoint = LastRFID;
    return LastCheckpoint;
  }
