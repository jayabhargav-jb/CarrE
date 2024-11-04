#include<stdlib.h>
#define l_motor_driver_left_en 5
#define l_motor_driver_right_en 4
#define l_motor_driver_left_pwm 10
#define l_motor_driver_right_pwm 9
#define r_motor_driver_left_en 7
#define r_motor_driver_right_en 6
#define r_motor_driver_left_pwm 12
#define r_motor_driver_right_pwm 11
// int cmd_val;

// char* cmd_val = (char*)malloc(10 * sizeof(char)); 
char cmd_val[11];
// variables
int dir, lpwm, rpwm;

int pwm;

int last_pwm_r = 0;
int last_pwm_l = 0;

void setup() {
  Serial.begin(9600);
  // cmd_val[8] = '\0';
  // pinMode(encoder, INPUT);
  // pinMode(encoder1, INPUT);
  pinMode(l_motor_driver_left_en, OUTPUT);
  pinMode(l_motor_driver_right_en, OUTPUT);
  pinMode(l_motor_driver_left_pwm, OUTPUT);
  pinMode(l_motor_driver_right_pwm, OUTPUT);
  pinMode(r_motor_driver_left_en, OUTPUT);
  pinMode(r_motor_driver_right_en, OUTPUT);
  pinMode(r_motor_driver_left_pwm, OUTPUT);
  pinMode(r_motor_driver_right_pwm, OUTPUT);
  // attachInterrupt(digitalPinToInterrupt(encoder),pulse, RISING);
  // attachInterrupt(digitalPinToInterrupt(encoder2),pulse_01, RISING);
  digitalWrite(l_motor_driver_left_en, 1);
  digitalWrite(l_motor_driver_right_en, 1);
  digitalWrite(r_motor_driver_left_en, 1);
  digitalWrite(r_motor_driver_right_en, 1);
}


void loop() {
  // char* cmd_val = (char*)malloc(10 * sizeof(char));
    if (Serial.available() > 9) {
    Serial.readBytesUntil('\n', cmd_val, 11); // Read until newline
    Serial.println(cmd_val);
    cmd_val[9] = '\0'; // Ensure null termination
    // print

    // Serial.print("Received: ");
    // Serial.println(cmd_val);

    // Parsing the values from the string
    char* token = strtok(cmd_val, " "); // Split by space

    if (token) {
      dir = atoi(token); // Convert first token to integer
      token = strtok(NULL, " "); // Move to next token
    }

    if (token) {
      lpwm = atoi(token); // Convert second token to integer
      token = strtok(NULL, " "); // Move to next token
    }

    if (token) {
      rpwm = atoi(token); // Convert third token to integer
    }
    
    if (dir==1) {
      analogWrite(l_motor_driver_left_pwm, lpwm);
      analogWrite(l_motor_driver_right_pwm, 0);
      analogWrite(r_motor_driver_left_pwm, 0);
      analogWrite(r_motor_driver_right_pwm, rpwm);
    }

    else if (dir == 2) {
      analogWrite(l_motor_driver_left_pwm, 0);
      analogWrite(l_motor_driver_right_pwm, lpwm);
      analogWrite(r_motor_driver_left_pwm, rpwm);
      analogWrite(r_motor_driver_right_pwm, 0);
    }

    else {
      analogWrite(l_motor_driver_left_pwm, 0);
      analogWrite(l_motor_driver_right_pwm, 0);
      analogWrite(r_motor_driver_left_pwm, 0);
      analogWrite(r_motor_driver_right_pwm, 0);
    }

    // Serial.flush(); // Wait for the transmission of outgoing serial data to complete
  // Serial.println("working");
  }
  // free(cmd_val);
}