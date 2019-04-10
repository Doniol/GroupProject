#include "BrickPi3.cpp" // For BrickPi3
#include <stdio.h>      // For printf
#include <stdlib.h>	// For system()
#include <unistd.h>     // For usleep
#include <signal.h>     // For catching exit signals
#include <iostream>     // For using cout
#include <time.h>	// For time

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
	int color_red = 0;
	int color_blue = 0;
	int color_green = 0;
	int sugar = 6;


	// Sensor setup
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_LIGHT_ON);

	// Assign sensor names
	sensor_color_t	    Color;
	sensor_ultrasonic_t Ultrasonic2;
	sensor_light_t      Light4;

	//Startup Sound
	//srand (time(NULL));
	
	//system("aplay 5u64r_d4ddy.wav");

	//BP.set_motor_power(PORT_B, 25); // Set basic robot speed
	//BP.set_motor_power(PORT_C, 25);

	while(true){
		BP.set_motor_power(PORT_B, 25); // Set basic robot speed
		BP.set_motor_power(PORT_C, 25);
		while(true){
			BP.get_sensor(PORT_2, &Ultrasonic2);// Get data from ultrasonic sensor
			BP.get_sensor(PORT_3, &Color);	// Get data from color sensor
			BP.get_sensor(PORT_4, &Light4);// Get data from light sensor
			cout << (Color.reflected_red + Color.reflected_green + Color.reflected_blue) / 3 << " - " << Light4.reflected << " - " << Ultrasonic2.cm << "\n"; // Print data gained from sensors
			if(Ultrasonic2.cm < 10 && Ultrasonic2.cm > 0.1){ // Compare data from sensor with predetermined values
				distance = Ultrasonic2.cm; // Assign gained data to variable for later use
				command = 1; // Distance
				break;
			}
			else if(((Color.reflected_red + Color.reflected_green + Color.reflected_blue) / 3 < 300) != (Light4.reflected > 2300)){ // If either the right or left sensor sees the black line
				color = (Color.reflected_red + Color.reflected_green + Color.reflected_blue) / 3;
				light = Light4.reflected;
				command = 2;	// Left and right
				break;
			}
			else if(((Color.reflected_red + Color.reflected_green + Color.reflected_blue) / 3 < 300) && (Light4.reflected > 2300)){ // If both the right and left sensors see a black line
				color = (Color.reflected_red + Color.reflected_green + Color.reflected_blue) / 3;
				light = Light4.reflected;
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
			BP.set_motor_power(PORT_B, 0);
			BP.set_motor_power(PORT_C, 0);
			usleep(1000000);
			
		}
		else if(command == 1){
			usleep(1000000);
			//distance < 7 && distance > 1 && 
			BP.set_motor_power(PORT_B, 0);
			BP.set_motor_power(PORT_C, 0);
			BP.get_sensor(PORT_1, &Color);
			color_red = Color.reflected_red;
			color_blue = Color.reflected_blue;
			color_green = Color.reflected_green;
			cout << color_red << "---" << color_green << "---" << color_blue << "\n";
		
			if(sugar > 0 && color_red > 440 && color_red < 480 && color_green > 410 && color_green < 450 && color_blue > 410 && color_blue < 450){  // Dit is voor een oranje beker 1 klontje
				BP.set_motor_power(PORT_D, 20);
				usleep(1400000);
				BP.set_motor_power(PORT_D, 0);
				sugar--;
				cout << "orange\n";
			}
			else if(sugar > 2 && color_red > 280 && color_red < 340 && color_green > 280 && color_green < 340 && color_blue > 300 && color_blue < 340){ //Dit is voor een blauwe beker 3 klontjes
				BP.set_motor_power(PORT_D, 20);
				usleep(5000000);
				//3900000
				BP.set_motor_power(PORT_D, 0);
				//BP.set_motor_power(PORT_B, 25);
				//BP.set_motor_power(PORT_C, 25);
				//usleep(400000);
				//BP.set_motor_power(PORT_B, 0);
				//BP.set_motor_power(PORT_C, 0);
				sugar-= 3;
				cout << "blue\n";
			}
			else if(sugar > 1 && color_red > 350 && color_red < 410 && color_green > 350 && color_green < 410 && color_blue > 350 && color_blue < 410){ //Dit is voor een groen beker 2 klontjes
				BP.set_motor_power(PORT_D, 30);
				usleep(1900000);
				BP.set_motor_power(PORT_D, 0);
				sugar-= 2;
				cout << "green\n";
			}
			usleep(500000);
		}
		BP.set_motor_power(PORT_B, 25);
		BP.set_motor_power(PORT_C, 25);
		//usleep(400000);

	}
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}