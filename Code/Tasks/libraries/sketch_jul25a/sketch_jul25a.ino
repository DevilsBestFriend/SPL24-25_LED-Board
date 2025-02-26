// Include the Servo library
#include <Servo.h>
// Define object 'myservo' to control the sensor
Servo myservo;

//The pin CONSTANTS for the line tracking sensors
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

// The pins CONSTANTS for other stuff
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

// A constant for our car speed
#define carSpeed 130

// Define variables to store the value of the sensor readings
int Echo = A4;  
int Trig = A5; 
// Define variables to hold our right, left, and middle distance values
int rightDistance = 0, leftDistance = 0, middleDistance = 0;

// Function to go forward
void forward(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("go forward!");
}

// Function to go backward
void back(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("go back!");
}

// Function to go left
void left(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("go left!");
}

// Function to go right
void right(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW); 
  Serial.println("go right!");
} 

// Function to stop
void stop(){
   digitalWrite(ENA, LOW);
   digitalWrite(ENB, LOW);
   Serial.println("Stop!");
} 

// Function to measure distance with the ultrasonic sensor
int Distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);  
  Fdistance= Fdistance / 58;       
  return (int)Fdistance;
}  

// Function to Initialize the Arduino
void setup(){
  // Attach the servo to pin 3
  myservo.attach(3);
  // Start the Serial Monitor
  Serial.begin(9600);     
  // Define the mode of our pins
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LT_R,INPUT);
  pinMode(LT_M,INPUT);
  pinMode(LT_L,INPUT);
  // Stop the car
  stop();
  // Put the sensor in the middle position 
  myservo.write(90);
}

void circulate(int leftTurn) {
    int sideDist = 0, midDist = 0;
    //Now try to circulate until we find next line
    // as long as there is no visible line, try again
    while (!(LT_M || LT_R || LT_L)) {
      myservo.write(leftTurn ? 0 : 180);
      delay(500);
      sideDist = Distance_test();
      myservo.write(90);
      delay(500);
      midDist = Distance_test();

      if (midDist > 20 && sideDist <= 20) {
        forward();
      } else if (sideDist > 20) {
        leftTurn ? right() : leftTurn();
      }
      //let it move a little
      //TODO: Adjust the delay to a reasonable value
      
      // by doing some experiments. Goal is to be able to go
      // about distance of 15 units before next scan
      delay(1000);

      //stop and get ready for next scan
      stop();
    }

    //At this point we should have found a line
    // Simply let the function return and
    // let loop() function handle the line tracking again
}

void loop() {
  // This is where the loop begins
  
  // LINE FOLLOWER
  // If we detect the middle sensor
  if(LT_M){
    // Then go forward
    forward();
  }
  // Else if we detect the RIGHT sensor
  else if(LT_R) {
    // Then go right
    right();
    // And keep going right until we don't detect anything on the right sensor
    while(LT_R);                             
  }
  // Else if we detect something on the left sensor
  else if(LT_L) {
    // Then go left
    left();
    // And keep going left until we don't detect anything on the left sensor
    while(LT_L);  

  }
  // OBJECT AVOIDANCE
  // Take a reading of what is in front of us  
  middleDistance = Distance_test();
  // Print the result in the Serial monitor
  Serial.print("Middle Distance = ");
  Serial.println(middleDistance);

  // If the distance to an obstacle is less than 20
  if(middleDistance <= 20) {
    int lastTurnLeft = 0;     
    // STOP!
    stop();
    // Wait half a second
    delay(500);
    // Now go in reverse                         
    back();
    // And set the servo to point right
    myservo.write(0);
    // Wait a second...
    delay(200);
    // ...and stop.
    stop();
    
    //  Read the distance to any obstacle that may be on the right      
    rightDistance = Distance_test();
    // Print the right distance on the serial monitor
    Serial.print("Right Distance = ");
    Serial.println(rightDistance);
    // Wait half a second
    delay(500);
    // Return the ultrasound sensor to the middle position
    myservo.write(90);
    // Wait another half a second to give the sensor a chance to catch up              
    delay(500);
    // And now set the servo to point to the left                                                  
    myservo.write(180);
    // Wait another half a second so it can catch up.              
    delay(500);
    // Take a reading to any obstacles that may be on the left
    leftDistance = Distance_test();
    // Print the left distance on the Serial monitor
    Serial.print("Left Distance = ");
    Serial.println(leftDistance);
    // Wait half a second
    delay(500);
    // Return the ultrasound sensor to the middle position
    myservo.write(90);
    // And wait a second              
    delay(1000);
    // Now, if the obstacle on the left is close to us
    if(rightDistance > leftDistance) {
      // Then let's go right
      right();
      delay(1000);
      stop();
      circulate(0);
    }
    // Else if the obstacle on the right is closer to us
    else if(rightDistance < leftDistance) {
      // Then let's go left
      left();
      delay(1000);
      stop();
      circulate(1);
    }
    // Break out of the object avoidance code
  }
// Go back to the start of the loop                     
}
