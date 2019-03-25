#include "BrickPi3.cpp" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  
  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
  BP.set_motor_limits(PORT_B, 60, 0);
  BP.set_motor_limits(PORT_C, 60, 0);
  
  int error;
  int color = 200;
  int light = 100;
  
  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
  BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
  
  sensor_color_t      Color1;
  sensor_light_t      Light3;

  while(true){
    error = 0;

    if(color > 200 && light < 2500){
      BP.set_motor_power(PORT_B, 25);
      BP.set_motor_power(PORT_C, 25);
      usleep(200000);
      BP.set_motor_power(PORT_B, 0);
      BP.set_motor_power(PORT_C, 0);
    } else if(color < 200){
      BP.set_motor_power(PORT_B, -25);
      BP.set_motor_power(PORT_C, 25);
      usleep(200000);
      BP.set_motor_power(PORT_B, 0);
      BP.set_motor_power(PORT_C, 0);
    } else if(light > 2500){
      BP.set_motor_power(PORT_B, 25);
      BP.set_motor_power(PORT_C, -25);
      usleep(200000);
      BP.set_motor_power(PORT_B, 0);
      BP.set_motor_power(PORT_C, 0);
    }    

    if(BP.get_sensor(PORT_1, &Color1)){
      error++;
    }else{
      printf("Color sensor (S1): detected %d red %4d green %4d blue %4d ambient %4d\n", Color1.color, Color1.reflected_red, Color1.reflected_green, Color1.reflected_blue, Color1.ambient);
      color = Color1.reflected_green;
    }
    
    if(BP.get_sensor(PORT_3, &Light3)){
      error++;
    }else{
      printf("Light sensor (S3): reflected %4d   ", Light3.reflected);
      light = Light3.reflected;
    }
    
    if(error == 2){
      printf("Waiting for sensors to be configured");
    }
    
    printf("\n\n");
    usleep(200000);
  }
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
