#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Stepper.h>
#include <stdio.h>
MPU6050 mpu;

const int ENA_PIN_LEFT = 2;
const int LmotorPin1 = 3;
const int LmotorPin2 = 4;

const int ENA_PIN_RIGHT = 5;
const int RmotorPin1 = 6;
const int RmotorPin2 = 7;

RF24 radio(49, 53);  // CE, CSN pins
byte addresses[][6] = {"1Node", "2Node"};

void grip(int16_t ay);
void shoulder(int16_t ay);
void elbow(int16_t ay);
void wrist(int16_t ay);

enum HandState {
  NEUTRAL, // No action state
  DRIVE,
  ROTATE_BASE,
  WRIST,
  ELBOW,
  SHOULDER,
  GRIP,
};

struct SensorData {
  HandState state;
  int16_t ax;
  int16_t ay;
  int16_t az;
}; 
SensorData data;

void setup() {
  Serial.begin(9600);
  pinMode(LmotorPin1, OUTPUT);
  pinMode(LmotorPin2, OUTPUT);
  pinMode(ENA_PIN_LEFT, OUTPUT);
  pinMode(RmotorPin1, OUTPUT);
  pinMode(RmotorPin2, OUTPUT);
  pinMode(ENA_PIN_RIGHT, OUTPUT);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(128);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    data.state = NEUTRAL; // base case
    radio.read(&data, sizeof(data));
    execute(data.state);
  }
}

void execute(HandState currentState){
  // int16_t ax = 0, ay = 0; // ax and ay need to be defined to use in this function
    switch (data.state) {
      case NEUTRAL:
        Serial.println("Neutral Mode!");
        break;
      case DRIVE:
        drive(data.ax, data.ay);
        break;
      case ROTATE_BASE:
        rotate(data.ay);
        break;
      case WRIST:
        wrist(data.ax);
        break;
      case ELBOW:
        elbow(data.ax);
        break;
      case SHOULDER:
        shoulder(data.ax);
        break;
      case GRIP:
        grip(data.ax);
        break;
    }
}

void drive(int16_t ax, int16_t ay){
  // write gyroscope/accelerometer code below
  // Up/down/left/right controlled with gyroscope
  Serial.println("Drive Mode!");
  Serial.print(data.ax);
  Serial.print(", ");
  Serial.println(data.ay);
  int16_t speed = 0;

  if (abs(data.ax) >= 230){
    speed = 255;
  } 
  else if (abs(data.ax) >= 155) {
    speed = 128;
  }
  else {
    speed = 0;
  }

  Serial.println(speed);

  if (data.ay < 75) {

    analogWrite(ENA_PIN_LEFT, speed);
    analogWrite(ENA_PIN_RIGHT, speed);
    digitalWrite(LmotorPin1, LOW); 
    digitalWrite(LmotorPin2, HIGH);
    digitalWrite(RmotorPin1, LOW); 
    digitalWrite(RmotorPin2, HIGH);
    Serial.println("Tilted Right");

  } else if (data.ay > 200) {

    analogWrite(ENA_PIN_LEFT, speed);
    analogWrite(ENA_PIN_RIGHT, speed);
    digitalWrite(LmotorPin1, HIGH); 
    digitalWrite(LmotorPin2, LOW);
    digitalWrite(RmotorPin1, HIGH); 
    digitalWrite(RmotorPin2, LOW);
    Serial.println("Tilted Left");
  }

  if (data.ax > 190) {

    analogWrite(ENA_PIN_LEFT, speed);
    analogWrite(ENA_PIN_RIGHT, speed);
    digitalWrite(LmotorPin1, LOW); 
    digitalWrite(LmotorPin2, HIGH);
    
    digitalWrite(RmotorPin1, HIGH); 
    digitalWrite(RmotorPin2, LOW);

    Serial.println("Tilted Forward");
    
  } else if (data.ax < 100) {

    analogWrite(ENA_PIN_LEFT, speed);
    analogWrite(ENA_PIN_RIGHT, speed);
    digitalWrite(LmotorPin1, HIGH); 
    digitalWrite(LmotorPin2, LOW);
    digitalWrite(RmotorPin1, LOW); 
    digitalWrite(RmotorPin2, HIGH);

    Serial.println("Tilted Backward");
  }
  
}  
void rotate(int16_t ax){
  // write gyroscope/accelerometer code below
  // Left/right controlled with gyroscope
  Serial.println("Rotate Mode!");

  while(check_flex_sensor && data.ax > 6000) {
    static HandState currentState = NEUTRAL; 
    HandState newState = determineServoState();
    if (newState != currentState) {
      currentState = newState;
    }
    switch (currentState) {
      case ROTATE_BASE:
        check_flex_sensor = true;
        myStepper.step(1);
        Serial.print("steps:");
        Serial.println(stepCount);
        stepCount++;
        delay(500);
      Serial.println("Rotate Right");
    }
  }

  while (data.ax < -6000) {
    static HandState currentState = NEUTRAL; 
    HandState newState = determineServoState();
    if (newState != currentState) {
      currentState = newState;
    }
    switch (currentState) {
    case ROTATE_BASE:
      check_flex_sensor = true;
      myStepper.step(-1);
      Serial.print("steps:");
      Serial.println(stepCount);
      stepCount--;
      delay(500);
    Serial.println("Rotate Left");
    }
  }
}

void wrist(int16_t ay){
  // write gyroscope/accelerometer code below
  // Up/down controlled with gyroscope
  Serial.println("Wrist Mode!");

  if (data.ay > 6000) {
    Serial.println("Wrist Forward");
  } else if (data.ay < -6000) {
    Serial.println("Wrist Backward");
  }
}

void elbow(int16_t ay){
  // write gyroscope/accelerometer code below
  // Up/down controlled with gyroscope
  Serial.println("Elbow Mode!");

  if (data.ay > 6000) {
    Serial.println("Elbow Forward");
  } else if (data.ay < -6000) {
    Serial.println("Elbow Backward");
  }

}

void shoulder(int16_t ay){
  // write gyroscope/accelerometer code here
  // Up/down controlled with gyroscope
  Serial.println("Shoulder Mode!");

  if (data.ay > 6000) {
    Serial.println("Shoulder Forward");
  } else if (data.ay < -6000) {
    Serial.println("Shoulder Backward");
  }

}

void grip(int16_t ay){
  // write gyroscope/accelerometer code here
  // Up/down controlled with gyroscope
  Serial.println("Grip Mode!");

  if (data.ay > 6000) {
    Serial.println("Grip Close");
  } else if (data.ay < -6000) {
    Serial.println("Grip Open");
  }
}
