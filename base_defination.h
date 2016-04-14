/**
 * This file include the basic definations used for 4WD Car designed by HuiJing
 * 
 * @platform	HJ-4WD/HJ-E/HL-1 + Keil U4 + STC89C52(11.0592Hz)
 * @author		H.Zhou
 * @date		2015.12.20
 */

#ifndef _Base_Defination_
#define _Base_Defination_

//Moto status
bit left_moto = 1; //1 represent forward, 0 represent backward
bit right_moto = 1; //1 represent forward, 0 represent backward
#define left_moto_forward {left_moto = 1; P1_4 = 1; P1_6 = 1;}
#define left_moto_backward {left_moto = 0; P1_4 = 0; P1_6 = 0;}
#define right_moto_forward {right_moto = 1; P1_0 = 1; P1_2 = 1;}
#define right_moto_backward {right_moto = 0; P1_0 = 0; P1_2 = 0;}

//Ultrasonic wave detector status
bit uw_status = 0; //1 represents ready, 0 represents detecting
short uw_counter = 0; //Count 20ms
int distance = 0; //The deistance calculated

//PWM defination
short pwm_left_moto = 10;
short pwm_right_moto = 10;
short pwm_gear = 18;
short pwm_left_moto_current = 0; //Count 20ms
short pwm_right_moto_current = 0; //COunt 20ms
short pwm_gear_current = 0; //Count 20ms
short trace_speed = 6;
#define default_trace_speed 6
short trace_speed_count = 0; //Count 500ms

//Digitial tube number defination
unsigned char const discode[] ={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xff};
short current_tube = 0; //Dynamic digitial tube. The number of tube that is being displayed.
short dt_num1 = 0;
short dt_num2 = 0;
short dt_num3 = 0;
short dt_num4 = 0;

//Infrared ray senser
#define left_trace P3_6	 //P3_5 connect left tracing infrared ray senser
#define right_trace P3_5	 //P3_6 connect right tracing infrared ray senser

#define left_avoid P3_7	 //P3_4 connect left avoiding obstacle infrared ray senser
#define right_avoid P3_4	 //P3_7 connect right avoiding obstacle infrared ray senser

//Fire fight
int stop_time; //If stop more than one second, initialize fire fight process
short fire_time; //If fire detector detect fire for more than 20ms, confirm it's a relable fire source
bit fire_status; //1 represents detected, 0 represents not detected

#endif