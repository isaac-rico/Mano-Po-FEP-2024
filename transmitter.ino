#include <RF24.h>
#include <SPI.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Stepper.h>
#include <unistd.h>
MPU6050 mpu;

// NRF setup
RF24 radio(9, 10);  // CE, CSN pins
const byte addresses[][6] = { "1Node", "2Node" };

// pin + sensors
const int FLEX_PINS[] = { A0, A1, A2, A3 };
const int NUM_SENSORS = 4;
const int FLEX_THRESHOLD = 500;

// MPU data
int16_t ax, ay, az;
int16_t gx, gy, gz;
struct mpuData {
  byte X;
  byte Y;
  byte Z;
};
mpuData g_data;

// Hand States
enum HandState {
  NEUTRAL,  // No action state
  DRIVE,
  ROTATE_BASE,
  WRIST,
  ELBOW,
  SHOULDER,
  GRIP,
};

// struct for data
struct SensorData {
  HandState state;
  int16_t ax;
  int16_t ay;
  int16_t az;
};


void setup() {
  Serial.begin(9600);
  radio.begin();
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(FLEX_PINS[i], INPUT);
    Serial.println(FLEX_PINS[i]);
  }

  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not connected");
  } else {
    Serial.println("MPU6050 connected");
  }
  //values below might change
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(128);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  SensorData data;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  data.ax = map(ax, -17000, 17000, 0, 255);
  data.ay = map(ay, -17000, 17000, 0, 255);
  data.az = map(az, -17000, 17000, 0, 255);
  // Serial.print("Axis X = ");
  // Serial.print(g_data.X);
  // Serial.print(" Axis Y = ");
  // Serial.print(g_data.Y);
  // Serial.print(" Axis Z = ");
  // Serial.println(g_data.Z);
  // HandState currentState = NEUTRAL; // Static variable to retain state between loop iterations, starts in NEUTRAL
  data.state = determineState();  // Determine the current state based on sensor readings
  Serial.println(data.state);
  Serial.print(data.ax);
  Serial.print(", ");
  Serial.print(data.ay);
  Serial.print(", ");
  Serial.println(data.az);
  radio.stopListening();
  bool result = radio.write(&data, sizeof(data));
  if (result) {
    Serial.println("Command sent successfully");
  } else {
    Serial.println("Error sending command");
  }
  delay(600);
}

HandState determineState() {
  int flexValues[NUM_SENSORS];
  // Read each flex sensor value
  for (int i = 0; i < NUM_SENSORS; i++) {
    flexValues[i] = analogRead(FLEX_PINS[i]);
  }
  int a0 = analogRead(FLEX_PINS[0]);
  int a1 = analogRead(FLEX_PINS[1]);
  int a2 = analogRead(FLEX_PINS[2]);
  int a3 = analogRead(FLEX_PINS[3]);

  // Check if all fingers are flexed for DRIVE mode
  bool allFingersFlexed = true;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (flexValues[i] < FLEX_THRESHOLD) {
      allFingersFlexed = false;  // If any finger is not flexed, exit loop
      break;
    }
  }

  if (allFingersFlexed) {

    return DRIVE;  // Enter DRIVE mode if all fingers are flexed
  }
  // Check for ROTATE_BASE state condition (Thumb open, other fingers flexed)
  if (a0 < FLEX_THRESHOLD && a1 >= FLEX_THRESHOLD && a2 >= FLEX_THRESHOLD && a3 >= FLEX_THRESHOLD) {
    return ROTATE_BASE;
  }
  //INSERT HAND FUNCTIONALITIES BELOW

  //create if statement for GRIP hand functionality

  else if (a0 >= FLEX_THRESHOLD && a1 < FLEX_THRESHOLD && a2 < FLEX_THRESHOLD && a3 < FLEX_THRESHOLD) {
    return GRIP;
  }

  //create if statement for WRIST hand functionality

  else if (a1 >= FLEX_THRESHOLD && a2 < FLEX_THRESHOLD && a3 < FLEX_THRESHOLD) {
    return WRIST;
  }

  //create if statement for ELBOW hand functionality

  else if (a2 >= FLEX_THRESHOLD && a1 < FLEX_THRESHOLD && a3 < FLEX_THRESHOLD) {
    return ELBOW;
  }

  //create if statement for SHOULDER hand functionality

  else if (a3 >= FLEX_THRESHOLD && a1 < FLEX_THRESHOLD && a2 < FLEX_THRESHOLD) {
    return SHOULDER;
  }

  //INSERT HAND FUNCTIONALITIES ABOVE
  return NEUTRAL;
}