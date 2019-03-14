#include "BrickPi3.h"   // for BrickPi3
#include <iostream>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

//Stop
void stop(void)
{
	 BP.set_motor_power(PORT_B, 0);
	 BP.set_motor_power(PORT_C, 0);
}

//Move Forward
void fwd(void)
{
	 BP.set_motor_power(PORT_B, 75);
	 BP.set_motor_power(PORT_C, 75);
	 sleep(1);
	 stop();

}
//Move Left
void left(void)
{
	 BP.set_motor_position_relative(PORT_C, 270);
	 BP.set_motor_position_relative(PORT_B, -270);
}
//Move Right
void right(void)
{
	 BP.set_motor_position_relative(PORT_C, -270);
	 BP.set_motor_position_relative(PORT_B, 270);
}
//Move backward
void back(void)
{
	 BP.set_motor_power(PORT_B, 190);
	 BP.set_motor_power(PORT_C, 190);
	 sleep(1);
	 stop();
}

int main()
{
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
  	BP.set_motor_limits(PORT_B, 60, 0);
	BP.set_motor_limits(PORT_C, 60, 0);
  	char inp;

	while(true)
	{
		cout << "Press w (forward), s (backward), a (left), d (right), s (stop): " << endl;
		cin >> inp;	//Take input from the terminal
		//Move the bot
		if(inp=='w') {
			  	fwd();
			}

		else if (inp=='s') {
			  	back();
			}

		else if (inp=='a'){
				left();
			}
		else if (inp=='d'){
				right();
			}
		else if (inp=='s'){
				stop();
			}




	}

	return 0;
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}