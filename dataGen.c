/*
 * dataGen.c
 * produces simulated data for the wearable device
 * data is produced by Round robin priority threads
 * most data generation can be easily modified through the definitions (defs.h)
 * frequencies are in seconds
 * data generated may not be completely accurate for the purpose of this project, data assumes moderate ranges and excludes outliers
 */

#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

/*
 * DEFS.H CONTAINS ALL DEFINES TO MODIFY DATA GENERATION
 */
#include "defs.h"

/*
 * Generator Functions
 */
void *generateHeartRate();
void *generateBloodPressure();
void *generateBodyTemperature();
void *generateStepCount();
void *generateGPS();
int   generateRandomNumber(int min, int max);


int main(int argc, char **argv)
{
	pthread_attr_t threads[NUMTHREADS];
	pthread_t      thr;
	struct sched_param  params;
	int i, ret, server_coid;

	/* seed random number generator for generateRandomNumber() */
	srand(time(0));

	/* initialize threads */
	for(i=0; i<NUMTHREADS; ++i)
	{
		pthread_attr_init(&threads[i]);
	}

	/* set threads to explicit schedule */
	for(i=0; i<NUMTHREADS; ++i)
	{
		ret = pthread_attr_setinheritsched(&threads[i], PTHREAD_EXPLICIT_SCHED);
		if(ret != 0) {printf("pthread_attr_setinheritsched() failed %d \n", errno); return 1;}
	}

	/* set threads to round robin */
	for(i=0; i<NUMTHREADS; ++i)
	{
		ret = pthread_attr_setschedpolicy(&threads[i], SCHED_RR);
		if(ret != 0){printf("pthread_attr_setschedpolicy() failed %d %d\n", ret, errno); return 1;}
	}

	/* setup sched_param structure */
	params.sched_ss_low_priority = MY_LOW_PRIORITY;
	memcpy(&params.sched_ss_init_budget, &MY_INIT_BUDGET, sizeof(MY_INIT_BUDGET));
	memcpy(&params.sched_ss_repl_period, &MY_REPL_PERIOD, sizeof(MY_REPL_PERIOD));
	params.sched_ss_max_repl = MY_MAX_REPL;


	/*
	 * set sched_param priorities and set thread sched_param
	 * 0 = heart rate
	 * 1 = blood pressure
	 * 2 = body temperature
	 * 3 = step count
	 * 4 = GPS
	 */
	/* heart rate thread */
	params.sched_priority = HEART_RATE_PRIORITY;
	ret = pthread_attr_setschedparam(&threads[0], &params);
	if(ret != 0){printf("pthread_attr_setschedparam() failed %d \n", errno); return 1;}

	/* blood pressure thread */
	params.sched_priority = BLOOD_PRESSURE_PRIORITY;
	ret = pthread_attr_setschedparam(&threads[1], &params);
	if(ret != 0){printf("pthread_attr_setschedparam() failed %d \n", errno); return 1;}

	/* body temperature thread */
	params.sched_priority = BODY_TEMPERATURE_PRIORITY;
	ret = pthread_attr_setschedparam(&threads[2], &params);
	if(ret != 0){printf("pthread_attr_setschedparam() failed %d \n", errno); return 1;}

	/* body temperature thread */
	params.sched_priority = STEP_COUNT_PRIORITY;
	ret = pthread_attr_setschedparam(&threads[3], &params);
	if(ret != 0){printf("pthread_attr_setschedparam() failed %d \n", errno); return 1;}

	/* GPS thread */
	params.sched_priority = GPS_PRIORITY;
	ret = pthread_attr_setschedparam(&threads[4], &params);
	if(ret != 0){printf("pthread_attr_setschedparam() failed %d \n", errno); return 1;}


	/* setup connection to server */
	if ((server_coid = name_open(ATTACH_POINT, 0)) == -1) {return EXIT_FAILURE;}

	/*
	 * start the threads
	 * 0 = heart rate
	 * 1 = blood pressure
	 * 2 = body temperature
	 * 3 = step count
	 *  4 = GPS
	 */

	/* heart thread (also get p_thread for join)*/
	ret = pthread_create(&thr, &threads[0], &generateHeartRate, server_coid);
	if(ret != 0){printf("pthread_create() failed %d \n", errno); return 1;}

	/* blood pressure thread */
	ret = pthread_create(NULL, &threads[1], &generateBloodPressure, server_coid);
	if(ret != 0){printf("pthread_create() failed %d \n", errno); return 1;}

	/* body temperature thread */
	ret = pthread_create(NULL, &threads[2], &generateBodyTemperature, server_coid);
	if(ret != 0){printf("pthread_create() failed %d \n", errno); return 1;}

	/* step count thread */
	ret = pthread_create(NULL, &threads[3], &generateStepCount, server_coid);
	if(ret != 0){printf("pthread_create() failed %d \n", errno); return 1;}

	/* GPS thread */
	ret = pthread_create(NULL, &threads[4], &generateGPS, server_coid);
	if(ret != 0){printf("pthread_create() failed %d \n", errno); return 1;}

	/*
	 * keep threads looping until manual termination
	 * server will also terminate once data generator is terminated
	 */
	pthread_join(thr, NULL);
	return 0;
}

/*
 * generateHeartRate()
 * generates heart rate data
 * data range is between HEART_RATE_MIN and HEART_RATE_MAX
 * generated heart rate increments of decrements by [-2, 2] (may cause range to be above or below MIN / MAX)
 * every 20 seconds, a new rate is generated
 * frequency = HEART_RATE_SLEEP
 * MsgSendPulse Priority = -1 (inherits the threads priority)
 */
void *generateHeartRate(int server_coid)
{
	int data;
	data = generateRandomNumber(HEART_RATE_MIN, HEART_RATE_MAX);
	for(int i=0;;++i)
	{
		//new heart rate generated every 20 sec
		if(i == 20)
		{
			data = generateRandomNumber(HEART_RATE_MIN, HEART_RATE_MAX);
			i = 0;
		}

		//randomly increase or decrease heart rate
		data += generateRandomNumber(-2, 2);
		printf("Heart rate = %d\n", data);

		if(MsgSendPulse(server_coid, -1, HEART_RATE_PULSE_CODE, data) == -1){printf("Pulse failed to send from heart rate");}
		sleep(HEART_RATE_SLEEP);
	}
	return 0;
}

/*
 * generateBloodPressure()
 * generates blood pressure data
 * data sent = systolic * 1000 + diastolic -> fits both into 1 int for pulse
 * server computes systolic with data/1000, diastolic with data%1000
 * frequency = BLOOD_PRESSURE_SLEEP
 */
void *generateBloodPressure(int server_coid)
{
	int data, systolic, diastolic;
	for(;;)
	{
		systolic  = generateRandomNumber(BLOOD_PRESSURE_SYSTOLIC_MIN, BLOOD_PRESSURE_SYSTOLIC_MAX);
		diastolic = generateRandomNumber(BLOOD_PRESSURE_DIASTOLIC_MIN, BLOOD_PRESSURE_DIASTOLIC_MAX);

		/* int manipulation to fit into pulse value */
		data = systolic * 1000 + diastolic;

		printf("Systolic blood pressure = %d\n", data/BLOOD_PRESSURE_INT_MANIP);
		printf("Diastolic blood pressure = %d\n", data%BLOOD_PRESSURE_INT_MANIP);

		if(MsgSendPulse(server_coid, -1, BLOOD_PRESSURE_PULSE_CODE, data) == -1){printf("Pulse failed to send from blood pressure");}
		sleep(BLOOD_PRESSURE_SLEEP);
	}
	return 0;
}

/*
 * generateBodyTempurate()
 * generates body temperature data
 * data range is between BODY_TEMPERATURE_MIN and BODY_TEMPERATURE_MAX
 * frequency = BODY_TEMPERATURE_SLEEP
 */
void *generateBodyTemperature(int server_coid)
{
	int data;
	for(;;)
	{
		data = generateRandomNumber(BODY_TEMPERATURE_MIN, BODY_TEMPERATURE_MAX);
		printf("Body temp = %d\n", data);
		if(MsgSendPulse(server_coid, -1, BODY_TEMPERATURE_PULSE_CODE, data) == -1){printf("Pulse failed to send from body temp");}
		sleep(BODY_TEMPERATURE_SLEEP);
	}
	return 0;
}

/*
 * generateStepCount()
 * generates step count data
 * data range is between BODY_TEMPERATURE_MIN and BODY_TEMPERATURE_MAX
 * frequency = BODY_TEMPERATURE_SLEEP
 */
void *generateStepCount(int server_coid)
{
	int data;
	for(;;)
	{
		data = generateRandomNumber(STEP_COUNT_MIN, STEP_COUNT_MAX);
		printf("Step count = %d\n", data);
		if(MsgSendPulse(server_coid, -1, STEP_COUNT_PULSE_CODE, data) == -1){printf("Pulse failed to send from step count");}
		sleep(STEP_COUNT_SLEEP);
	}
	return 0;
}

/*
 * generateGPS()
 * generates GPS data
 * data range is between GPS_MIN and GPS_MAX
 * data is longitude and latitude -> coordinates
 * data sent = longitude * GPS_INT_MANIP + latitude -> fits both into 1 int for pulse
 * server computes longitude with data/GPS_INT_MANIP, latitude with data%GPS_INT_MANIP
 * frequency = GPS_SLEEP
 */
void *generateGPS(int server_coid)
{
	int data, longitude, latitude;
	for(;;)
	{
		longitude = generateRandomNumber(GPS_MIN, GPS_MAX);
		latitude = generateRandomNumber(GPS_MIN, GPS_MAX);

		/* int manip */
		data = longitude * GPS_INT_MANIP + latitude;

		printf("GPS longitude data = %d\n", data/GPS_INT_MANIP);
		printf("GPS latitude data = %d\n", data%GPS_INT_MANIP);

		if(MsgSendPulse(server_coid, -1, GPS_PULSE_CODE, data) == -1){printf("Pulse failed to send from GPS");}
		sleep(GPS_SLEEP);
	}
	return 0;
}

/*
 * generateRandomNumber()
 * generates a random number in range of [min, max]
 * srand() seeded in main()
 */
int generateRandomNumber(int min, int max) {return (rand() % (max - min + 1) + min);}
