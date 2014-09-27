#include "XBee.h"

// Récupération de la qualité du signal
//icucnt_t last_width, last_period;

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

static systime_t timeRst;

void initXBee()
{
		//SD2	
	timeRst = chTimeNow();
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
//	if(timeRst+30000 == chTimeNow())
//	{
//		rstXBee();
//	}
	char bufSend[] = {'T','0','1','5','R','0','4','6','L','0','1','3','A','0','1','2','4','5','B','0','9','5','S','0','5','1'};

	sprintf(bufSend, "T%03dR%03dL%03dA%05dB%03dS%03d", data->tangage, data->roulis, data->lacet, data->altitude, data->battery, data->signal);
	sdWrite(&SD2, (uint8_t*)bufSend, strlen(bufSend));
}

DATA_COMM rcvData()
{	
	int i = 0;
	int j = 0;
	DATA_COMM data;
	char bufRead[] = {'T','0','1','5','R','0','4','6','L','0','1','3','A','0','1','2','4','5','B','0','9','5','S','0','5','1','\0'};
	char bufFinalString[] = {'T','0','1','5','R','0','4','6','L','0','1','3','A','0','1','2','4','5','B','0','9','5','S','0','5','1','\0'};
	
	do
	{
		sdReadTimeout(&SD2, (uint8_t*)bufRead, (size_t)strlen(bufRead), (systime_t)TIME_INFINITE); 
		i = 0;
		while(bufRead[i] != 'T')
		{
			i++;
		}
		
		for ( j = 0; j<strlen(bufRead); j++)
		{
			bufFinalString[j] = bufRead[(j+i)%strlen(bufRead)];
		}
	}
	while(sscanf(bufFinalString, "T%dR%dL%dA%dB%dS%d", &data.tangage, &data.roulis, &data.lacet, &data.altitude, &data.battery, &data.signal)!=6);

	return data;
}

//void rstXBee()
//{
//	char string[] = {"Resetting XBee"};
//	char ATMod[] = {'+', '+', '+'};
//	char RstCmd[] = {'A', 'T', 'F', 'R', '\r', '\n'};
//	
//	//Ajouter envoi reset Xbee PC

//	sdWrite(&SD2, (uint8_t*)string, strlen(string));
//	chThdSleepMilliseconds( 1000 );
//	sdWrite(&SD2, (uint8_t*)ATMod, strlen(ATMod));
//	chThdSleepMilliseconds( 10 );
//	sdWrite(&SD2, (uint8_t*)RstCmd, strlen(RstCmd));
//}
