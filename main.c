/**
 * This file intends to accomplish tracking and avoiding obstacle using 4WD Car designed by HuiJing
 * Jump cap definations are in base_defination.h
 * 
 * @platform	HJ-4WD/HJ-E/HL-1 + Keil U4 + STC89C52(11.0592Hz)
 * @author		H.Zhou
 * @date		2015.12.20
 */

#include <base_defination.h>
#include <base_function.h>
#include <AT89X52.H>

void avoid_obstacle();
void fire_fight();

/**
 * Main Function
 **/
void main() {
	//Initialize
	init_pins();
	init_PWM_timer();
	
	delay(2000); //The first measure of ultrasonic detecter are not reliable, delay to avoid it.
	while (1) {
		while(distance > 100) { //If distance is less than 10cm, start to avoid obstacle.
			if (left_trace == 0) {
				if (right_trace == 0) {
					pwm_left_moto = trace_speed;
					pwm_right_moto = trace_speed;
				}
				else {
					pwm_left_moto = 5;
					pwm_right_moto = 10;
				}
			}
			else if (right_trace == 0){
				pwm_left_moto = 10;
				pwm_right_moto = 5;
			}
			else {
				pwm_left_moto = 10;
				pwm_right_moto = 10;
			}
			if (stop_time > 500) { //Check if the stop more than 1s. If so, start fire fight.
				P2_6 = 0; //Alert for one seconed. Announcing the start of blow-out-fire process. 
				delay (7142);
				P2_6 = 1;
				fire_fight();
				while(1);
			}
		}
		pwm_left_moto = pwm_right_moto = 10;
		delay(2000); //Prevent some sudden error of ultrasonic detector. Redetect distance again.
		if (distance <= 100) avoid_obstacle();
	}
}

/**
 * Function to avoid obstacle
 **/
void avoid_obstacle() {
	int l1, l2;
	gear_turn(28); //Check left
	delay(4000);
	l1 = distance;
	gear_turn(8); //Check right
	delay(6000);
	l2 = distance;
	if (l1>l2) { //Which direction is farther
		gear_turn(8); //Gear turn right
		pwm_left_moto = 10; //Moto turn left
		pwm_right_moto = 2;
		delay(4000);
		while((!left_trace)&&(!right_trace)) { //Not detect the line
			if (right_avoid == 1) { //Judgement of infrared detector
				pwm_left_moto = 6;
				pwm_right_moto = 10;
			}
			else {
				pwm_left_moto = 6;
				pwm_right_moto = 6;
			}
			while (distance<150) { //Judgement of Ultrosonic detector
				pwm_left_moto = 8;
				pwm_right_moto = 6;
			}
		}
		while (right_trace) { //When right tracer reach the line, it need to move off line before getting back to tracing
			pwm_left_moto = 6;
			pwm_right_moto = 6;
		}
	}
	else {
		gear_turn(28); //Gear turn left
		pwm_left_moto = 2; //Moto turn left
		pwm_right_moto = 10;
		delay(4000);
		while((!left_trace)&&(!right_trace)) { //Not detect the line
			if (left_avoid == 1) { //Judgement of infrared detector
				pwm_left_moto = 10;
				pwm_right_moto = 6;
			}
			else {
				pwm_left_moto = 6;
				pwm_right_moto = 6;
			}
			while (distance<150) { //Judgement of Ultrosonic detector
				pwm_left_moto = 6;
				pwm_right_moto = 8;
			}
		}
		while(left_trace) { //When lefft tracer reach the line, it need to move off line before getting back to tracing.
			pwm_left_moto = 6;
			pwm_right_moto = 6;
		}
		
	}
	gear_turn(18); //Gear return middle
	pwm_left_moto = 10; //Stop moto
	pwm_right_moto = 10;
	delay(2000);
	trace_speed = 8; //Start at low speed
	stop_time = 0; //Don't judge this stop as the beginning of fire fight
}

/**
 * This is the function of fire fight
 * The car shall circle around to find where the fire source is, and put it out
 **/
void fire_fight() {
	pwm_left_moto = 6; //Circle around to find the fire
	pwm_right_moto = 10;
	while (!fire_status);
	pwm_left_moto = 10; //Stop for a moment
	pwm_right_moto = 10;
	delay (2000);
	while (distance>70) { //Near the fire, then blow out the fire
		if (fire_status){
			pwm_left_moto = 8;
			pwm_right_moto = 8;
		}
		else { //Direction recalculation
			gear_turn(20); //little angle to the left
			delay(2000);
			if (fire_status) {
				gear_turn(18);
				delay(2000);
				pwm_left_moto = 10;
				pwm_right_moto = 4;
				while (!fire_status);
			}
			else {
				gear_turn(16); //little angle to the right
				delay(2000);
				if (fire_status) {
					gear_turn(18);
					delay(2000);
					pwm_left_moto = 4;
					pwm_right_moto = 10;
					while(!fire_status);
				}
				else{ //There is a problem, fire disappeared.
					pwm_left_moto = 10;
					pwm_right_moto = 10;
					P2_6 = 0; //Alert for one seconed. 
					delay(7142);
					P2_6 = 1;
					while(1);
				}
			}
		}
	}
	pwm_left_moto = 10; //Stop in front of the fire 
	pwm_right_moto = 10;
	P3_1 = 0; //Start the fan
	while(fire_status);
	delay (60000); //Wait long enough to ensure the fire is out
	P3_1 = 1;
	P2_6 = 0; //ALert for two seconed, representing the end of the program.
	delay(14285);
	P2_6 = 1;
}