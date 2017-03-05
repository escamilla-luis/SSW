//////////////////////////////////////////////   Whale.. Whale.. Whale...
// Spartan Suerway Vehicle Code:            //
//  Arduino code for vehicles 2015-2016     //          ":"
//                                          //        ___:____     |"\/"|
//  Please follow this syntax for nameing   //      ,'        `.    \  /
//    Syntax: variable_name;                //      |  O        \___/  |
//            functionNames();              //   ~^~^~^~^~^~^~^~^~^~^~^~^~
//////////////////////////////////////////////    What do we have here???


#include <TimerOne.h>
#include <ServoTimer2.h>

// Define vehicle id number
#define vehicle_number  1

// Pin Definitions
#define barcode_sensor_1    2   // Interrupt Pin
#define barcode_sensor_2    3   // Interrupt Pin
#define motor_enco_1a       A2  // Interrupt pin using custom interrupt setup
#define motor_enco_1b       A3  //  (Motor 2 encoder on pins A4, A5 - not used)
#define motor1_dir          A0  //
#define motor2_dir          A1  //
#define motor1              9   // PWM pin
#define motor2              10  // PWM pin
#define servo_pin           11  // PWM Pin
#define trigger             12  //
#define echo                13  //


//System Wide Global Variables
int state = 0;  // 0 = Offline, 1 = Online/Idle, 2 = Online/Active
unsigned int track_location = 0;
unsigned int instruction[25][2];
int instruction_step = 0;

// Control System Variables
int sense_rpm = 0, pwm_out = 0, rpm_err;
int acc_err, enc_count, set_rpm;
const float v_Kp = 0.01, v_Ki = 0.1;
boolean STOP = true;

boolean enc_prestate;
boolean servo_switch;
int switch_left, switch_right;

int head_distance = 60;
int last_hDistance;
const int dist_limit = 3;
const int dist_thresh = 16;


// Speed Controller Variables
unsigned long last_update, ping;
long period = 1000 / 20;
int control_rpm;


// Communication Variables
String input_string = "";         // String to hold incoming data
boolean string_complete = false;  // Flag for checking if string is complete
boolean string_enable = false;    // Flag for stream in

byte *_track    = new byte[2];  // Temporary char arrays for
byte *_distance = new byte[2];  // parsing chars into 16 bit
byte *_speed    = new byte[2];  // data types
byte *_m_count  = new byte[2];


//Barcode Scanner Variables
const int max_bit_count = 16;
unsigned int thresh_min = 1;
unsigned int thresh_max = 200;

unsigned int bit_count1, bit_count2;
unsigned long last_tick1, last_tick2;
unsigned int times1[max_bit_count];
unsigned int times2[max_bit_count];


//Hardware Functions
void barcode(); //Barcode change interrupt
void encoder(); //Encoder rising interrupt
ServoTimer2 servo;

void setup() {
    // Disable interrupts
    noInterrupts();
    
    // Set fast pwm pins
    PCICR |= 0x01;  PCMSK1 |= 0x08;
    
    // Timer Counter Control Register for Timer0
    //  Set clock mode for timer one
    //TCCR0A = 0xa1;  TCCR0B = 0x01;
    
    // Set clock speed
    //  - A: First timer limit
    //  - B: PWM Duty cycle
    //    Used for motor speed pwm control
    //OCR0A = 0;      OCR0B = 0;      // overflow 1, 0% duty cycle
    
    
    // Component Pinmodes
    pinMode(barcode_sensor_1, INPUT);   // Left Barcode Scanner
    pinMode(barcode_sensor_2, INPUT);   // Right Barcode Scanner
    pinMode(servo_pin, OUTPUT);         // Servo control
    pinMode(motor_enco_1a, INPUT);      // Encoder interrupt
    pinMode(motor_enco_1b, INPUT);      // Encoder interrupt
    pinMode(motor1, OUTPUT);            // motor 1 pwm
    pinMode(motor2, OUTPUT);            // motor 2 pwm
    pinMode(motor1_dir, OUTPUT);        // motor 1 direction
    pinMode(motor2_dir, OUTPUT);        // motor 2 direction
    pinMode(trigger, OUTPUT);           // Ultrasonic trigger
    pinMode(echo, INPUT);               // Ultrasonic echo
    
    digitalWrite(motor1_dir, HIGH);     // Set initial motor1 direction
    digitalWrite(motor2_dir, LOW);      // Set initial motor2 diection
    digitalWrite(motor_enco_1a, LOW);   // Set interrupt condition
    digitalWrite(motor_enco_1b, LOW);   //   or disable pullup resistor
    
    servo.attach(servo_pin);

    if(vehicle_number == 5){
        switch_left = 800; switch_right = 2100;
    } else {
        switch_left = 2100; switch_right = 800;
    }
    
    
    // Set TimerOne to signal at 20Hz frequency (20 times/sec)
    Timer1.initialize(1000000 / 1000);
    
    
    // Hardware Interrupts
    attachInterrupt(digitalPinToInterrupt(barcode_sensor_1), barcode1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(barcode_sensor_2), barcode2, CHANGE);
    
    // Set up External Interrupts on Analog pins
    PCMSK1 |= 0b00111100;
    PCICR  |= 0b00000010;
    
    // Serial Communication
    Serial.begin(57600);
    
    
    // Set Initial Variables
    state = 1;
    enc_count = 0;
    set_rpm = 0;
    
    // Start timer1 PWM for Motors
    Timer1.pwm(motor1, 0);
    Timer1.pwm(motor2, 0);

    // Send online signal
    byte checksum = 0x00;
    
    checksum = 0x00 + (byte)vehicle_number + 0x30 + (byte)state;
    
    Serial.print((char)0x81);             //Message Start
    Serial.print((char)0x00);             //  0 Receiver
    Serial.print((char)vehicle_number);   //  1 Sender
    Serial.print((char)0x30);             //  2 Type
    Serial.print((char)state);            //  3 Status
    Serial.print((char)(0 - checksum));   //  4 Checksum
    Serial.println((char)0x7E);           //Message End
    
    
    // Reenable interrupts
    interrupts();
}

void loop() {
    // Run speed controller
    if (millis() - last_update >= period)
        controlsUpdate();

    if (millis() - ping >= 2500){
        heartbeat();
    }
}

void controlsUpdate(){
    // Collision Avoidance
    head_distance = ultrasonic(); //Update distance ahead of vehicle
    
    if (head_distance > dist_thresh)
        control_rpm = set_rpm;
    else if (head_distance <= dist_thresh && head_distance >= dist_limit)
        control_rpm = set_rpm * (head_distance * head_distance) / (dist_thresh * dist_thresh);
    else if (head_distance < dist_limit){
        control_rpm = 0;
        rpm_err = 0; acc_err = 0;
    }
 
    
    // PI Speed Controller
    //count1 *= 5.27; //20(20hz) * 60(min) / 3(pole) / 75.81 ~94ticks/cycle or 1880/sec 4.133rev/s at wheels at max pwm
    sense_rpm = (float)( enc_count / 6 ) * ( 1000.0 / (millis() - last_update));
    
    rpm_err = control_rpm - sense_rpm;
    acc_err += rpm_err;
    pwm_out = (v_Kp * rpm_err) + (v_Ki * acc_err);
    
    // PWM range limiter
    if (pwm_out >= 255)     { pwm_out = 255;    }
    else if (pwm_out <= 0)  { pwm_out = 0;      }
    
    // Update Motor power outputs
    if (STOP) {
        rpm_err = 0; acc_err = 0;
        Timer1.setPwmDuty(motor1, 0);
        Timer1.setPwmDuty(motor2, 0);
    }
    else if ( pwm_out > 4 && pwm_out <= 200) {
        Timer1.setPwmDuty(motor1,  pwm_out    * 4);
        Timer1.setPwmDuty(motor2, (pwm_out - 4) * 4);
    }
    else {
        Timer1.setPwmDuty(motor1, pwm_out * 4);
        Timer1.setPwmDuty(motor2, pwm_out * 4);
    }
    
    // Servo control using clock signal
    // Values = 800, 2100, program it so the second makes the left arm go up when it boots up
    if (servo_switch)
        servo.write(switch_right);
    else
        servo.write(switch_left);

    //Serial.println((String)"cRPM " + control_rpm + " sRPM " + sense_rpm + " PWM_out " + pwm_out + " eRPM " + rpm_err + " eAcc " + acc_err + " hDist " + head_distance);
    
    enc_count = 0;
    last_update = millis();
}

void serialEvent() {
    byte checksum = 0x00;
    
    while (Serial.available())
    {
        // Get new byte:
        char char_in = (char)Serial.read();
        
        // End message
        if (char_in == (char)0x7E && string_enable == true) {
            string_complete = true;
            string_enable = false;
        
            // Calculate checksum
            if (checksum == 0)
                processData();
            else{
                byte checksum = 0x00;
                
                checksum = 0x00 + (byte)vehicle_number + 0xF0 + (byte)state;
                
                Serial.print((char)0x81);             //Message Start
                Serial.print((char)0x00);             //  0 Receiver
                Serial.print((char)vehicle_number);   //  1 Sender
                Serial.print((char)0x08);             //  2 Type
                Serial.print((char)state);            //  3 Status
                Serial.print((char)(0 - checksum));   //  8 Checksum
                Serial.println((char)0x7E);           //Message End
            }
        }
    
        // Record incomming message
        if (string_enable == true) {
            input_string += char_in;
            checksum += (byte)char_in;
        }
    
        // Start Message
        if (char_in == (char)0x81 && string_enable == false) {
            string_enable = true;
            input_string = "";
        }
    }
}

// Serial output function
void sendMessage(String message) {
    byte checksum = 0x00;
    
    // Write out message start character
    Serial.print((char)0x81);
    
    // Write message
    for (int ltr = 0; ltr < message.length(); ltr++) {
        Serial.print(message.charAt(ltr));
        checksum += (byte)message[ltr];
    }
    
    // Write out checksum and message end character
    Serial.print((char)0 - checksum);
    Serial.println((char)0x7E);
}

void processData() {
    char *temp = new char[2];
    
    if ((int)input_string[0] == vehicle_number || input_string[0] == 0xF0) {
        if (input_string[2] == (char)0x1F) {      //Set all
            temp[0] = input_string[4];
            temp[1] = input_string[3];
            memcpy(&track_location, temp, sizeof(int));
            temp[0] = input_string[8];
            temp[1] = input_string[7];
            memcpy(&set_rpm, temp, sizeof(int));
            
            
        } else if (input_string[2] == (char)0x05) { // Send Current State
            sendState();
        
        } else if (input_string[2] == (char)0x01) { // Go
            temp[0] = input_string[4];
            temp[1] = input_string[5];
            memcpy(&set_rpm, temp, sizeof(int));
            STOP = false;
        
        } else if (input_string[2] == (char)0x02) { // Emergency Stop
            set_rpm = 0;
            STOP = true;
        
        } else if (input_string[2] == (char)0x07) { // Switch Servo
            servo_switch = !servo_switch;
        
        } else if (input_string[2] == (char)0x10) { // Receive Instructions
            int message_pos = 4;
            int instruct_step = 0;
            
            while (message_pos < input_string.length()-1) {
                instruction[instruct_step][0] = (input_string[message_pos] << 8) | (input_string[message_pos+1] & 0xFF);
                instruction[instruct_step][1] = input_string[message_pos + 2];
                
                message_pos += 3;
                instruct_step++;
            }
            instruction[instruct_step][0] = 0xFF;

            // Signal message received
            byte checksum = 0x00;
            checksum = 0x00 + (byte)vehicle_number + 0x20 + (byte)state;
            
            Serial.print((char)0x81);             //Message Start
            Serial.print((char)0x00);             //  0 Receiver
            Serial.print((char)vehicle_number);   //  1 Sender
            Serial.print((char)0x20);             //  2 Type
            Serial.print((char)state);            //  3 Status
            Serial.print((char)(0 - checksum));   //  8 Checksum
            Serial.println((char)0x7E);           //Message End

            //for(int i = 0; instruction[i][0] != 0xFF; i++)
                //Serial.println((String)"Track: " + instruction[i][0] + " instruct: " + instruction[i][1]);

            state = 2;
        }
    }
}

void sendState() {
    byte checksum = 0x00;
    memcpy(_track, &track_location, sizeof(int));
    memcpy(_speed, &sense_rpm, sizeof(int));

    checksum = 0x00 + (byte)vehicle_number + 0x05 + (byte)state + (byte)_track[1] + (byte)_track[0] + (byte)_speed[1] + (byte)_speed[0];
    
    Serial.print((char)0x81);             //Message Start
    Serial.print((char)0x00);             //  0 Receiver
    Serial.print((char)vehicle_number);   //  1 Sender
    Serial.print((char)0x05);             //  2 Type
    Serial.print((char)state);            //  3 Status
    Serial.print((char)_track[1]);        //  4 Track
    Serial.print((char)_track[0]);        //  5
    Serial.print((char)_speed[1]);        //  6 State
    Serial.print((char)_speed[0]);        //  7
    Serial.print((char)(0 - checksum));   //  8 Checksum
    Serial.println((char)0x7E);           //Message End
}

void heartbeat() {
    byte checksum = 0x00;
    
    checksum = 0x00 + (byte)vehicle_number + 0x08 + (byte)state;
    
    Serial.print((char)0x81);             //Message Start
    Serial.print((char)0x00);             //  0 Receiver
    Serial.print((char)vehicle_number);   //  1 Sender
    Serial.print((char)0x08);             //  2 Type
    Serial.print((char)state);            //  3 Status
    Serial.print((char)(0 - checksum));   //  8 Checksum
    Serial.println((char)0x7E);           //Message End

    ping = millis();
}

void calculateBarcode(unsigned int times[]) {
    int time_short = times[0], time_long = times[0];
    int u_time = 0;
    int barcode_val = 0;
    
    // Find the minima and maxima in recorded number range
    for (int idx = 1; idx < max_bit_count; idx++) {
        if (times[idx] < time_short && times[idx] != 0)
            time_short = times[idx];
            
        if (times[idx] > time_long)
            time_long = times[idx];
    }

    // Calculate the time of a short bar
    u_time = (time_short + time_long) / 3;
    
    // Convert recoreded times into an integer
    // by using bit math.
    for (int idx = 0; idx < max_bit_count; idx++) {
        if (times[idx] > u_time * 1.3){
            barcode_val |= (1 << idx);
        }else{
            barcode_val |= (0 << idx);
        }
    }
    
    // Clear variables
    memset(times, 0, sizeof(int) * max_bit_count);

    // Barcode Noise Filter (1024 used since we don't have barcodes that large)
    if(barcode_val > 0 && barcode_val < 1024){
        track_location = barcode_val;
        
        // Send current state information
        sendState();
        
        // Execute instruction action
        instructionUpdate();
    }
}


void instructionUpdate(){
    // Switching Signal
    if(track_location == instruction[instruction_step][0] && state == 2) {
        
        switch(instruction[instruction_step][1]){
            case 0:
                set_rpm = 0;
                STOP = true;
                break;
            case 1:
                servo_switch = 0;
                break;
            case 2:
                servo_switch = 1;
                break;
        }

        instruction_step++;

        if(instruction[instruction_step][0] == 0xFF){
            state = 1;      sendState();    // Change state of vehicle
            if(digitalRead(instruction[instruction_step][1]+2) == 1)
                set_rpm = 0;    STOP = true;    // Stop vehicle
        }
    }
}

    
//----------------Sensor Functions----------------\\
void barcode1() {
    unsigned long current_time = millis();
    unsigned long d_time = current_time - last_tick1;

    if(bit_count1 < max_bit_count && d_time > thresh_min){
        if (digitalRead(barcode_sensor_1) == 1)
            times1[bit_count1] = d_time;
        else
            times1[bit_count1] = (1.9 * d_time);
        
        last_tick1 = current_time;
        bit_count1++;
    }

    if(d_time > thresh_max)
        bit_count1 = 0;

    if(bit_count1 == max_bit_count){
        calculateBarcode(times1);
        bit_count1 = 0;
    }
}

void barcode2() {
    unsigned long current_time = millis();
    unsigned long d_time = current_time - last_tick2;

    if(bit_count2 < max_bit_count && d_time > thresh_min){
        if (digitalRead(barcode_sensor_2) == 1)
            times2[bit_count2] = d_time;
        else
            times2[bit_count2] = (1.9 * d_time);
        
        last_tick2 = current_time;
        bit_count2++;
    }

    if(d_time > thresh_max)
        bit_count2 = 0;

    if(bit_count2 == max_bit_count){
        calculateBarcode(times2);
        bit_count2 = 0;
    }
}

ISR(PCINT1_vect) {
    if (digitalRead(motor_enco_1a) == HIGH && enc_prestate)
        if (digitalRead(motor_enco_1b) == HIGH)
            enc_count++;
        else
            enc_count--;
    
    if (digitalRead(motor_enco_1a) == LOW)
        enc_prestate = true;
    else
        enc_prestate = false;
}

int ultrasonic() {
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    
    int duration = pulseIn(echo, HIGH, 3000);

    if(duration > 0)
        return float(duration / 58.2);
    else
        return 30;
}
