//----------------------------------GLOBAL VARIABLES----------------------------------------

#include <StackArray.h>
#include <Servo.h>
#define SERVO_PIN     4  // Pin for Servo
#define HALL_SENSOR1  26  // Smart Hall Effect Sensor
#define HALL_SENSOR2  27  // Dumb Hall Effect Sensor
#define SEND_PATH1 6
#define SEND_PATH2 7

StackArray <int> scheduleArray;
//int scheduleArray[] = {};

Servo myservo;  // Initializing the servo

int path1to2[]={0, 0}; //0 for a 
int path1to3[]={1, 0}; //1 for a
int path1to4[]={1, 0}; //2 for a

int path2to1[]={0}; //3 for a
int path2to3[]={1, 1, 1, 0}; //4 for a
int path2to4[]={1, 1, 0}; //5 for a

int path3to1[]={1, 1, 0}; //6 for a
int path3to2[]={1, 1, 1, 0, 0}; //7 for a
int path3to4[]={0}; //8 for a

int path4to1[]={1, 0}; //9 for a
int path4to2[]={1, 1, 0, 0}; //10 for a
int path4to3[]={0, 0}; //11 for a

int hall_1_state;// Initializing variable used for reading of the smart sensor
int hall_2_state;// Initializing variable used for reading of the dumb sensor

int a;
int b;

//-------------------------------------VOID SETUP-------------------------------------------

void setup() 
{
  myservo.attach(SERVO_PIN);
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);
  pinMode(SEND_PATH1,INPUT);
  pinMode(SEND_PATH2,INPUT);
  digitalWrite(SEND_PATH1, HIGH);
  digitalWrite(SEND_PATH2, HIGH);
  Serial.begin(9600);
}

//---------------------------------------VOID LOOP------------------------------------------------------------------

void loop() 
{
  hall_1_state = digitalRead(HALL_SENSOR1);
  hall_2_state = digitalRead(HALL_SENSOR2);

  Signal2Path2Stack(); //Nested Functions that takes thesignal, determines the path, then generates the stack
  
  IfSmartSensorActivated();
  IfDumbSensorActivated();
  
}

//-------------------------Signal2Path2Stack--------------------------------------------------------------------------
void Signal2Path2Stack() //button1 and button2 represent different signals sent
{
  int button1 = digitalRead(SEND_PATH1);
  int button2 = digitalRead(SEND_PATH2);
  
  if(button1==0 & scheduleArray.isEmpty()) //If a path is sent and the array is empty 
  {
  a=4;
  WhichPath(a); 
  delay(500);
  GenerateStack(b);
  }

  if(button2==0 & scheduleArray.isEmpty()) //If a path is sent and the array is empty 
  {
  a=7;
  WhichPath(a); 
  delay(500);
  GenerateStack(b);
  }
}  

//---------------------------WhichPath------------------------------
int WhichPath(int a)
{
  switch(a)
    {  
    case 0: 
      Serial.print("The current path is 1 to 2\n");
      a=0;
    break;
    case 1:
      Serial.print("The current path is 1 to 3\n");
      a=1;
    break;
    case 2:
      Serial.print("The current path is 1 to 4\n");
      a=2;
    break;    
    case 3:
      Serial.print("The current path is 2 to 1\n");
      a=3;
    break;    
    case 4:
      Serial.print("The current path is 2 to 3\n");
      a=4;
    break;    
    case 5:
      Serial.print("The current path is 2 to 4\n");
      a=5;
    break;
    case 6:
      Serial.print("The current path is 3 to 1\n");
      a=6;
    break;
    case 7:
      Serial.print("The current path is 3 to 2\n");
      a=7;
    break;
    case 8:
      Serial.print("The current path is 3 to 4\n");
      a=8;
    break;
    case 9:
      Serial.print("The current path is 4 to 1\n");
      a=9;
    break;
    case 10:
      Serial.print("The current path is 4 to 2\n");
      a=10;
    break;
    case 11:
      Serial.print("The current path is 4 to 3\n");
      a=11;
    break;
    }
b=a;
return b;
}


//------------------------GenerateStack-------------------------

void GenerateStack(int b)
{
  Serial.println("The Stack Array is:");
  switch(b)
    {
     case 0:
       for(int i = (sizeof(path1to2)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path1to2[i]);
          Serial.println(scheduleArray.peek());
        }
    break;
    case 1:
        for(int i = (sizeof(path1to3)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path1to3[i]);
          Serial.println(scheduleArray.peek());
        }
     break;
     case 2:
        for(int i = (sizeof(path1to4)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path1to4[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 3:
        for(int i = (sizeof(path2to1)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path2to1[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 4:
        for(int i = (sizeof(path2to3)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path2to3[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 5:
        for(int i = (sizeof(path2to4)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path2to4[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 6:
        for(int i = (sizeof(path3to1)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path3to1[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 7:
        for(int i = (sizeof(path3to2)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path3to2[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 8:
        for(int i = (sizeof(path3to4)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path3to4[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 9:
        for(int i = (sizeof(path4to1)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path4to1[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 10:
        for(int i = (sizeof(path4to2)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path4to2[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      case 11:
        for(int i = (sizeof(path4to3)/2)-1; i > -1 ; i--) 
        {
          scheduleArray.push(path4to3[i]);
          Serial.println(scheduleArray.peek());
        }
      break;
      }
Serial.println("---------");
}

//----------------------------IfSmartSensorActivated------------------------------
void IfSmartSensorActivated()
{
    if (hall_1_state==0 & !scheduleArray.isEmpty()) //If smart sensor detects a magnet and stack is not empty
    {
    delay(500);
    
    int laststackvalue = scheduleArray.pop(); // Pop and assign last element of stack to laststackvalue
    Serial.println(laststackvalue);
 
    switch(laststackvalue) //Based on the value of the last element, switch or not don't switch
      {
      case 1: 
      myservo.write(60);
      Serial.println("Switched");
      break;

      case 0:
      myservo.write(180);
      Serial.println("Not Switched");
      break;
      }
    
    }
}

//-------------------------------IfDumbSensorActivated------------------------
void IfDumbSensorActivated()
{
  if (hall_2_state==0) //If dumb sensor detects a magnet
  {
    myservo.write(160);
    Serial.print("Dumb\n");
    delay(500);
  }
}
  
