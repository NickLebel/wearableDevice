/*
 * server.c
 * receives msgs (pulses) from dataGen.c
 * server terminates when dataGen disconnects
 *
 * sends http requests to wearable device web server
 * API Doc -> https://absorbed-bone-267.notion.site/Wearable-Web-Server-API-f3e05f98a6e441269ec196b2640cb77e
 *
 * TODO: replace prints in switch (msg.pulse.code) with appropriate posts -> https://www.w3schools.blog/send-http-request-in-c
 */
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/time.h>
#include <string.h>
#include <curl/curl.h>

#include "defs.h"

#define WEBSITE_URL			"https://mywebsite.com"

char* generatePayload(_Int8t pulseCode, char* dataType, int value);
int makePostRequest(int deviceID, char* datatype, char* payload);

int main(int argc, char **argv)
{
	typedef union
	{
		struct _pulse pulse;
	} myMessage_t;

	name_attach_t *attach;
	myMessage_t msg;
	int 	rcvid;
	int 	heartRate, bloodPressure, bodyTemperature, stepCount, GPSData;
	char*	payload;

	/* Create a local name (/dev/name/local/...) */
	if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {return EXIT_FAILURE;}

	for(;;)
	{
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		//received pulse
		if (rcvid == 0)
		{
			//handle pulse codes received from dataGen
			switch(msg.pulse.code)
			{
				//TODO: replace prints with API requests -> https://www.w3schools.blog/send-http-request-in-c
				//if server cannot keep up with incoming / outgoing requests, may need to implement threads to send outgoing requests
				//no issues (so far) between dataGen -> server with round robin + priority threads on data side
				case _PULSE_CODE_DISCONNECT:
					printf("\n\n<>Data generation terminated, closing device server.<>\n\n");
					return 0;
				case HEART_RATE_PULSE_CODE:
					heartRate = msg.pulse.value.sival_int;
					printf("received heart rate data = %d\n", heartRate);
					payload = generatePayload(msg.pulse.code, "heartRate", heartRate);
					makePostRequest(0, "heartRate", payload);
					break;
				case BLOOD_PRESSURE_PULSE_CODE:
					//possible to have two ints produced for systolic and diastolic?
					bloodPressure = msg.pulse.value.sival_int;
					printf("received blood pressure data = %d\n", bloodPressure);
					payload = generatePayload(msg.pulse.code, "bloodPressure", bloodPressure);
					makePostRequest(0, "bloodPressure", payload);
					break;
				case BODY_TEMPERATURE_PULSE_CODE:
					bodyTemperature = msg.pulse.value.sival_int;
					printf("received body temp data = %d\n", bodyTemperature);
					payload = generatePayload(msg.pulse.code, "bodyTemperature", bodyTemperature);
					makePostRequest(0, "bodyTemperature", payload);
					break;
				case STEP_COUNT_PULSE_CODE:
					stepCount = msg.pulse.value.sival_int;
					printf("received step count data = %d\n", stepCount);
					payload = generatePayload(msg.pulse.code, "stepCount", stepCount);
					makePostRequest(0, "stepCount", payload);
					break;
				case GPS_PULSE_CODE:
					//no api route
					printf("received gps data = %d\n", msg.pulse.value.sival_int);
					break;
				default:
					printf("Pulse code = %d | value = %d\n\n", msg.pulse.code, msg.pulse.value.sival_int);
			}
		}

		//something other than a pulse was received (should not occur)
		else
		{
			printf("Something other than a pulse was received, server terminating.\n\n");
			return EXIT_FAILURE;
		}

	}
	return 0;
}

char* generatePayload(_Int8t pulseCode, char* dataType, int value)
{
	char* payload = (char*) malloc(256);	//probably malloc this better later
	long timeStamp = time(NULL);
	sprintf(payload, "{\"timestamp\":%ld, \"%s\":%d}", timeStamp, dataType, value);
	return payload;
}

int makePostRequest(int deviceID, char* datatype, char* payload)
{
	printf("Sending payload: %s\n", payload);
	/*CURL 		*curl;
	CURLcode 	res;
	char*		website;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if(curl == NULL) {
		return 128;
	}

	sprintf(website, "%s/%d/%s", WEBSITE_URL, deviceID, datatype);

	struct curl_slist *headers = NULL;
	curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_URL, website);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
	res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
    	fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
	curl_global_cleanup();*/
	free(payload);
	return 0;
}
