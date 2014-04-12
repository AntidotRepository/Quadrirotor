#include "XBee.h"

// Récupération de la qualité du signal
icucnt_t last_width, last_period;

 SerialConfig uartCfg=
{
	 BAUDRATE,									/* Baudrate																	 				*/
	 0,											 		/* cr1 register values															 */
	 0,											 		/* cr2 register values															 */
	 0													/* cr3 register values															 */
};

//static void icuwidthcb(ICUDriver *icup) {

//  palSetPad(GPIOB, GPIOB_LED3);
//  last_width = icuGetWidth(icup);
//}

//static void icuperiodcb(ICUDriver *icup) {

//  palClearPad(GPIOB, GPIOB_LED3);
//  last_period = icuGetPeriod(icup);
//}

//static ICUConfig icucfg = {
//  ICU_INPUT_ACTIVE_HIGH,
//  10000,                                    /* 10kHz ICU clock frequency.   */
//  icuwidthcb,
//  icuperiodcb,
//  NULL,
//  ICU_CHANNEL_1,
//  0
//};

void initXBee()
{
		//SD2	
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	sdStart(&SD2, &uartCfg);
//	// Initialisation entrée PWM qualité signal
//	icuStart(&ICUD3, &icucfg);
//	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(2));
//	icuEnable(&ICUD3);
}

// Communication sans fils
void sendData ( DATA_COMM *data )
{
	char bufSend[40]={0};
	sprintf(bufSend, "T%03dR%03dL%03dA%05dB%03dS%03d", data->tangage, data->roulis, data->lacet, data->altitude, data->battery, data->signal);
	sdWrite(&SD2, (uint8_t*)bufSend, strlen(bufSend));
}

//DATA_COMM* rcvData()
//{	
//	int i = 0;
//	int j = 0;
//	DATA_COMM *data = NULL;
//	
//	char bufRead[]="T000R000L000A00000B000S000";
//	char bufFinalString[]="T000R000L000A00000B000S000";
//	
//	sdRead(&SD2, (uint8_t*)bufRead, strlen(bufRead));
//	
//	while(bufRead[i] != 'T')
//	{
//		i++;
//	}
//	
//	for ( j = 0; j<strlen(bufRead); j++)
//	{
//		bufFinalString[j] = bufRead[(j+i)%strlen(bufRead)];
//		i++;
//	}
//	
//	scanf(bufFinalString, "T%03dR%03dL%03dA%05dB%06.2fS%06.2f", data->tangage, data->roulis, data->lacet, data->altitude, data->battery, data->signal);

//	return data;
//}

