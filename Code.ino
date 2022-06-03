#include <Servo.h> // Includes servo library
#include <dht.h> // include dht library

float temperature;

//====for ultrasonic sensor:
const int trigPin = 9;   // HC-SR04 trigger pin is connected to digital 9
const int echoPin = 8;   // HC-SR04 echo pin is connected to digital 8
//====for flame sensor
const int flamePin = 3;  // flame sensor signal pin is connected to digital 3
int Flame = HIGH;        // flame sensor is initialized with a boolean HIGH
//====for DHT11
#define dhtPin A0     // Analog Pin A0 of Arduino is connected to DHT11 out pin
dht DHT;
//====for Mo26 sound sense
const int pulsePin = A5;                 // Signal pin of MO26 is connected to A5
int highPulse;                           // Pulse High Time in microsecond
int lowPulse;                            // Pulse Low time in microsecond
float totalPulse;                        // Total of high & low times
float frequency;                         // Calculated Frequency

//l293D pins to arduino digital
const int leftIn_1 = 7; // (pin 7 of l293D motor driver)
const int leftIn_2 = 6; // (pin 2 of l293D motor driver)
const int rightIn_1 = 5; // (pin 10 of l293D motor driver)
const int rightIn_2 = 4; // (pin 15 of l293D motor driver)

const int servoPin = 10;  // orange signal pin of servo to digital 10
//--------------------------------------------------------------
Servo myservo;
// defines variables
long duration;
int distance;
int distanceFront;
int distanceRev; 
int distance_1;
int leastDistance = 70; // This is the threshold least distance to initiate stop and turn

void setup() {
//HC-SR04
  pinMode (trigPin, OUTPUT); // trigger Pin is set as output to send ultrasonic waves
  pinMode (echoPin, INPUT); // echo Pin is set as input to receive ultrasonic waves
//flame sensor
  pinMode(flamePin, INPUT); // flame signal pin is set to read inputs
//DHT11
  delay(500); // delay for sensor reading boot-up
  Serial.println("DHT11 (Humidity and temperature) Sensor\n");
  delay(1000); // delay for interfacing with the sensor
//MO26
  pinMode(pulsePin,INPUT); // MO26 signal pin is set to read inputs
//set l293D pins for OUTPUT 
  pinMode(leftIn_1, OUTPUT); // pin 7 of l293D motor driver is set to act as output
  pinMode(leftIn_2, OUTPUT); // pin 2 of l293D motor driver is set to act as output
  pinMode(rightIn_1, OUTPUT); // pin 10 of l293D motor driver is set to act as output
  pinMode(rightIn_2, OUTPUT); // pin 15 of l293D motor driver is set to act as output
  
  myservo.attach(servoPin); 
  myservo.write(90); //initiate the angle of rotation for servo wing at 90deg
  Serial.begin(9600); // starts serial communication at 9600 baud rate for arduino
}

void loop() {
  //defines sweep rotation according to least distance detected by ultrasonic
  distanceFront = ping (); //ping sends and receives a pulse by the ultrasonic sensor
  if (distanceFront > leastDistance) {
    front(); //leads to block controlling forward movement
    delay(300); 
  }
  //if the distance on the front is less than least distance, stop
  else {
    Break(); //leads to block controlling stop sequence
    getDistance(); 
    //after stop and servo sweep, if distance to the right is greater than least distance, turn right
    if (distanceRight > leastDistance) {
      right(); //leads to block controlling right side turn movement
      delay(400); 
      front(); 
    }
    //after stop and servo sweep, if distance to the left is greater than least distance, turn left
    else if(distanceLeft > leastDistance) {
      left(); //leads to block controlling left side turn movement
      delay (1000); 
      front();
    }
    //after stop and servo sweep, if distance on both sides are less than least distance, move back
    else {
      back(); //leads to block controlling backward movement
      delay(900);
      Break();
    }
  }
  //----------------flame sensor------------------
  Flame = digitalRead(flamePin);
  //when light is incident on the flame sensor, it produces a signal low
  if (Flame== LOW) //since flame was initialized as boolean HIGH, a low signal implies presence of flame
  {
    Serial.println("Flame detected"); //print flame detected if LOW
    Serial.println(Flame);
    delay (2000);
  }
  else //
  {
    Serial.println("No Flame"); //print flame not detected if signal remains HIGH
    Serial.println(Flame);
    delay (2000);
  }
  //--------------------DHT11---------------------
  DHT.read11(dhtPin);
    temperature = DHT.temperature; //using DHT library function for temperature
    Serial.print("Humidity = ");
    Serial.print(DHT.humidity); //using DHT library function for humidity
    Serial.print("% \t");
    Serial.print("Temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println(" C");
    //Serial.println(temperature);
    delay(3000); 
  
  //--------------------frequency by MO26---------------------
    highPulse = pulseIn(pulsePin,HIGH);   //read the high pulse
    lowPulse = pulseIn(pulsePin,LOW);     //read the low pulse
    
    totalPulse = highPulse + lowPulse;    // add the pulses
    frequency = 1000000/totalPulse;         // convert to Hz

    Serial.print ("Frequency: ");
    Serial.print ((unsigned long) frequency);
    Serial.println (" Hz");
    delay(1000);  
 }

// define ping()
int ping() {
  //initiating the ultrasonic sensor and sending a test ping for starting the HC-SR04
  digitalWrite(trigPin, LOW); // clears the trigger Pin
  delayMicroseconds(2); //delay by 2 microseconds
  digitalWrite(trigPin, HIGH); // sends a test pulse
  delayMicroseconds(10); // send a pulse for 10 microseconds
  digitalWrite(trigPin, LOW); // turns off again
  duration = pulseIn(echoPin, HIGH); // Convert the time into a distance by reading the echo pin for the 10ms pulse sent
  distance = (duration/2) / 29.1; // Divide by 29.1 or multiply by 0.0343 for distance in centimetres
  Serial.print("Distance of object= ");
  Serial.print(distance);
  Serial.print("cm"); //prints the distance in cm
  Serial.println("");
}

//code block to display detected distances in serial monitor
void displayDistance() { 
  // display detected distances in serial monitor
  Serial.print("Right Distance: ");
  Serial.print (distanceRight);
  Serial.println(""); 
  Serial.print("Front Distance: "); 
  Serial.print (distanceFront) ;
  Serial.println(""); 
  Serial.print("Left Distance: ");
  Serial.print (distanceLeft);
  Serial.println("");
}

//code block for forward movement
void front() {
  Serial.println("Move Forward");
  digitalWrite(leftIn_1, LOW); //make pin 7 of driver LOW
  digitalWrite(leftIn_2, HIGH); //make pin 2 of driver HIGH
  digitalWrite(rightIn_1, LOW); //make pin 10 LOW
  digitalWrite(rightIn_2, HIGH); //make pin 15 HIGH
}
//code block for reverse movement
void back() {
  Serial.println("Move Back");
  digitalWrite(leftIn_1, HIGH); //make pin 7 HIGH
  digitalWrite(leftIn_2, LOW); //make pin 2 LOW
  digitalWrite(rightIn_1, HIGH); //make pin 10 HIGH
  digitalWrite(rightIn_2, LOW); //make pin 15 LOW
}
//code block for left side turn movement
void left() {
  digitalWrite(leftIn_1, HIGH); //make pin 7 HIGH
  digitalWrite(leftIn_2, LOW); //make pin 2 LOW
  digitalWrite(rightIn_1, LOW); //make pin 10 LOW
  digitalWrite(rightIn_2, HIGH); //make pin 15 HIGH
}
//code block for right side turn movement
void right() {
  digitalWrite(leftIn_1, LOW); //make pin 7 LOW
  digitalWrite(leftIn_2, HIGH); //make pin 2 HIGH
  digitalWrite(rightIn_1, HIGH); //make pin 10 HIGH
  digitalWrite(rightIn_2, LOW); //make pin 15 LOW
}
//code block for stop sequence
void Break() {
  digitalWrite(leftIn_2, LOW); //make pin 7 LOW
  digitalWrite(leftIn_1, LOW); //make pin 2 LOW
  digitalWrite(rightIn_1, LOW); //make pin 10 LOW
  digitalWrite(rightIn_2, LOW); //make pin 15 LOW
}
//respective distance check and servo head turn
void getDistance() {
  myservo.write(0); //back to 0deg from initial 90deg. This initiates right side sweep
  delay(500);
  int tempRight_l = ping(); //a temporary ping is received and stored from right sweep
  myservo.write(45); //sweep 45deg (right sweep)
  delay(500);
  int tempRight_2 = ping(); //another temporary ping is received and stored from right sweep
  //if the first ping is less than second, it is set as the distanceRight distance for forward decision at the top
  if(tempRight_l < tempRight_2) {
    distanceRight = tempRight_l;
  }
  //if the second ping is less than first, it is set as the distanceRight distance for forward decision at the top
  else {
    distanceRight = tempRight_2;
  }
  myservo.write(90); //rotate back to central 90deg position
  delay(500);
  distanceFront = ping(); //send and read ping for forward distance
  
  myservo.write(135); //sweep 135deg (left sweep)
  delay(500);
  int tempLeft_1 = ping(); //a temporary ping is received and stored from left sweep
  myservo.write(180); //sweep 180deg to complete the left sweep
  delay(500);
  int tempLeft_2 = ping(); //another temporary ping is received and stored from left sweep
  //if the first ping is less than second, it is set as the distanceLeft distance for forward decision at the top
  if (tempLeft_1 < tempLeft_2) {
    distanceLeft = tempLeft_1;
  }
  //if the second ping is less than first, it is set as the distanceLeft distance for forward decision at the top
  else{
    distanceLeft = tempLeft_2;
  }
  myservo.write(90); //servo rotation back to original 90deg position
}
