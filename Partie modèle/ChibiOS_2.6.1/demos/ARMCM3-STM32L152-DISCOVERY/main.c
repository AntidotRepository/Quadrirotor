/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define OSS 2 //Oversampling settings (taux d'�chantillonage du BMP085 compris entre 0 (faible taux mais peu �nergivore) et 3 (8 �chantillons avant envois mais tr�s �nergivore))


/* Max number of mail in the box. */
#define MAILBOX_SIZE						1

I2CDriver *i2cDriver1;
const I2CConfig *i2cConfig1;

//Globale contenant les caract�res � envoyer par la liaison sans fil. (peut �tre remplacer par des mailbox)
char stringTangageRoulis[8] = {0};
char stringAltitude[6] = {0};

typedef struct PressureVar PressureVar;
struct PressureVar
{
	long X1;
	long X2;
	long X3;
	
	long B3;
	unsigned long B4;
	long B5;
	long B6;
	unsigned long B7;
};

typedef struct BMP085_reg BMP085_reg;
struct BMP085_reg
{
	uint8_t ac1[2]; //Registre 0xAA
	uint8_t ac2[2]; //Registre 0xAC
	uint8_t ac3[2]; //Registre 0xAE
	uint8_t ac4[2]; //Registre 0xB0
	uint8_t ac5[2]; //Registre 0xB2
	uint8_t ac6[2]; //Registre 0xB4

	uint8_t b1[2]; //Registre 0xB6
	uint8_t b2[2]; //Registre 0xB8

	uint8_t mb[2]; //Registre 0xBA
	uint8_t mc[2]; //Registre 0xBC
	uint8_t md[2]; //Registre 0xBE
	
};

typedef struct long_BMP085_reg long_BMP085_reg;
struct long_BMP085_reg
{
	int ac1; 					//Registre 0xAA
	int ac2; 					//Registre 0xAC
	int ac3; 					//Registre 0xAE
	unsigned int ac4; //Registre 0xB0
	unsigned int ac5; //Registre 0xB2
	unsigned int ac6; //Registre 0xB4

	int b1; 					//Registre 0xB6
	int b2; 					//Registre 0xB8

	int mb; 					//Registre 0xBA
	int mc; 					//Registre 0xBC
	int md; 					//Registre 0xBE
};



static const I2CConfig i2ccfg1 = 
{
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2
};



static SerialConfig uartCfg=
{
   57600,                  /* Baudrate                                          */
   0,                       /* cr1 register values                               */
   0,                       /* cr2 register values                               */
   0                        /* cr3 register values                               */
};


//Initialisation des Mailbox
//Mailbox communication
static Mailbox mb_Com;
//Mailbox Lacet
static Mailbox mb_Lacet;
//Mailbox Roulis/Tangage
static Mailbox mb_Roulis_Tangage;
//Mailbox Altitude
static Mailbox mb_Altitude;

static WORKING_AREA(waComSnd, 128);
static msg_t ThreadComSnd(void *arg)
{
	char dataRead[50] = {0};
	
	while(TRUE)
	{
		//R�ception des donn�es
		sdRead(&SD2, (uint8_t*)dataRead, 50);
	}
}

static WORKING_AREA(waComRcv, 128);
static msg_t ThreadComRcv(void *arg)
{
	// gestion de la communication (Xbee Pro Serie 2)
	char dataWrite[]="T123R456L789A12345S";

	//Mailbox communication
//	static msg_t b_Com[MAILBOX_SIZE];
	
	//Initialisation du module Xbee
	// ...	

  /*
   * Activates the serial driver 1 using the driver default configuration.
   * PA9 and PA10 are routed to USART1.
   */
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
	sdStart(&SD2, &uartCfg);
	
	while(TRUE)
	{
		//lecture de la bo�te aux lettres.
//		chMBFetch( &mb_Roulis_Tangage, 	stringTangageRoulis,	TIME_INFINITE);
		sprintf(dataWrite, "%sL789A12345S", stringTangageRoulis);
		//Envois des donn�es
		sdWrite(&SD2, (uint8_t*)dataWrite, strlen(dataWrite));
		
		//Envoi dans la mailbox de communication
		#warning Revoir le deuxi�me param�tre (durer avant qu'on renvoi, peut �tre mettre TIME_INFINITE)
//		chMBPost( &mb_Com, b_Com[MAILBOX_SIZE-1], 100);
	}
}

static WORKING_AREA(waLacet, 128);
static msg_t ThreadLacet(void *arg)
{
	// gestion du lacet (boussole)
	
	//Mailbox Lacet
//	static msg_t b_Lacet[MAILBOX_SIZE];
	
	//Initialisation de la boussole
	// ...

	while(TRUE)
	{
		//Lecture de la boussole
//		chMBPost( &mb_Lacet, b_Lacet[MAILBOX_SIZE-1], TIME_INFINITE);
	}
}

static WORKING_AREA(waRoulisTangage, 128);
static msg_t ThreadRoulisTangage(void *arg)
{
	// gestion du Roulis et du tangage(gyroscope)
	
	
	//tableau stockage donn�es re�ues sur l'I�C
	static uint8_t rxbuf[10] = {0};
	static uint8_t txbuf[10] = {0};

	int tangage;
	int roulis;



	//Initialisation du gyroscope
	txbuf[0] = 0x6B;																											//Registre
	txbuf[1] = 0x00;																											//Donn�e
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);	//Etat de dormance
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = 0x19;																											//Registre
	txbuf[1] = 0x07;																											//Donn�e
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);	//R�glage de la fr�quence d'�chantillonage (125Hz)
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = 0x1A;																											//Registre
	txbuf[1] = 0x06;																											//Donn�e
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);	//R�glage de la fr�quence du filtre passe-bas (5Hz)
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = 0x1B;																											//Registre
	txbuf[1] = 0x18;																											//Donn�e
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);	//Soi et la plage de mesure du gyroscope, des valeurs typiques: 0x18 (pas d'auto, 2000deg / s)
	i2cReleaseBus(&I2CD1);
	
	txbuf[0] = 0x1C;																											//Registre
	txbuf[1] = 0x01;													  													//Donn�e
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);	//Gamme d'acc�l�rom�tre d'auto-test et mesure la fr�quence du filtre passe-haut, les valeurs typiques: 0x01 (non auto, 2G, 5 Hz)
	i2cReleaseBus(&I2CD1);
	
	while(TRUE)
	{
		//r�cup�ration de GYRO_XOUT_H
		txbuf[0] = 0x3B; //Registre de GYRO_XOUT_H
		i2cAcquireBus(&I2CD1);	
		i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 1, rxbuf, 2, 1000);
		i2cReleaseBus(&I2CD1);
		//R�union de l'octet de poids faible et de l'octet de poids fort
		tangage = rxbuf[0]<<8;
		tangage = tangage | (unsigned int)rxbuf[1];
		//Conversion de l'angle
		if (tangage < 0xFFFF && tangage > 0xBFFF)
		{
			tangage = (tangage - 0xBFFF)/0xB6-0x5A;
		}
		else if (tangage > 0x0000 && tangage < 0x4000)
		{
			tangage = tangage/0xB6;
		}
		else // angle = 0
		{
			tangage = 0;
		}
		
		//r�cup�ration de GYRO_XOUT_Y
		txbuf[0] = 0x3D; //Registre de GYRO_YOUT_H
		i2cAcquireBus(&I2CD1);	
		i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 1, rxbuf, 2, 1000);
		i2cReleaseBus(&I2CD1);
		//R�union de l'octet de poids faible et de l'octet de poids fort
		roulis = rxbuf[0]<<8;
		roulis = roulis | (unsigned int)rxbuf[1];
		//Conversion de l'angle
		if (roulis < 0xFFFF && roulis > 0xBFFF)
		{
			roulis = (roulis - 0xBFFF)/0xB6-0x5A;
		}
		else if (roulis > 0x0000 && roulis < 0x4000)
		{
			roulis = roulis/0xB6;
		}
		else // angle = 0
		{
			roulis = 0;
		}
		
		//Fabrication d'une chaine de caract�res � envoyer
		sprintf(stringTangageRoulis, "T%03dR%03d", tangage, roulis);
	}
}


void readUncompensatedTemperature(long_BMP085_reg *registres, int *uncompensatedTemperature)
{
		uint8_t txbuf[2] = {0};
		uint8_t rawTemperature[2] = {0};

		txbuf[0] = 0xF4; //Registrer address //Calibration data ac1
		txbuf[1] = 0x2E; //Temperature
		i2cAcquireBus(&I2CD1);	
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 100);
		i2cReleaseBus(&I2CD1);
		txbuf[0] = 0xF6;
		i2cAcquireBus(&I2CD1);
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 100);
		i2cMasterReceiveTimeout(&I2CD1, 0x77, rawTemperature, 2, 100);
		i2cReleaseBus(&I2CD1);
		
		*uncompensatedTemperature = ((rawTemperature[0]<<8)+rawTemperature[1]);	
}

void readUncompensatedPressure(long_BMP085_reg *registres, long *uncompensatedPressure)
{
		uint8_t txbuf[2] = {0};
		uint8_t rawPressure[3] = {0};

		txbuf[0] = 0xF4; //Registrer address //Calibration data ac1
		txbuf[1] = 0x34+(OSS<<6); //Temperature
		i2cAcquireBus(&I2CD1);	
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 100);
		i2cReleaseBus(&I2CD1);
		txbuf[0] = 0xF6;
		i2cAcquireBus(&I2CD1);
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 100);
		i2cMasterReceiveTimeout(&I2CD1, 0x77, rawPressure, 3, 100);
		i2cReleaseBus(&I2CD1);
		printf("trololo");
		*uncompensatedPressure = ((rawPressure[0]<<16)+(rawPressure[1]<<8)+rawPressure[2]);	
}

void calculateTemperature(long_BMP085_reg *registres, int *trueTemperature, PressureVar *variables, int *uncompensatedTemperature)
{
	variables->X1 = ((long)*uncompensatedTemperature-(registres->ac6))*(registres->ac5)>>15;
	variables->X2 = (long)registres->mc<<11/(variables->X1+registres->md);
	variables->B5 = variables->X1+variables->X2;
	*trueTemperature = (variables->B5+8)>>4;
}

void calculatePressure(long_BMP085_reg *registres, long *pressure, PressureVar *variables)
{
	int uncompensatedTemperature;
	long uncompensatedPressure;
	int temperature;
	
	readUncompensatedTemperature(registres, &uncompensatedTemperature);
	readUncompensatedPressure(registres, &uncompensatedPressure);
	calculateTemperature(registres, &temperature, variables, &uncompensatedTemperature);
	
	variables->B6 = variables->B5-4000; 																					//OK
	variables->X1 = ((registres->b2)*((variables->B6)*(variables->B6))>>12)>>11; 	//NOK
	variables->X2 = registres->ac2*variables->B6>>11;															//OK
	variables->X3 = variables->X1+variables->X2;																	//OK	
	variables->B3 = (((int32_t)registres->ac1*4+variables->X3)<<(OSS+2))>>2; 			//OK
	variables->X1 = registres->ac3*variables->B6>>13;															//OK
	variables->X2 = (registres->b1*(variables->B6*variables->B6>>12))>>16;				// pas test
	variables->X3 = ((variables->X1+variables->X2)+2)>>2;													// pas test
	variables->B4 = registres->ac4*(uint32_t)(variables->X3+32768)>>15;						//NOK
	variables->B7 = ((uint32_t)uncompensatedPressure-variables->B3)*(50000>>OSS);	//OK

	if(variables->B7 < 0x80000000)
	{
		*pressure = (variables->B7*2)/variables->B4;																//NOK
	}
	else
	{
		*pressure = (variables->B7/variables->B4)*2;																//NOK
	}
	variables->X1 = (*pressure>>8)*(*pressure>>8);																//OK
	variables->X1 = (variables->X1*3038)>>16;																			//NOK
	variables->X2 = (-7357**pressure)>>16;																				//NOK
	*pressure = *pressure+((variables->X1+variables->X2+3791)>>4);								//NOK
}


static WORKING_AREA(waAltitude, 128);
static msg_t ThreadAltitude(void *arg)
{	
	uint8_t txbuf[10] = {0};
	BMP085_reg registres;
	long_BMP085_reg long_registres;
	PressureVar variablesPression;
	long pressure = 0;
	long altitude = 0;

	txbuf[0] = 0xAA; //Calibration data ac1
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.ac1, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac1 = (registres.ac1[0]<<8)+registres.ac1[1];

	txbuf[0] = 0xAC; //Calibration data ac2
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.ac2, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac2 = (registres.ac2[0]<<8)+registres.ac2[1];
	//r�cup�ration de ac3
	txbuf[0] = 0xAE; //Calibration data ac3
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.ac3, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac3 = (registres.ac3[0]<<8)+registres.ac3[1];
	//r�cup�ration de ac4
	txbuf[0] = 0xB0; //Calibration data ac4
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.ac4, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac4 = (registres.ac4[0]<<8)+registres.ac4[1];
	//r�cup�ration de ac5
	txbuf[0] = 0xB2; //Calibration data ac5
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.ac5, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac5 = (registres.ac5[0]<<8)+registres.ac5[1];
	//r�cup�ration de ac6
	txbuf[0] = 0xB4; //Calibration data ac6
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.ac6, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac6 = (registres.ac6[0]<<8)+registres.ac6[1];
	
	//r�cup�ration de b1
	txbuf[0] = 0xB6; //Calibration data b1
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.b1, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.b1 = (registres.b1[0]<<8)+registres.b1[1];
	//r�cup�ration de b2
	txbuf[0] = 0xB8; //Calibration data b2
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.b2, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.b2 = (registres.b2[0]<<8)+registres.b2[1];

	//r�cup�ration de mb
	txbuf[0] = 0xBA; //Calibration data mb
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.mb, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.mb = (registres.mb[0]<<8)+registres.mb[1];
	//r�cup�ration de mc
	txbuf[0] = 0xBA; //Calibration data mc
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.mc, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.mc = (registres.mc[0]<<8)+registres.mc[1];
	//r�cup�ration de md
	txbuf[0] = 0xBC; //Calibration data md
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, NULL, 0, 1000);
	i2cMasterReceiveTimeout(&I2CD1, 0x77, registres.md, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.md = (registres.md[0]<<8)+registres.md[1];
	
	while(TRUE)
	{
		calculatePressure(&long_registres, &pressure, &variablesPression);
		altitude = 44330*(1-pow((pressure/1013.25), (1/5.255)));
		sprintf(stringAltitude, "A%05ld", altitude);
	}
}

static WORKING_AREA(waIntelligence, 128);
static msg_t ThreadIntelligence(void *arg)
{
	// gestion de l'intelligence g�n�rale du syst�me
	
	//Variables pour stockage des messages des autres t�ches
//	msg_t *msgp_Com 						= NULL;
//	msg_t *msgp_Lacet 					= NULL;
//	msg_t *msgp_Roulis_Tangage 	= NULL;
//	msg_t *msgp_Altitude 				= NULL;
	
	//Initialisation de l'outil de mesure de la pression atmosph�rique pour calculer l'altitude
	// ...
	
	while(TRUE)
	{
		//Lecture des bo�tes aux lettres.
//		chMBFetch( &mb_Com, 						msgp_Com, 						TIME_INFINITE);
//		chMBFetch( &mb_Lacet, 					msgp_Lacet, 					TIME_INFINITE);
//		chMBFetch( &mb_Roulis_Tangage, 	msgp_Roulis_Tangage,	TIME_INFINITE);
//		chMBFetch( &mb_Altitude, 				msgp_Altitude, 				TIME_INFINITE);
		
		//Variation des ESCs
	}
}


/*
 * Application entry point.
 */
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

	/*
	* Initialisation de l'I�C
	*/
	//I�C 2
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */
	//I�C 1
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */
	
	i2cInit();
	i2cObjectInit(&I2CD1);
	i2cStart(&I2CD1, &i2ccfg1);


  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waComRcv, sizeof(waComRcv), NORMALPRIO, ThreadComRcv, NULL);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
//	chThdCreateStatic(waLacet, sizeof(waLacet), NORMALPRIO, ThreadLacet, NULL);
	chThdCreateStatic(waRoulisTangage, sizeof(waRoulisTangage), NORMALPRIO, ThreadRoulisTangage, NULL);
	chThdCreateStatic(waAltitude, sizeof(waAltitude), NORMALPRIO, ThreadAltitude, NULL);
//	chThdCreateStatic(waIntelligence, sizeof(waIntelligence), NORMALPRIO, ThreadIntelligence, NULL);
//  /*
//   * Normal main() thread activity, in this demo it does nothing except
//   * sleeping in a loop and check the button state, when the button is
//   * pressed the test procedure is launched with output on the serial
//   * driver 1.

//   */
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
