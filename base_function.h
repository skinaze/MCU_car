/**
 * This file include the basic functions used for 4WD Car designed by HuiJing
 * 
 * Jump cap requestment:
 * Moto: Use jump caps on all the pins of J6-1 and J6-2, release jump caps on pins of J6-3.  
 * Digitial tube: Use jump cap on J13. 
 * Infrared ray senser: Use jump caps on J5. 
 * 
 * @platform	HJ-4WD/HJ-E/HL-1 + Keil U4 + STC89C52(11.0592Hz)
 * @author		H.Zhou
 * @date		2015.12.20
 */

#include <base_defination.h>
#include <AT89X52.H>

#ifndef _Base_Functions_
#define _Base_Functions_

/**
 * The initial function of timer and interrupt
 * Gear pwm timer use timer 0
 * Moto pwm timer use timer 2
 * Digitial tube use timer 2
 * Pwm timer create the pwm for moto, gear and digitial tube
 * Ultrasonic detector use timer 1 and int 1
 **/
void init_PWM_timer() {
	//Initial of T0/T1
	TMOD ^= 0x92;
	IE ^= 0xA6;
	IP ^=0x02;
	TH0 = 164;
	TL0 = 164;
	TH1 = 0;
	TL1 = 0;
	TCON ^= 0x54;
	//Initial of T2
	T2MOD = 0x00;
	T2CON = 0x04;
	RCAP2H = TH2 = 0xF8;
	RCAP2L = TL2 = 0xCD;
}

/**
 * The initial function of pins
 **/
void init_pins() {
	//Moto initialize
	P1_0 = P1_2 = P1_4 = P1_6 = 1;
	//Digitial tube initialize
	P2_0 = P2_1 = P2_2 = P2_3 = 1;
	//Gear initialize
	P2_7 = 0;
	//Ultrasonic detector initialize
	P2_5 = 0;
}

/**
 * Dynamic Digitial tube driver
 **/
void digitial_tube(){
	switch (current_tube) {
		case 0:
		P2_3 = 1;
		P0 = discode[dt_num1];
		P2_0 = 0;
		break;
		
		case 1:
		P2_0 = 1;
		P0 = discode[dt_num2];
		P2_1 = 0;
		break;
		
		case 2:
		P2_1 = 1;
		P0 = discode[dt_num3];
		P2_2 = 0;
		break;
		
		case 3:
		P2_2 = 1;
		P0 = discode[dt_num4];
		P2_3 = 0;
		break;
	}
	current_tube++;
	if (current_tube == 4) current_tube = 0;
}

/**
 * Gear driver
 **/
void gear() {
	if (pwm_gear_current<pwm_gear) P2_7 = 1;
	else P2_7 = 0;
	pwm_gear_current++;
	if (pwm_gear_current == 200) pwm_gear_current =0;
}

/**
 * Start gear turning
 * @pram	i		The angle for gear to turn.
 **/
void gear_turn(short i) {
	pwm_gear = i;
}

/**
 * Moto driver
 * The pwm of moto forward and backward are different.
 * When moto move forward, the longer the high voltage level, the faster.
 * When moto move backward, the longer the low voltage level, the faster.
 **/
void moto() {
	if (pwm_left_moto_current<pwm_left_moto) {
		if (left_moto) {P1_5 = 1; P1_7 = 1;}
		else {P1_5 = 0; P1_7 = 0;}
	}
	else {
		if (left_moto) {P1_5 = 0; P1_7 = 0;}
		else {P1_5 = 1; P1_7 = 1;}
	}
	pwm_left_moto_current++;
	if (pwm_left_moto_current == 10) pwm_left_moto_current =0;
	
	if (pwm_right_moto_current<pwm_right_moto) {
		if (right_moto) {P1_1 = 1; P1_3 = 1;}
		else {P1_1 = 0; P1_3 = 0;}
	}
	else {
		if (right_moto) {P1_1 = 0; P1_3 = 0;}
		else {P1_1 = 1; P1_3 = 1;}
	}
	pwm_right_moto_current++;
	if (pwm_right_moto_current == 10) pwm_right_moto_current =0;
}

/**
 * The interrupt function of timer0
 **/
void int_timer0() interrupt 1 using 1 {
	TF0 = 0;
	gear();
}

/**
 * The interrupt function of timer2
 **/
void int_timer2() interrupt 5 using 2 {
	TF2 = 0;
	if ((uw_counter++)==10) {uw_status = 0; P2_5 = 1; uw_counter = 0;} //Start a ultrasonic detect every 20ms;
	digitial_tube();
	P2_5 = 0; //The high voltage level should remain more than 10us, thus call other function before set P2.5 low.
	moto();
	//Trace speed turn default slowly
	if (trace_speed>default_trace_speed) {if (trace_speed_count++ == 250) {trace_speed_count = 0; trace_speed--;} }
	else if (trace_speed<default_trace_speed) {if (trace_speed_count++ == 250) {trace_speed_count = 0; trace_speed++;} }
	//Count stop time
	if ((pwm_left_moto == 10)&&(pwm_right_moto == 10)) stop_time++; else stop_time = 0;
	//Count fire time
	if (!P3_0) fire_time++; else {fire_status = 0; fire_time = 0;}
	if (fire_time == 10) fire_status = 1;
}

/**
 * The interrupt function of int1
 * This interrupt intends to store the distance counted
 **/
void int_int1() interrupt 2 using 3 {
	long i;
	i = TH1*256+TL1;
	i *= 344;
	distance = i/2000;
	//Rewite Tube
	i = distance;
	dt_num1 = i/1000;
	i %= 1000;
	dt_num2 = i/100;
	i %= 100;
	dt_num3 = i/10;
	dt_num4 = i%10;
	//Clear counted number in t1;
	TH1 = 0;
	TL1 = 0;
	uw_status = 1;
}

/**
 * Deyley function
 * @pram	time		The time to delay(135~140us/num)
 **/
void delay(long time) {
	while ((time--)>0);
}
#endif