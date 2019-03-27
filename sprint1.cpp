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
  int distance = 200;
  int command = 0;
  
  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
  BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_ULTRASONIC);
  BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
  
  sensor_color_t      Color1;
  sensor_ultrasonic_t Ultrasonic4;
  sensor_light_t      Light3;

  while(true){
    BP.set_motor_power(PORT_B, 25);
    BP.set_motor_power(PORT_C, 25);
    while(true){
      BP.get_sensor(PORT_1, &Color1);
      BP.get_sensor(PORT_4, &Ultrasonic4);
      BP.get_sensor(PORT_3, &Light3);
      cout << Color1.reflected_green << " - " << Light3.reflected << " - " << Ultrasonic4.cm << "\n";
      if(Ultrasonic4.cm < 10 && Ultrasonic4.cm > 0.1){
        distance = Ultrasonic4.cm;
        command = 1;
        break;
      } else if((Color1.reflected_green < 200) != (Light3.reflected > 2500)){
        color = Color1.reflected_green;
        light = Light3.reflected;
        command = 2;
        break;
      }
    }
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    if(color < 200 && command == 2){
      BP.set_motor_power(PORT_B, -25);
      BP.set_motor_power(PORT_C, 25);
      usleep(20000);
    } else if(light > 2500 && command == 2){
      BP.set_motor_power(PORT_B, 25);
      BP.set_motor_power(PORT_C, -25);
      usleep(20000);
    } else if(distance < 20 && distance > 0.1 && command == 1){
      BP.set_motor_position_relative(PORT_B, 600);
      BP.set_motor_position_relative(PORT_C, -600);
      usleep(2000000);
      BP.set_motor_power(PORT_B, 25);
      BP.set_motor_power(PORT_C, 25);
      usleep(1000000);
      BP.set_motor_power(PORT_B, 0);
      BP.set_motor_power(PORT_C, 0);
      while(distance < 30){
	BP.set_motor_power(PORT_B, 25);
        BP.set_motor_power(PORT_C, 25);
        usleep(1000000);
	BP.set_motor_power(PORT_B, 0);
        BP.set_motor_power(PORT_C, 0);
        BP.set_motor_position_relative(PORT_D, 400);
  	usleep(2000000);
  	BP.get_sensor(PORT_4, &Ultrasonic4);
  	distance = Ultrasonic4.cm;
  	BP.set_motor_position_relative(PORT_D, -400);
  	usleep(2000000);
	cout << distance << " 1st While \n";
      }
      BP.set_motor_position_relative(PORT_B, -600);
      BP.set_motor_position_relative(PORT_C, 600);
      usleep(2000000);
      BP.get_sensor(PORT_4, &Ultrasonic4);
      distance = Ultrasonic4.cm;
      cout << distance << " Second Check \n";
      if(distance < 20){
        BP.set_motor_position_relative(PORT_B, 600);
        BP.set_motor_position_relative(PORT_C, -600);
        usleep(2000000);
        command = 4;
      }
      while(distance < 20){
	BP.set_motor_power(PORT_B, 25);
        BP.set_motor_power(PORT_C, 25);
        usleep(1000000);
	BP.set_motor_power(PORT_B, 0);
        BP.set_motor_power(PORT_C, 0);
        BP.set_motor_position_relative(PORT_D, 400);
  	usleep(2000000);
  	BP.get_sensor(PORT_4, &Ultrasonic4);
  	distance = Ultrasonic4.cm;
  	BP.set_motor_position_relative(PORT_D, -400);
  	usleep(2000000);
	cout << distance << " 2nd While \n";
        command++;
      }
      if(command >= 4){
        if(command == 4){
          BP.set_motor_power(PORT_B, 25);
          BP.set_motor_power(PORT_C, 25);
          usleep(1000000);
          BP.set_motor_power(PORT_B, 0);
          BP.set_motor_power(PORT_C, 0);
        }
        BP.set_motor_position_relative(PORT_B, -500);
        BP.set_motor_position_relative(PORT_C, 500);
        usleep(2000000);
      }
      BP.set_motor_power(PORT_B, 25);
      BP.set_motor_power(PORT_C, 25);
      usleep(3000000);
      BP.set_motor_power(PORT_B, 0);
      BP.set_motor_power(PORT_C, 0);
      BP.set_motor_position_relative(PORT_D, 400);
      usleep(2000000);
      BP.get_sensor(PORT_4, &Ultrasonic4);
      distance = Ultrasonic4.cm;
      BP.set_motor_position_relative(PORT_D, -400);
      usleep(2000000);
      cout << distance << " Third Check \n";
      while(distance < 30){
	BP.set_motor_power(PORT_B, 25);
        BP.set_motor_power(PORT_C, 25);
        usleep(1000000);
	BP.set_motor_power(PORT_B, 0);
        BP.set_motor_power(PORT_C, 0);
        BP.set_motor_position_relative(PORT_D, 400);
  	usleep(2000000);
  	BP.get_sensor(PORT_4, &Ultrasonic4);
  	distance = Ultrasonic4.cm;
  	BP.set_motor_position_relative(PORT_D, -400);
  	usleep(2000000);
	cout << distance << " 3rd While \n";
      }
      BP.set_motor_position_relative(PORT_B, 100);
      BP.set_motor_position_relative(PORT_C, 1300);
      usleep(2000000);
      BP.set_motor_power(PORT_B, 25);
      BP.set_motor_power(PORT_C, 25);
      usleep(1000000);
      BP.set_motor_power(PORT_B, 0);
      BP.set_motor_power(PORT_C, 0);
      while(true){
        BP.get_sensor(PORT_1, &Color1);
        color = Color1.reflected_green;
        cout << color << " Green \n";
        if(color < 200){
          BP.set_motor_power(PORT_B, 0);
          BP.set_motor_power(PORT_C, 0);
          break;
        } else {
          BP.set_motor_power(PORT_B, 25);
          BP.set_motor_power(PORT_C, 25);
        }
      }
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
