// Guys, you guys can see how to process this code using serial monitor

#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin
#define STOP 13 // emergency stop

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance
int stop_counter = 0;
int restar_counter = 0;
void setup() {
 Serial.begin (57600);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(STOP, OUTPUT);

}

void loop() {
/* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;
 
 if (distance >= maximumRange || distance <= minimumRange){
 /* Send a negative number to computer and Turn LED ON 
 to indicate "out of range" */
 Serial.println("-1");

 }
 else {
 /* Send the distance to the computer using Serial protocol, and
 turn LED OFF to indicate successful reading. */
 Serial.println(distance);
 
 }
 if (distance < 30) {
  stop_counter++;
 }
 else{
  stop_counter = 0;
 
}
   Serial.println(stop_counter);
   if ( stop_counter == 10)
   {
    restar_counter = 0;
     digitalWrite(STOP, HIGH);
    Serial.println("too close");
    stop_counter = 0;
    delay(1000);
    digitalWrite(STOP, LOW);
    }


   
 //Delay 50ms before next reading.
 delay(50);
}
