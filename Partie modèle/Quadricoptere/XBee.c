#include "XBee.h"



void sendData ( DATA_COMM *data )
{
	char bufSend[]="T000R000L000A00000B000S000";
	sprintf(bufSend, "T%03dR%03dL%03dA%05dB%06.2fS%06.2f", data->tangage, data->roulis, data->lacet, data->altitude, data->battery, data->signal);
	sdWrite(&SD2, (uint8_t*)bufSend, strlen(bufSend));
}

DATA_COMM* rcvData()
{
	DATA_COMM *data = NULL;
	
	int i = 0;
	int j = 0;
	
	char bufRead[]="T000R000L000A00000B000S000";
	char bufFinalString[]="T000R000L000A00000B000S000";
	
	sdRead(&SD2, (uint8_t*)bufRead, strlen(bufRead));
	
	while(bufRead[i] != 'T')
	{
		i++;
	}
	
	for ( j = 0; j<strlen(bufRead); j++)
	{
		bufFinalString[j] = bufRead[(j+i)%strlen(bufRead)];
		i++;
	}
	
	scanf(bufFinalString, "T%03dR%03dL%03dA%05dB%06.2fS%06.2f", data->tangage, data->roulis, data->lacet, data->altitude, data->battery, data->signal);

	return data;
}

float getSignal()
{
	//ToDo
	//récupérer la PWM du XBee
	return 0;
}
