#include "test.h"



void test()
{
}

void sendData ( DATA_COMM *data )
{
	char bufSend[]="T000R000L000A00000B000S000";
	sprintf(bufSend, "T%03dR%03dL%03dA%05dB%07.2fS%07.2f", data->tangage, data->roulis, data->lacet, data->altitude, data->battery, data->signal);
	sdWrite(&SD2, (uint8_t*)bufSend, strlen(bufSend));
}
