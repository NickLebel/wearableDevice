/*
 * defs.h
 * contains definitions used between dataGen.c and server.c
 */
#ifndef _DEFS_H_
#define _DEFS_H_

/*
 * attach point for server / dataGen communication
 */
#define ATTACH_POINT "wearableDeviceAttach"

/*
 * Number of data generating threads
 */
#define NUMTHREADS					5

/*
 * Priorities
 */
#define HEART_RATE_PRIORITY 		9
#define BLOOD_PRESSURE_PRIORITY 	8
#define BODY_TEMPERATURE_PRIORITY 	7
#define STEP_COUNT_PRIORITY			6
#define GPS_PRIORITY				5

/*
 * Frequencies (in seconds)
 */
#define HEART_RATE_SLEEP			1
#define BLOOD_PRESSURE_SLEEP 		5
#define BODY_TEMPERATURE_SLEEP		10
#define STEP_COUNT_SLEEP			10
#define GPS_SLEEP					30

/*
 * RNG ranges (data generated is between min and max)
 * Data sources:
 * heart rate    : https://www.healthline.com/health/dangerous-heart-rate#dangerous-heart-rate
 * Blood pressure: https://www.verywellhealth.com/systolic-and-diastolic-blood-pressure-1746075
 */
#define HEART_RATE_MIN					60
#define HEART_RATE_MAX					200
#define BLOOD_PRESSURE_SYSTOLIC_MIN		90
#define BLOOD_PRESSURE_SYSTOLIC_MAX		200
#define BLOOD_PRESSURE_DIASTOLIC_MIN	60
#define BLOOD_PRESSURE_DIASTOLIC_MAX	140
#define BODY_TEMPERATURE_MIN			95		//temp in degrees Fahrenheit
#define BODY_TEMPERATURE_MAX			105		//temp in degrees Fahrenheit
#define STEP_COUNT_MIN					0
#define STEP_COUNT_MAX					500
#define GPS_MIN							1
#define GPS_MAX							999

/*
 * Int manip for pulses
 */
#define BLOOD_PRESSURE_INT_MANIP		1000
#define GPS_INT_MANIP					10000

/*
 * Pulse codes
 */
#define HEART_RATE_PULSE_CODE		99
#define BLOOD_PRESSURE_PULSE_CODE	88
#define BODY_TEMPERATURE_PULSE_CODE	77
#define STEP_COUNT_PULSE_CODE		66
#define GPS_PULSE_CODE				55

/*
 * Schedule parameters setup
 */
struct timespec g_init_budget = { 5, 0 };
struct timespec g_repl_period = { 10, 0 };
#define MY_LOW_PRIORITY 1
#define MY_REPL_PERIOD g_repl_period
#define MY_INIT_BUDGET g_init_budget
#define MY_MAX_REPL 10

/*
 * Http requests
 */
#define PORT				80
#define WEBSITE_HOST		"18.209.255.145"
#define USER_ID				"638fd1616d83ee235428c93a"
#define PAYLOAD_SIZE		256
#define MESSAGE_BUFFER_SIZE	1024
#define DEBUG				1

#endif //_DEFS_H
