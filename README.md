# Mano Po: Mobile Gesture-Controlled Robotic Arm

Link to [full report](Design%20Report.pdf).

Mano Po is a wirelessly controlled mobile robot with a 4-DOF robotic arm, operated through a real-time gesture glove interface. Developed for MAE 93: FUSION Engineering Project at UC Irvine, the system combines sensor-based hand tracking, wireless communication, tank-tread mobility, and articulated arm control to enable intuitive human-in-the-loop robotics.

Designed for hazardous or hard-to-reach environments, the robot allows users to remotely drive, rotate, position arm joints, and grip objects through natural hand gestures.

---

## Key Features
- Wireless gesture glove control
- Flex sensor–based mode switching
- NRF24L01 low-latency wireless communication
- 6-axis MPU6050 motion tracking
- Tank-tread chassis with in-place turning
- 4-DOF articulated robotic arm
- Grip control through tilt-based gesture input
- Real-time state machine for mutually exclusive control modes

--- 

## Hardware Stack

**Glove Controller (Transmitter)**
- Arduino Uno
- 4× Flex Sensors
- MPU6050 Accelerometer + Gyroscope
- NRF24L01 Wireless Module
- Custom glove-mounted perfboard

**Mobile Robot (Receiver)**
- Arduino Mega
- Dual DC motors + L298N motor driver
- Stepper motor for base swivel
- 4 servo motors for articulated arm + claw
- Tank tread drivetrain
- 12V 5Ah LiPo battery
- Custom 3D-printed arm + claw assembly

---

## Software Architecture
See [transmitter.ino](/Arduino%20%20Code/transmitter.ino) and [receiver.ino](/Arduino%20%20Code/receiver.ino) for full code.

The system uses a wireless client-server architecture.

**Input Pipeline**
1. Flex sensors detect finger-specific gestures
2. MPU6050 captures tilt and orientation
3. Arduino Uno packages data into a SensorData struct
4. NRF24L01 transmits data wirelessly
5. Arduino Mega interprets the current control state
6. Corresponding motors and servos execute motion

Example of data transmission:

```
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
```
--- 
**State Machine Logic**
The robot operates through a set of mutually exclusive states determined by determineState():

| Mode | Gesture | Action |
| ----- | --------------------------- | ------ |
| Neutral | All fingers open | Idle/Safe mode |
| Drive | All fingers flexed | Move chassis |
| Rotate Base | Thumb open, other fingers flexed | Swivel arm base |
| Wrist | Index finger flexed | Wrist movement |
| Elbow | Middle finger flexed | Elbow movement |
| Shoulder | Ring finger flexed | Shoulder movement |
| Grip | Thumb finger flexed | Open/close claws |

This architecture prevents overlapping commands and makes robot control feel more intuitive.

---

**Control Algorithms**

**Drive Control**
- Hand tilt forward/backward → forward & reverse motion
- Hand tilt left/right → in-place turning
- Multi-stage PWM speed control:
    - 0
    - 128
    - 255

**Joint Stabilization**
Joint motion uses threshold-based gyroscope filtering (±6000) to reduce unintended jitter caused by natural hand shakiness.

---

## Results

The robot successfully demonstrated:
- Wireless real-time gesture control
- Stable lateral movement across terrain
- Accurate joint articulation
- Successful block pickup and transport
- Reliable claw gripping
- Competition-ready prototype performance

The final system was able to grab objects, transport them, and place them into an onboard tray, validating both mobility and manipulation workflows.

--- 

## Engineering Challenges & Improvements

**Key lessons from testing:**
- Chassis weight reduced motor efficiency
- Torque limitations impacted tread performance
- Material choices increased system mass
- Mechanical-hardware integration required tighter coordination

**Future Improvements:**
- ESP32-based wireless upgrade
- Higher torque drive motors
- Gear reduction system for treads
- Lighter laser-cut chassis
- Better weight distribution
- Simplified claw actuation
- Improved torque transfer shafts

---

## My Contributions

**Role: Hardware Subteam Lead**

**Hardware Engineering**
- Managed component sourcing, logistics, and BOM planning
- Selected sensors, microcontrollers, motors, drivers, and power components
- Designed full electrical schematics for glove transmitter and robot receiver systems
- Planned wiring architecture for motors, flex sensors, IMU, and wireless modules
- Optimized hardware choices around weight, power budget, and competition constraints

**PCB & Circuit Assembly**
- Built and soldered custom perfboard and PCB assemblies
- Integrated NRF24L01, MPU6050, flex sensors, and power regulation circuitry
- Performed wire management, connector routing, and reliability improvements

**Embedded Systems / Firmware**
- Wrote Arduino firmware for:
    - wireless glove transmitter
    - robot receiver
    - motor driver control
    - servo articulation
    - stepper base rotation
- Implemented gesture-to-state machine logic
- Built sensor data structs and wireless packet transmission
- Programmed IMU threshold filtering to reduce motion jitter
- Tuned PWM motor control and joint response behavior

**Testing & Integration**
- Led full hardware bring-up and subsystem debugging
- Tested wireless communication reliability
- Tuned motor responsiveness and grip control
- Troubleshot mechanical-electrical integration issues during final assembly
- Iterated on prototype failures under competition constraints


