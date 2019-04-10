#include "BrickPi3.cpp" // For BrickPi3
#include <stdio.h>      // For printf
#include <stdlib.h>	// For system()
#include <unistd.h>     // For usleep
#include <signal.h>     // For catching exit signals
#include <iostream>     // For using cout
#include <time.h>	// For time
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

BrickPi3 BP;

void exit_signal_handler(int signo);

int main(){
	signal(SIGINT, exit_signal_handler);	// Register the exit function for Ctrl+C

	BP.detect(); 	// Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_motor_limits(PORT_B, 60, 0); // Set limits for motors
	BP.set_motor_limits(PORT_C, 60, 0);
	BP.set_motor_limits(PORT_D, 50, 200);
	// Start values for sensors
	int color = 300;
	int light = 2600;
	int distance = 200;
	int command = 0;
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
		else if(command == 3){ // Stop at crossroad
			BP.set_motor_power(PORT_B, 0);
			BP.set_motor_power(PORT_C, 0);
			break;
			
		}
		else if(command == 1){
			usleep(1300000); // Continue driving for a moment to position robot in front of cup
			BP.set_motor_power(PORT_B, 0);
			BP.set_motor_power(PORT_C, 0);
			system("fswebcam --no-banner /home/pi/Desktop/Project/image.jpg"); // Take a picture with the camera
			Mat img = imread("/home/pi/Desktop/Project/image.jpg"); // Create variable with picture data
			int redval = 0; // Create variables for later use
			int blueval = 0;
			int greenval = 0;
			for(unsigned int x = 0; x < img.size().width; x++){ // Loop through all pixels in image by looping through all rows and columns
				for(unsigned int y = 0; y < img.size().height; y++){
					Vec3b intensity = img.at<Vec3b>(y, x); // Select pixel at certain coördinate
					int blue = intensity.val[0]; // Get color values from selected pixel
					int green = intensity.val[1];
					int red = intensity.val[2];
					cout << blue << " blue " << green << " green " << red << " red\n";
					if(red > blue && red > green){
						redval++;
					} else if(blue > red && blue > green){
						blueval++;
					} else if(green > red && green > blue){
						greenval++;
					}
				}
			}
			if(redval > (6 * greenval) && redval > blueval){
				cout << "Color is Red: " << blueval << " blue " << greenval << " green " << redval << " red\n";
				BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
				BP.set_motor_position(PORT_D, 360);
				usleep(2000000);
				sugar--;
			} else if(blueval > greenval && blueval > redval){
				cout << "Color is Blue: " << blueval << " blue " << greenval << " green " << redval << " red\n";
				BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
				BP.set_motor_position(PORT_D, 360);
				usleep(2000000);
				BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
				BP.set_motor_position(PORT_D, 360);
				usleep(2000000);
				sugar -= 2;
			} else if((6 * greenval) > redval && greenval > blueval){
				cout << "Color is Green: " << blueval << " blue " << greenval << " green " << redval << " red\n";
				BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
				BP.set_motor_position(PORT_D, 360);
				usleep(2000000);
				BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
				BP.set_motor_position(PORT_D, 360);
				usleep(2000000);
				BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
				BP.set_motor_position(PORT_D, 360);
				usleep(2000000);
				sugar -= 3;
			} else {
				cout << "Unrecognized color or no balls/sugar cubes left!!\n";
			}
			BP.set_motor_power(PORT_B, 25);
			BP.set_motor_power(PORT_C, 25);
			usleep(2000000);
		}
	}
	BP.reset_all();
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}