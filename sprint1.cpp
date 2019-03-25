#include "BrickPi3.cpp" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  
  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
  BP.set_motor_limits(PORT_B, 60, 0);
  BP.set_motor_limits(PORT_C, 60, 0);
  
  int error;
  int color = 300;
  int light = 2600;
  
  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
  BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
  
  sensor_color_t      Color1;
  sensor_light_t      Light3;

  while(true){
    BP.set_motor_power(PORT_B, 25);
    BP.set_motor_power(PORT_C, 25);
    while(true){
      if(BP.get_sensor(PORT_1, &Color1)){
        cout << Color1.reflected_green << " - " << Light3.reflected << " Straight\n";
      } else {
        if(BP.get_sensor(PORT_3, &Light3)){
          cout << Color1.reflected_green << " - " << Light3.reflected << " Straight\n";
        } else {
          cout << Color1.reflected_green << " - " << Light3.reflected << " Straight\n";
          if((Color1.reflected_green < 200) != (Light3.reflected > 2500)){
            color = Color1.reflected_green;
            light = Light3.reflected;
            break;
          }
        }
      }
    }
    if(color < 200){
      BP.set_motor_power(PORT_B, -25);
      BP.set_motor_power(PORT_C, 25);
      usleep(20000);
    } else if( light > 2500){
      BP.set_motor_power(PORT_B, 25);
      BP.set_motor_power(PORT_C, -25);
      usleep(20000);
    }
  }
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
