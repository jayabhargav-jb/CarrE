#include "header_for_two_motor_control.h"

#define PID_INCLUDED 1

// Interrupt service routine (ISR) for Motor 1
void pulseCounter1() {
  pulseCount1++;
}

void pulseCounter2() {
  pulseCount2++;
}

// Function to calculate checksum
int calculateChecksum(const char* message) {
  int checksum = 0;
  int length = strlen(message);

  // Iterate over each character in the message (excluding the checksum part)
  for (int i = 0; i < length - 4; i++) {
    // Serial.println(message[i]);
    checksum += message[i];  // Add the ASCII value of each character
  }
  return checksum % 256;  // Modulo 256 to keep checksum within byte range
}

// Function to extract the checksum from the command
int extractChecksum(const char* message) {
  char checksumStr[4];  // Buffer to store checksum (3 digits + null terminator)
  strncpy(checksumStr, &message[strlen(message) - 3], 3);  // Extract last 3 characters
  checksumStr[3] = '\0';  // Null-terminate
  return atoi(checksumStr);  // Convert to integer
}

// Function to verify the checksum
bool verifyChecksum(const char* message) {
  int receivedChecksum = extractChecksum(message);
  // Serial.println(receivedChecksum);
  int calculatedChecksum = calculateChecksum(message);
  return receivedChecksum == calculatedChecksum;
}

float speed_calculation(unsigned long int pulse_Counter1, unsigned long int pulse_Counter2) {
  pulseCountdiff_1 = (pulse_Counter1 - lastpulseCount1);
  pulseCountdiff_2 = (pulse_Counter2 - lastpulseCount2);
  
  rpm1 = (pulseCountdiff_1 * multiplier_constant) / (pulsesPerRevolution * 19.1);
  rpm2 = (pulseCountdiff_2 * multiplier_constant) / (pulsesPerRevolution * 19.1);

  if (lastpulseCount1 > pulseCount1) {
    rpm1 = prevrpm1;
  }
  if (lastpulseCount2 > pulseCount2) {
    rpm2 = prevrpm2;
  }

  lastpulseCount1 = pulseCount1;
  prevrpm1 = rpm1;

  lastpulseCount2 = pulseCount2;
  prevrpm2 = rpm2;

  return rpm1, rpm2;
}

#ifdef PID_INCLUDED
int PID_calculations(float rpm1, float rpm2) {
  unsigned long currentTime = millis();
  unsigned long deltaTime = (currentTime - previousTime);
  
  error1 = (setpoint1) - (rpm1);
  error2 = (setpoint2) - (rpm2);

  integral1 += (error1 * deltaTime);
  integral2 += (error2 * deltaTime);

  if (integral1 > integralMax) integral1 = integralMax;
  if (integral1 < integralMin) integral1 = integralMin;

  if (integral2 > integralMax) integral2 = integralMax;
  if (integral2 < integralMin) integral2 = integralMin;

  derivative1 = ((error1 - prevError1) / (deltaTime));
  derivative2 = ((error2 - prevError2) / (deltaTime));

  output1 = (Kp1 * error1) + (Ki1 * integral1) + (Kd1 * derivative1);
  output2 = (Kp2 * error2) + (Ki2 * integral2) + (Kd2 * derivative2);

  out1 = (int)(output1);
  out2 = (int)(output2);

  out1 = (int)constrain(output1, 0, 100);
  out2 = (int)constrain(output2, 0, 100);

  prevError1 = error1;
  prevError2 = error2;
  previousTime = currentTime;

  return out1, out2;
}
#endif

void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate
  cmd_val[11] = '\0';
  pinMode(enable_1r, OUTPUT);
  pinMode(enable_1l, OUTPUT);
  pinMode(enable_2r, OUTPUT);
  pinMode(enable_2l, OUTPUT);

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  pinMode(pwm_r1, OUTPUT);
  pinMode(pwm_l2, OUTPUT);
  pinMode(pwm_r2, OUTPUT);
  pinMode(pwm_l1, OUTPUT);

  digitalWrite(enable_1r, HIGH);
  digitalWrite(enable_1l, HIGH);
  digitalWrite(enable_2r, HIGH);
  digitalWrite(enable_2l, HIGH);

  attachInterrupt(digitalPinToInterrupt(encoderPin1), pulseCounter1, RISING);  // Motor 1 encoder
  attachInterrupt(digitalPinToInterrupt(encoderPin2), pulseCounter2, RISING);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check if enough time has passed and the checksum is valid
  if (Serial.available() > 11) {
    Serial.readBytesUntil('\n', cmd_val, 12);  // Read until newline
    cmd_val[11] = '\0';  // Ensure null termination

    // Verify checksum before processing
    if (!verifyChecksum(cmd_val)) {
      // Serial.println("Checksum invalid");
      dir = 0;
    }
    else {
      // Parse the message (excluding the checksum)
      char* token = strtok(cmd_val, " ");  // Split by space
      if (token) {
        dir = atoi(token);  // Convert first token to integer
        token = strtok(NULL, " ");  // Move to next token
      }
      if (token) {
        setpoint1 = atoi(token);  // Convert second token to integer
        token = strtok(NULL, " ");  // Move to next token
      }
      if (token) {
        setpoint2 = atoi(token);  // Convert third token to integer
      }
    }
    
  }

  if ((currentMillis - previousMillis) >= interval) {
    rpm1, rpm2 = speed_calculation(pulseCount1, pulseCount2);

#ifdef PID_INCLUDED
    out1, out2 = PID_calculations(rpm1, rpm2);
#else
    out1 = 30 * 1.5;
    out2 = 30 * 1.5;
#endif

    pwmforconversion_1 = (out1 / 1.5);
    pwmforconversion_2 = (out2 / 1.5);

    if (dir == 1) {
      analogWrite(pwm_l1, pwmforconversion_1);
      analogWrite(pwm_r1, 0);
      analogWrite(pwm_l2, 0);
      analogWrite(pwm_r2, pwmforconversion_2);
    }
    else if (dir == 2) {
      analogWrite(pwm_l1, 0);
      analogWrite(pwm_r1, pwmforconversion_1);
      analogWrite(pwm_l2, pwmforconversion_2);
      analogWrite(pwm_r2, 0);
    }
    else {
      analogWrite(pwm_l1, 0);
      analogWrite(pwm_r1, 0);
      analogWrite(pwm_l2, 0);
      analogWrite(pwm_r2, 0);
    }

    previousMillis = currentMillis;  // Update time tracking
  }
}
