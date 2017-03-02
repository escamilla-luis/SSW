//Code utilizes arrays for pathing by taking advantage of the sizeof function.
//Using sizeof on an array with return an integer of 2 * # of integers inside
//the array. 
//There is a different array for every single path. Therefore, we know the sizeof each
//array. In addition we also know that arrays are indexed at 0.
//Everytime the smart sensor senses a magnet, we will subtract 2 from the sizeof of the
//array, while adding 1 to the index variable.
//Utilizing a while loop for sizeof > 0, this allows us to read each and every element 
//inside the array from left to right until the sizeof of the array falls below 0.
//-------------------------------------------------------------------------------------------

#include <Servo.h>
#define SERVO_PIN     3  // Pin for Servo
#define HALL_SENSOR1  2  // Smart Hall Effect Sensor
#define HALL_SENSOR2  4  // Dumb Hall Effect Sensor

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
int hall_2_state;
int x=0; // Initializing variable used for index of array at 0 (Arrays are indexed at 0)
int a=7; // CHANGE THIS TO CHANGE PATH GIVEN
int route;
int s;

//-------------------------------------------------------------------------------------------

void setup() 
{
  myservo.attach(SERVO_PIN);
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);
  Serial.begin(9600);
}

//-------------------------------------------------------------------------------------------

void loop() 
{
whichpath();    
}

//Functions path and index down here (Index function is nested inside of path function)-----
void path(int route, int s)
{
 hall_1_state = digitalRead(HALL_SENSOR1); // Assigning variable as reading of smart sensor
 hall_2_state = digitalRead(HALL_SENSOR2);
while (s > 0)// While size of array is larger than 0 [EQUIVALENT TO VOID LOOP() HERE]
 {
  
   hall_1_state = digitalRead(HALL_SENSOR1);
   hall_2_state = digitalRead(HALL_SENSOR2);

switch(hall_2_state)
    {
     case 0:
     myservo.write(80);
     Serial.print("Dumb \n");
     delay(500);
     break;
     case 1:
     //do nothing
     break;
    }
    
    int p=index(route,x); //CALLS FOR FUNCTION INDEX
     switch (hall_1_state) 
      {
         case 0: // If sensor senses a magnet
                switch (p)
                {
                  case 1:
                    Serial.print("Index position x is:");
                    Serial.print(x);
                    Serial.println();
                    Serial.println("Switch");
                    Serial.println();
                    myservo.write(30);
                    s=s-2;  // Sizeof = sizeof - 2
                    x=x+1;    //Index + 1
                    delay(500);
                  break;
                  
                  case 0:
                    Serial.print("Index position x is:");
                    Serial.print(x);
                    Serial.println();
                    Serial.println("Nope");
                    Serial.println();
                    myservo.write(180);
                    s=s-2; // Sizeof = sizeof - 2
                    x=x+1;   // Index + 1
                    delay(500);
                  break;
                }
                //s=s;
              
                break;  
         case 1: //If sensor does not sense a magnet
                 //do nothing
                 break;
       }
 } //EQUIVALENT TO END OF VOID LOOP()
 x=0; //Resets x after path is done
 Serial.print("We have reached the destination. Where would you like to go next?\n");
}
//------------------------------------------------------------------------------
int index(int route, int x)
{
  int yesorno;
  switch (route)
  {
    case 0:
      yesorno=path1to2[x];
    break;
    case 1:
      yesorno=path1to3[x];
    break;
    case 2:
      yesorno=path1to4[x];
    break;
    case 3:
      yesorno=path2to1[x];
    break;
    case 4:
      yesorno=path2to3[x];
    break;
    case 5:
      yesorno=path2to4[x];
    break;
    case 6:
      yesorno=path3to1[x];
    break;
    case 7:
      yesorno=path3to2[x];
    break;
    case 8:
      yesorno=path3to4[x];
    break;
    case 9:
      yesorno=path4to1[x];
    break;
    case 10:
      yesorno=path4to2[x];
    break;
    case 11:
      yesorno=path4to3[x];
    break;
  }
return yesorno;
}

void whichpath()
{
  switch(a)
    {  
    case 0: 
      Serial.print("The current path is 1 to 2\n");
      route=0;
      s=sizeof(path1to2);
      path(route,s);
    break;
    case 1:
      Serial.print("The current path is 1 to 3\n");
      route=1;
      s=sizeof(path1to3);
      path(route,s);
    break;
    case 2:
      Serial.print("The current path is 1 to 4\n");
      route=1;
      s=sizeof(path1to4);
      path(route,s);
    break;    
    case 3:
      Serial.print("The current path is 2 to 1\n");
      route=3;
      s=sizeof(path2to1);
      path(route,s);
    break;    
    case 4:
      Serial.print("The current path is 2 to 3\n");
      route=4;
      s=sizeof(path2to3);
      path(route,s);
    break;    
    case 5:
      Serial.print("The current path is 2 to 4\n");
      route=5;
      s=sizeof(path2to4);
      path(route,s);
    break;
    case 6:
      Serial.print("The current path is 3 to 1\n");
      route=6;
      s=sizeof(path3to1);
      path(route,s);
    break;
    case 7:
      Serial.print("The current path is 3 to 2\n");
      route=7;
      s=sizeof(path3to2);
      path(route,s);
    break;
    case 8:
      Serial.print("The current path is 3 to 4\n");
      route=8;
      s=sizeof(path3to4);
      path(route,s);
    break;
    case 9:
      Serial.print("The current path is 4 to 1\n");
      route=9;
      s=sizeof(path4to1);
      path(route,s);
    break;
    case 10:
      Serial.print("The current path is 4 to 2\n");
      route=10;
      s=sizeof(path4to2);
      path(route,s);
    break;
    case 11:
      Serial.print("The current path is 4 to 3\n");
      route=11;
      s=sizeof(path4to3);
      path(route,s);
    break;
    case 12:
    //do nothing
    break;
    }
}

