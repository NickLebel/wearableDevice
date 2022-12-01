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

#include "defs.h"

int main(int argc, char **argv)
{
	typedef union
	{
		struct _pulse pulse;
	} myMessage_t;

	name_attach_t *attach;
	myMessage_t msg;
	int rcvid;


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
					printf("received heart rate data = %d\n", msg.pulse.value.sival_int);
					break;
				case BLOOD_PRESSURE_PULSE_CODE:
					printf("received blood pressure data = %d\n", msg.pulse.value.sival_int);
					break;
				case BODY_TEMPERATURE_PULSE_CODE:
					printf("received body temp data = %d\n", msg.pulse.value.sival_int);
					break;
				case STEP_COUNT_PULSE_CODE:
					printf("received step count data = %d\n", msg.pulse.value.sival_int);
					break;
				case GPS_PULSE_CODE:
					printf("receieved gps data = %d\n", msg.pulse.value.sival_int);
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
