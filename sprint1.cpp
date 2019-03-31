#include "BrickPi3.cpp" // For BrickPi3
#include <stdio.h>      // For printf
#include <unistd.h>     // For usleep
#include <signal.h>     // For catching exit signals
#include <iostream>     // For using cout

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

int main(){
	signal(SIGINT, exit_signal_handler);	// Register the exit function for Ctrl+C

	BP.detect(); 	// Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_motor_limits(PORT_B, 60, 0);
	BP.set_motor_limits(PORT_C, 60, 0);
	// Start values for sensors
	int color = 300;
	int light = 2600;
	int distance = 200;
	int command = 0;
	// Sensor setup
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	// Assign sensor names
	sensor_color_t      Color1;
	sensor_ultrasonic_t Ultrasonic4;
	sensor_light_t      Light3;

	while(true){
		BP.set_motor_power(PORT_B, 25); // Set basic robot speed
		BP.set_motor_power(PORT_C, 25);
		while(true){
			BP.get_sensor(PORT_1, &Color1);	// Get data from color sensor
			BP.get_sensor(PORT_4, &Ultrasonic4);// Get data from ultrasonic sensor
			BP.get_sensor(PORT_3, &Light3);// Get data from light sensor
			cout << (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3 << " - " << Light3.reflected << " - " << Ultrasonic4.cm << "\n"; // Print data gained from sensors
			if(Ultrasonic4.cm < 10 && Ultrasonic4.cm > 0.1){ // Compare data from sensor with predetermined values
				distance = Ultrasonic4.cm; // Assign gained data to variable for later use
				command = 1; // Distance
				break;
			}
			else if(((Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3 < 300) != (Light3.reflected > 2300)){ // If either the right or left sensor sees the black line
				color = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
				light = Light3.reflected;
				command = 2;	// Left and right
				break;
			}
			else if(((Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3 < 300) && (Light3.reflected > 2300)){ // If both the right and left sensors see a black line
				color = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
				light = Light3.reflected;
				command = 3;	// Crossroad
				break;
			}
		}
		if(color < 300 && command == 2){ // Left
			BP.set_motor_power(PORT_B, -25);
			BP.set_motor_power(PORT_C, 25);
			usleep(20000);
		}
		else if(light > 2300 && command == 2){ // Right
			BP.set_motor_power(PORT_B, 25);
			BP.set_motor_power(PORT_C, -25);
			usleep(20000);
		}
		else if(command == 3){ // Straight at crossroad
			BP.set_motor_power(PORT_B, 25);
			BP.set_motor_power(PORT_C, 25);
			usleep(200000);
		}
		else if(distance < 7 && distance > 0.1 && command == 1){ // Spot obstacle
			BP.set_motor_position_relative(PORT_B, 500); // First turn, turn 90 degrees then drive forward
			BP.set_motor_position_relative(PORT_C, -500);
			usleep(2000000);
			BP.set_motor_power(PORT_B, 25);
			BP.set_motor_power(PORT_C, 25);
			BP.set_motor_position_relative(PORT_D, 320); // Turn ultrasonic scanner sideways
			usleep(2000000);
			while(distance < 10){	
				BP.get_sensor(PORT_4, &Ultrasonic4); // Check distance between robot and obstacle while driving forward
				distance = Ultrasonic4.cm;
				cout << distance << " 1st While \n";
			}
			BP.set_motor_position_relative(PORT_D, -320); // Obstacle no longer in range, turn ultrasonic sensor back
			usleep(1000000);
			BP.set_motor_power(PORT_B, 0);
			BP.set_motor_power(PORT_C, 0);
			BP.set_motor_position_relative(PORT_B, -600); // Second turn, turn 90 degrees and continue forward
			BP.set_motor_position_relative(PORT_C, 600);
			usleep(2000000);
			BP.set_motor_power(PORT_B, 25);
			BP.set_motor_power(PORT_C, 25);
			usleep(1000000);
			BP.set_motor_position_relative(PORT_D, 320); // Turn ultrasonic sensor sideways
			usleep(3000000);
			while(distance < 20){ // While obstacle is in sight keep driving and scanning the distance
				BP.get_sensor(PORT_4, &Ultrasonic4);
				distance = Ultrasonic4.cm;
				cout << distance << " 2nd While \n";
			}
			BP.set_motor_position_relative(PORT_D, -320); // Turn ultrasonic sensor forward
			usleep(1500000);
			BP.set_motor_power(PORT_B, 0);
			BP.set_motor_power(PORT_C, 0);
			BP.set_motor_position_relative(PORT_B, -600); // Third turn. Turn 90 degrees and continue driving forward until the line is found, turn right and then follow the regular set of instructions
			BP.set_motor_position_relative(PORT_C, 600);
			usleep(2000000);
			BP.set_motor_power(PORT_B, 25);
			BP.set_motor_power(PORT_C, 25);
			BP.get_sensor(PORT_3, &Light3);// Get data from light sensor
			light = Light3.reflected;
			while(light < 2300){
				BP.get_sensor(PORT_3, &Light3);// Get data from light sensor
				light = Light3.reflected;
			}
			BP.set_motor_power(PORT_B, 0);
			BP.set_motor_power(PORT_C, 0);
			BP.set_motor_position_relative(PORT_B, 200);
			BP.set_motor_position_relative(PORT_C, -200);
			usleep(2000000);
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