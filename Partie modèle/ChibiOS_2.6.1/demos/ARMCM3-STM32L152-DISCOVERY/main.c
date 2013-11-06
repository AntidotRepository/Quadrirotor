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

//static void pwmpcb(PWMDriver *pwmp);
//static void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n);
//static void spicb(SPIDriver *spip);

///* Total number of channels to be sampled by a single ADC operation.*/
//#define ADC_GRP1_NUM_CHANNELS   2

///* Depth of the conversion buffer, channels are sampled four times each.*/
//#define ADC_GRP1_BUF_DEPTH      4

/* Max number of mail in the box. */
#define MAILBOX_SIZE						1

I2CDriver *i2cDriver1;
const I2CConfig *i2cConfig1;

//Globale contenant les caract�res � envoyer par la liaison sans fil. (peut �tre remplacer par des mailbox)
char stringTangageRoulis[8] = {0};


//I2CConfig configI2C_1;
//I2CDriver driverI2C_1;
/*
 * ADC samples buffer.
 */
//static adcsample_t samples[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 4 samples of 2 channels, SW triggered.
 * Channels:    IN10   (48 cycles sample time)
 *              Sensor (192 cycles sample time)
 */
//static const ADCConversionGroup adcgrpcfg = {
//  FALSE,
//  ADC_GRP1_NUM_CHANNELS,
//  adccb,
//  NULL,
//  /* HW dependent part.*/
//  0,                        /* CR1 */
//  ADC_CR2_SWSTART,          /* CR2 */
//  0,
//  ADC_SMPR2_SMP_AN10(ADC_SAMPLE_48) | ADC_SMPR2_SMP_SENSOR(ADC_SAMPLE_192),
//  0,
//  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
//  0,
//  0,
//  0,
//  ADC_SQR5_SQ2_N(ADC_CHANNEL_IN10) | ADC_SQR5_SQ1_N(ADC_CHANNEL_SENSOR)
//};

/*
 * PWM configuration structure.
 * Cyclic callback enabled, channels 1 and 2 enabled without callbacks,
 * the active state is a logic one.
 */
//static PWMConfig pwmcfg = {
//  10000,                                    /* 10kHz PWM clock frequency.   */
//  10000,                                    /* PWM period 1S (in ticks).    */
//  pwmpcb,
//  {
//    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
//    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
//    {PWM_OUTPUT_DISABLED, NULL},
//    {PWM_OUTPUT_DISABLED, NULL}
//  },
//  /* HW dependent part.*/
//  0,
//  0
//};

/*
 * SPI configuration structure.
 * Maximum speed (12MHz), CPHA=0, CPOL=0, 16bits frames, MSb transmitted first.
 * The slave select line is the pin GPIOA_SPI1NSS on the port GPIOA.
 */
//static const SPIConfig spicfg = {
//  spicb,
//  /* HW dependent part.*/
//  GPIOB,
//  12,
//  SPI_CR1_DFF
//};

static const I2CConfig i2ccfg1 = 
{
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2
};

static const I2CConfig i2ccfg2 = {
    OPMODE_I2C,
		3000000, //100000, //
    FAST_DUTY_CYCLE_16_9, //STD_DUTY_CYCLE, //
};
/*
 * PWM cyclic callback.
 * A new ADC conversion is started.
 */
//static void pwmpcb(PWMDriver *pwmp) {

//  (void)pwmp;

//  /* Starts an asynchronous ADC conversion operation, the conversion
//     will be executed in parallel to the current PWM cycle and will
//     terminate before the next PWM cycle.*/
//  chSysLockFromIsr();
//  adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
//  chSysUnlockFromIsr();
//}

/*
 * ADC end conversion callback.
 * The PWM channels are reprogrammed using the latest ADC samples.
 * The latest samples are transmitted into a single SPI transaction.
 */
//void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n) {

//  (void) buffer; (void) n;
//  /* Note, only in the ADC_COMPLETE state because the ADC driver fires an-
//     intermediate callback when the buffer is half full.*/
//  if (adcp->state == ADC_COMPLETE) {
//    adcsample_t avg_ch1, avg_ch2;

//    /* Calculates the average values from the ADC samples.*/
//    avg_ch1 = (samples[0] + samples[2] + samples[4] + samples[6]) / 4;
//    avg_ch2 = (samples[1] + samples[3] + samples[5] + samples[7]) / 4;

//    chSysLockFromIsr();

//    /* Changes the channels pulse width, the change will be effective
//       starting from the next cycle.*/
//    pwmEnableChannelI(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, avg_ch1));
//    pwmEnableChannelI(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, avg_ch2));

//    /* SPI slave selection and transmission start.*/
//    spiSelectI(&SPID2);
//    spiStartSendI(&SPID2, ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH, samples);

//    chSysUnlockFromIsr();
//  }
//}

/*
 * SPI end transfer callback.
 */
//static void spicb(SPIDriver *spip) {

//  /* On transfer end just releases the slave select line.*/
//  chSysLockFromIsr();
//  spiUnselectI(spip);
//  chSysUnlockFromIsr();
//}

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



static WORKING_AREA(waAltitude, 128);
static msg_t ThreadAltitude(void *arg)
{
	
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
	i2cObjectInit(&I2CD2);
	i2cStart(&I2CD2, &i2ccfg2);

//  /*
//   * If the user button is pressed after the reset then the test suite is
//   * executed immediately before activating the various device drivers in
//   * order to not alter the benchmark scores.
//   */
//  if (palReadPad(GPIOA, GPIOA_BUTTON))
//    TestThread(&SD1);

//  /*
//   * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
//   * PB12 - NSS.
//   * PB13 - SCK.
//   * PB14 - MISO.
//   * PB15 - MOSI.
//   */
//  spiStart(&SPID2, &spicfg);
//  palSetPad(GPIOB, 12);
//  palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |
//                           PAL_STM32_OSPEED_HIGHEST);           /* NSS.     */
//  palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) |
//                           PAL_STM32_OSPEED_HIGHEST);           /* SCK.     */
//  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));              /* MISO.    */
//  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
//                           PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */

//  /*
//   * Initializes the ADC driver 1 and enable the thermal sensor.
//   * The pin PC0 on the port GPIOC is programmed as analog input.
//   */
//  adcStart(&ADCD1, NULL);
//  adcSTM32EnableTSVREFE();
//  palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG);

//  /*
//   * Initializes the PWM driver 4, routes the TIM4 outputs to the board LEDs.
//   */
//  pwmStart(&PWMD4, &pwmcfg);
//  palSetPadMode(GPIOB, GPIOB_LED4, PAL_MODE_ALTERNATE(2));
//  palSetPadMode(GPIOB, GPIOB_LED3, PAL_MODE_ALTERNATE(2));

  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waComRcv, sizeof(waComRcv), NORMALPRIO, ThreadComRcv, NULL);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
//	chThdCreateStatic(waLacet, sizeof(waLacet), NORMALPRIO, ThreadLacet, NULL);
	chThdCreateStatic(waRoulisTangage, sizeof(waRoulisTangage), NORMALPRIO, ThreadRoulisTangage, NULL);
//	chThdCreateStatic(waAltitude, sizeof(waAltitude), NORMALPRIO, ThreadAltitude, NULL);
//	chThdCreateStatic(waIntelligence, sizeof(waIntelligence), NORMALPRIO, ThreadIntelligence, NULL);
//  /*
//   * Normal main() thread activity, in this demo it does nothing except
//   * sleeping in a loop and check the button state, when the button is
//   * pressed the test procedure is launched with output on the serial
//   * driver 1.

//   */
  while (TRUE) {
//    if (palReadPad(GPIOA, GPIOA_BUTTON))
//      TestThread(&SD1);
    chThdSleepMilliseconds(500);
  }
}
