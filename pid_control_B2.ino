#include <PID_v1.h>
#include <Encoder.h>

// Motor control pins
const int motor1Pin = 10; // PWM pin for motor 1
const int motor2Pin = 11; // PWM pin for motor 2

// Encoder pins
const int encoder1PinA = 18; // Encoder 1 A pin
const int encoder1PinB = 19; // Encoder 1 B pin
const int encoder2PinA = 20; // Encoder 2 A pin
const int encoder2PinB = 21; // Encoder 2 B pin

// PID parameters
double Setpoint1, Input1, Output1;
double Setpoint2, Input2, Output2;
double Kp = 1, Ki = 2, Kd = 0; // Adjust these values as needed

// Create PID instances
PID myPID1(&Input1, &Output1, &Setpoint1, Kp, Ki, Kd, DIRECT);
PID myPID2(&Input2, &Output2, &Setpoint2, Kp, Ki, Kd, DIRECT);

// Integral windup limits
double windupGuard = 100; // Max output for PWM

// Create encoder instances
Encoder encoder1(encoder1PinA, encoder1PinB);
Encoder encoder2(encoder2PinA, encoder2PinB);

// Speed control variables
volatile long lastPosition1 = 0;
volatile long lastPosition2 = 0;
double speed1 = 0;
double speed2 = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(motor1Pin, OUTPUT);
  pinMode(motor2Pin, OUTPUT);

  // Initialize setpoints (desired speed)
  Setpoint1 = 25; // Desired speed for motor 1
  Setpoint2 = 25; // Desired speed for motor 2

  // Initialize PID
  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
  
  // Set output limits
  myPID1.SetOutputLimits(0, windupGuard);
  myPID2.SetOutputLimits(0, windupGuard);

  // Initialize encoder positions
  encoder1.write(0);
  encoder2.write(0);
}

void loop() {
  // Calculate speed every 100 ms
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 100) {
    // Read encoder values
    long position1 = encoder1.read();
    long position2 = encoder2.read();

    // Calculate speed (counts per second)
    speed1 = (position1 - lastPosition1) * 10; // Multiplied by 10 for 100 ms interval
    speed2 = (position2 - lastPosition2) * 10;

    // Update last positions
    lastPosition1 = position1;
    lastPosition2 = position2;

    // Set the input for PID computation
    Input1 = speed1;
    Input2 = speed2;

    // Compute PID for both motors
    myPID1.Compute();
    myPID2.Compute();

    // Constrain outputs to avoid windup
    Output1 = constrain(Output1, 0, windupGuard);
    Output2 = constrain(Output2, 0, windupGuard);

    // Write to motors
    analogWrite(motor1Pin, Output1);
    analogWrite(motor2Pin, Output2);

    // Print for debugging
    Serial.print("Motor 1 Output: ");
    Serial.print(Output1);
    Serial.print(" | Speed: ");
    Serial.print(speed1);
    Serial.print(" | Motor 2 Output: ");
    Serial.print(Output2);
    Serial.print(" | Speed: ");
    Serial.println(speed2);

    lastTime = currentTime;
  }
}
