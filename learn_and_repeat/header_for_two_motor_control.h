#ifndef _header_forpid_H
#define _header_forpid_H

#define pwm_r1 9
#define pwm_l1 10
#define pwm_r2 11
#define pwm_l2 12
#define enable_1r 4
#define enable_1l 5
#define enable_2r 6
#define enable_2l 7
const int encoderPin1 = 2;  // Encoder 1 signal pin 
const int encoderPin2 = 20;  // Encoder 2 signal pin
int calculateChecksum(const char* message);
int extractChecksum(const char* message);
bool verifyChecksum(const char* message);
void pulseCounter1();
void pulseCounter2();
class pid_control{
  private:
  struct array_1{
  float rpm_1[500];
  float rpm_2[500];
  int dir[500];
  }arr;
  
  public:
   
   float* speed_calculation(unsigned long int pulse_Counter1,unsigned long int pulse_Counter2);
   int* PID_calculations(float rpm_1, float rpm_2,float setpoint_1,float setpoint_2);
   void learn_phase(float rpm_1,float rpm_2,int dir);
   float* repeat_phase();
   float rpms_arr[2];
   int outputs_arr[2];
   float repeat_arr[3];
   //int repeat_command=0;
};






#endif // _header_forpid_H
