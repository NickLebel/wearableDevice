/*
 * defs.h
 */
#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <sys/neutrino.h>

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
#define GPS_SLEEP					60

/*
 * RNG ranges (data generated is between min and max)
 */
#define HEART_RATE_MIN				30
#define HEART_RATE_MAX				240
#define BLOOD_PRESSURE_MIN			90
#define BLOOD_PRESSURE_MAX			200
#define BODY_TEMPERATURE_MIN		90
#define BODY_TEMPERATURE_MAX		105
#define STEP_COUNT_MIN				400
#define STEP_COUNT_MAX				1000
#define GPS_MIN						0
#define GPS_MAX						1

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

#endif //_DEFS_H
