/*
 * server.c
 * receives msgs (pulses) from dataGen.c
 * server terminates when dataGen disconnects
 *
 * sends http requests to wearable device web server
 * API Doc -> https://absorbed-bone-267.notion.site/Wearable-Web-Server-API-f3e05f98a6e441269ec196b2640cb77e
 *
 */
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>


#include "defs.h"

#define PORT				80
#define WEBSITE_HOST		"18.209.255.145"
#define PAYLOAD_SIZE		256
#define MESSAGE_BUFFER_SIZE	1024
#define DEBUG				1						//todo

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
					payload = generatePayload(msg.pulse.code, "heartRate", msg.pulse.value.sival_int);
					makePostRequest(0, "heart-rate", payload);
					break;
				case BLOOD_PRESSURE_PULSE_CODE:
					//todo possible to have two ints produced for systolic and diastolic?
					payload = generatePayload(msg.pulse.code, "bloodPressure", msg.pulse.value.sival_int);
					makePostRequest(0, "blood-pressure", payload);
					break;
				case BODY_TEMPERATURE_PULSE_CODE:
					payload = generatePayload(msg.pulse.code, "bodyTemperature", msg.pulse.value.sival_int);
					makePostRequest(0, "body-temperature", payload);
					break;
				case STEP_COUNT_PULSE_CODE:
					payload = generatePayload(msg.pulse.code, "stepCount", msg.pulse.value.sival_int);
					makePostRequest(0, "step-count", payload);
					break;
				case GPS_PULSE_CODE:
					//todo no api route
					printf("Received gps data: %d\n", msg.pulse.value.sival_int);
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
	if (DEBUG == 1)
	{
		printf("Received %s data: %d\n", dataType, value);
	}

	char* payload = (char*) malloc(PAYLOAD_SIZE);
	long timeStamp = time(NULL);

	/* Build JSON payload */
	snprintf(payload, PAYLOAD_SIZE,
			"{\r\n"
			"  \"timestamp\":%ld,\r\n"
			"  \"%s\":%d\r\n"
			"}", timeStamp, dataType, value);

	return payload;
}

int makePostRequest(int deviceID, char* datatype, char* payload)
{
	char 				*host = WEBSITE_HOST;
	char				*userID = "638e81df7353cb6eced3875c";
	struct hostent 		*server;
	struct sockaddr_in 	serv_addr;
	int 				sockfd;
	char 				message[MESSAGE_BUFFER_SIZE], response[MESSAGE_BUFFER_SIZE];

	if (DEBUG == 2)	//we want to send to httpbin.org for testing
	{
		snprintf(message, sizeof(message),
				"POST /post HTTP/1.0\r\n"
				"Host: httpbin.org\r\n"
				"Content-Type: application/json\r\n"
				"Content-Length: %d\r\n\r\n"
				"%s\r\n", (unsigned int) strlen(payload), payload);
	}
	else
	{
		/* Build http request */
		snprintf(message, sizeof(message),
			"POST /api/users/%s/biometrics/%s HTTP/1.0\r\n"
			"Host: %s\r\n"
			"Content-type: application/json\r\n"
			"Content-length: %d\r\n\r\n"
			"%s\r\n", userID, datatype, host, (unsigned int) strlen(payload), payload);
	}

	if (DEBUG >= 1)
	{
		printf("Http request: \n%s\n", message);
	}

	/* Create socket file descriptor */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("Error: failed to create socket.");
		exit(EXIT_FAILURE);
	}

	/* Look up host ip */
	server = gethostbyname(host);
	if (server == NULL)
	{
		perror("Error: failed to get host.");
		exit(EXIT_FAILURE);
	}

	/* Fill in structure */
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
	memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

	/* Connect the socket */
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		perror("Error: failed to connect.");
		exit(EXIT_FAILURE);
	}

	/* Send http request */
	if (write(sockfd, message, strlen(message)) < 0)
	{
		perror("Error: failed to send http request.");
		exit(EXIT_FAILURE);
	}

	/* Read response */
	if (read(sockfd, response, sizeof(response)) < 0)
	{
		perror("Error: failed to read http response.");
		exit(EXIT_FAILURE);
	}

	if (DEBUG >= 1)
	{
		printf("Response: \n%s\n", response);
	}

	/* Clean up */
	close(sockfd);
	free(payload);

	return 0;
}
