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

#define OSS 3 //Oversampling settings (taux d'�chantillonage du BMP085 compris entre 0 (faible taux mais peu �nergivore) et 3 (8 �chantillons avant envois mais tr�s �nergivore))

//static void pwmpcb1(PWMDriver *pwmp);
//static void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n);
//static void spicb(SPIDriver *spip);

///* Total number of channels to be sampled by a single ADC operation.*/
//#define ADC_GRP1_NUM_CHANNELS	 2

///* Depth of the conversion buffer, channels are sampled four times each.*/
//#define ADC_GRP1_BUF_DEPTH			4

/* Max number of mail in the box. */
#define MAILBOX_SIZE						1

I2CDriver *i2cDriver1;
const I2CConfig *i2cConfig1;

//Globale contenant les caract�res � envoyer par la liaison sans fil. (peut �tre remplacer par des mailbox)

int g_tangage = 0;
int g_roulis = 0;
int g_altitude = 0;
int g_lacet = 0;

//static void pwmpcb1(PWMDriver *pwmp) {

//  (void)pwmp;

//  /* Starts an asynchronous ADC conversion operation, the conversion
//     will be executed in parallel to the current PWM cycle and will
//     terminate before the next PWM cycle.*/
////  chSysLockFromIsr();
////  adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
////  chSysUnlockFromIsr();
//	palClearPad(GPIOB, GPIOB_PIN15);
//}

static void pwmc1cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, GPIOB_PIN15);
}

//static PWMConfig pwmcfg1 = {
//  10000,                                    /* 10kHz PWM clock frequency.   */
//  10000,                                    /* PWM period 1S (in ticks).    */
//  pwmpcb1,
//  {
//    {PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb},
//    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
//    {PWM_OUTPUT_DISABLED, NULL},
//    {PWM_OUTPUT_DISABLED, NULL}
//  },
//  /* HW dependent part.*/
//  0,
//  0
//};
//static void pwmpcb2(PWMDriver *pwmp) {

//  (void)pwmp;

//  /* Starts an asynchronous ADC conversion operation, the conversion
//     will be executed in parallel to the current PWM cycle and will
//     terminate before the next PWM cycle.*/
////  chSysLockFromIsr();
////  adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
////  chSysUnlockFromIsr();
//	palClearPad(GPIOB, GPIOB_PIN14);
//}

static void pwmc2cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, GPIOB_PIN14);
}

//static PWMConfig pwmcfg2 = {
//  10000,                                    /* 10kHz PWM clock frequency.   */
//  10000,                                    /* PWM period 1S (in ticks).    */
//  pwmpcb2,
//  {
//    {PWM_OUTPUT_ACTIVE_HIGH, pwmc2cb},
//    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
//    {PWM_OUTPUT_DISABLED, NULL},
//    {PWM_OUTPUT_DISABLED, NULL}
//  },
//  /* HW dependent part.*/
//  0,
//  0
//};
//static void pwmpcb3(PWMDriver *pwmp) {

//  (void)pwmp;

//  /* Starts an asynchronous ADC conversion operation, the conversion
//     will be executed in parallel to the current PWM cycle and will
//     terminate before the next PWM cycle.*/
////  chSysLockFromIsr();
////  adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
////  chSysUnlockFromIsr();
//	palClearPad(GPIOB, GPIOB_PIN13);
//}

static void pwmc3cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, GPIOB_PIN13);
}

//static PWMConfig pwmcfg3 = {
//  10000,                                    /* 10kHz PWM clock frequency.   */
//  10000,                                    /* PWM period 1S (in ticks).    */
//  pwmpcb3,
//  {
//    {PWM_OUTPUT_ACTIVE_HIGH, pwmc3cb},
//    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
//    {PWM_OUTPUT_DISABLED, NULL},
//    {PWM_OUTPUT_DISABLED, NULL}
//  },
//  /* HW dependent part.*/
//  0,
//  0
//};
static void pwmpcb4(PWMDriver *pwmp) {

  (void)pwmp;

  /* Starts an asynchronous ADC conversion operation, the conversion
     will be executed in parallel to the current PWM cycle and will
     terminate before the next PWM cycle.*/
//  chSysLockFromIsr();
//  adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
//  chSysUnlockFromIsr();
	palClearPad(GPIOB, GPIOB_PIN12);
	palClearPad(GPIOB, GPIOB_PIN13);
	palClearPad(GPIOB, GPIOB_PIN14);
	palClearPad(GPIOB, GPIOB_PIN15);
}

static void pwmc4cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, GPIOB_PIN12);
}

static PWMConfig pwmcfg4 = {
  10000,                                    /* 10kHz PWM clock frequency.   */
  20,                                    /* PWM period 1S (in ticks).    */
  pwmpcb4,
  {
    {PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb},
    {PWM_OUTPUT_ACTIVE_HIGH, pwmc2cb},
    {PWM_OUTPUT_ACTIVE_HIGH, pwmc3cb},
    {PWM_OUTPUT_ACTIVE_HIGH, pwmc4cb}
  },
  /* HW dependent part.*/
  0,
  0
};

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
	short ac1[2]; //Registre 0xAA
	short ac2[2]; //Registre 0xAC
	short ac3[2]; //Registre 0xAE
	unsigned short ac4[2]; //Registre 0xB0
	unsigned short ac5[2]; //Registre 0xB2
	unsigned short ac6[2]; //Registre 0xB4

	short b1[2]; //Registre 0xB6
	short b2[2]; //Registre 0xB8

	short mb[2]; //Registre 0xBA
	short mc[2]; //Registre 0xBC
	short md[2]; //Registre 0xBE
	
};

typedef struct long_BMP085_reg long_BMP085_reg;
struct long_BMP085_reg
{
	short ac1; //Registre 0xAA
	short ac2; //Registre 0xAC
	short ac3; //Registre 0xAE
	unsigned short ac4; //Registre 0xB0
	unsigned short ac5; //Registre 0xB2
	unsigned short ac6; //Registre 0xB4

	short b1; //Registre 0xB6
	short b2; //Registre 0xB8

	short mb; //Registre 0xBA
	short mc; //Registre 0xBC
	short md; //Registre 0xBE
};

//I2CConfig configI2C_1;
//I2CDriver driverI2C_1;
/*
 * ADC samples buffer.
 */
//static adcsample_t samples[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

/*
 * ADC conversion group.
 * Mode:				Linear buffer, 4 samples of 2 channels, SW triggered.
 * Channels:		IN10	 (48 cycles sample time)
 *							Sensor (192 cycles sample time)
 */
//static const ADCConversionGroup adcgrpcfg = {
//	FALSE,
//	ADC_GRP1_NUM_CHANNELS,
//	adccb,
//	NULL,
//	/* HW dependent part.*/
//	0,												/* CR1 */
//	ADC_CR2_SWSTART,					/* CR2 */
//	0,
//	ADC_SMPR2_SMP_AN10(ADC_SAMPLE_48) | ADC_SMPR2_SMP_SENSOR(ADC_SAMPLE_192),
//	0,
//	ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
//	0,
//	0,
//	0,
//	ADC_SQR5_SQ2_N(ADC_CHANNEL_IN10) | ADC_SQR5_SQ1_N(ADC_CHANNEL_SENSOR)
//};

/*
 * PWM configuration structure.
 * Cyclic callback enabled, channels 1 and 2 enabled without callbacks,
 * the active state is a logic one.
 */
//static PWMConfig pwmcfg1 = {
//	10000,																		/* 10kHz PWM clock frequency.	 */
//	10000,																		/* PWM period 1S (in ticks).		*/
//	pwmpcb1,
//	{
//		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
//		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
//		{PWM_OUTPUT_DISABLED, NULL},
//		{PWM_OUTPUT_DISABLED, NULL}
//	},
//	/* HW dependent part.*/
//	0,
//	0
//};

/*
 * SPI configuration structure.
 * Maximum speed (12MHz), CPHA=0, CPOL=0, 16bits frames, MSb transmitted first.
 * The slave select line is the pin GPIOA_SPI1NSS on the port GPIOA.
 */
//static const SPIConfig spicfg = {
//	spicb,
//	/* HW dependent part.*/
//	GPIOB,
//	12,
//	SPI_CR1_DFF
//};

static const I2CConfig i2ccfg1 = 
{
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2
};

/*
 * PWM cyclic callback.
 * A new ADC conversion is started.
 */
//static void pwmpcb1(PWMDriver *pwmp) {

//	(void)pwmp;

//	/* Starts an asynchronous ADC conversion operation, the conversion
//		 will be executed in parallel to the current PWM cycle and will
//		 terminate before the next PWM cycle.*/
//	chSysLockFromIsr();
//	adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
//	chSysUnlockFromIsr();
//}

/*
 * ADC end conversion callback.
 * The PWM channels are reprogrammed using the latest ADC samples.
 * The latest samples are transmitted into a single SPI transaction.
 */
//void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n) {

//	(void) buffer; (void) n;
//	/* Note, only in the ADC_COMPLETE state because the ADC driver fires an-
//		 intermediate callback when the buffer is half full.*/
//	if (adcp->state == ADC_COMPLETE) {
//		adcsample_t avg_ch1, avg_ch2;

//		/* Calculates the average values from the ADC samples.*/
//		avg_ch1 = (samples[0] + samples[2] + samples[4] + samples[6]) / 4;
//		avg_ch2 = (samples[1] + samples[3] + samples[5] + samples[7]) / 4;

//		chSysLockFromIsr();

//		/* Changes the channels pulse width, the change will be effective
//			 starting from the next cycle.*/
//		pwmEnableChannelI(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, avg_ch1));
//		pwmEnableChannelI(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, avg_ch2));

//		/* SPI slave selection and transmission start.*/
//		spiSelectI(&SPID2);
//		spiStartSendI(&SPID2, ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH, samples);

//		chSysUnlockFromIsr();
//	}
//}

/*
 * SPI end transfer callback.
 */
//static void spicb(SPIDriver *spip) {

//	/* On transfer end just releases the slave select line.*/
//	chSysLockFromIsr();
//	spiUnselectI(spip);
//	chSysUnlockFromIsr();
//}

static SerialConfig uartCfg=
{
	 57600,									/* Baudrate																					*/
	 0,											 /* cr1 register values															 */
	 0,											 /* cr2 register values															 */
	 0												/* cr3 register values															 */
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
		sprintf(dataWrite, "T%03dR%03dL789A%05dS", g_tangage, g_roulis, g_altitude);
		//Envois des donn�es
		sdWrite(&SD2, (uint8_t*)dataWrite, strlen(dataWrite));

		//Envoi dans la mailbox de communication
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
	uint8_t rxbuf[10] = {0};
	uint8_t txbuf[10] = {0};

	uint16_t angleNord = 0;

//	txbuf[0] = 0x00;
//	txbuf[1] = 0x78;
//	i2cAcquireBus(&I2CD1);																									
//	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, 100);	//Configuration pour mode continu
//	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x01;
	txbuf[1] = 0x20;
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, 100);	//Configuration pour mode continu
	i2cReleaseBus(&I2CD1);

//	txbuf[0] = 0x02;																											//Registre
//	txbuf[1] = 0x00;																											//Donn�e
//	i2cAcquireBus(&I2CD1);																									
//	i2cMasterTransmitTimeout(&I2CD1, 0x1E, txbuf, 2, NULL, 0, 100);	//Configuration pour mode continu
//	i2cReleaseBus(&I2CD1);

	while(TRUE)
	{
		//Lecture de la boussole
		i2cAcquireBus(&I2CD1);
		i2cMasterReceiveTimeout(&I2CD1, 0x1E, rxbuf, 10, 100);
		i2cReleaseBus(&I2CD1);
				
		angleNord = rxbuf[4]<<8;
		angleNord = angleNord | (unsigned int)rxbuf[5];
		
		g_lacet = angleNord;
//		chMBPost( &mb_Lacet, b_Lacet[MAILBOX_SIZE-1], TIME_INFINITE);
	}
}

static WORKING_AREA(waRoulisTangage, 128);
static msg_t ThreadRoulisTangage(void *arg)
{
	// gestion du Roulis et du tangage(gyroscope)
	
	//tableau stockage donn�es re�ues sur l'I�C
	uint8_t rxbuf[10] = {0};
	uint8_t txbuf[10] = {0};

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
	txbuf[1] = 0x01;																											//Donn�e
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
		g_roulis = roulis;
		g_tangage = tangage;
		
		tangage = 0;
		roulis = 0;
	}
}

long readUncompensatedTemperature()
{
		uint8_t txbuf[2] = {0};
		uint8_t rawTemperature[2] = {0};
		txbuf[0] = 0xF4; //Registrer address //Calibration data ac1
		txbuf[1] = 0x2E; //Temperature
		i2cAcquireBus(&I2CD1);	
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 1000);
		i2cReleaseBus(&I2CD1);
		chThdSleepMilliseconds(5);

		txbuf[0] = 0xF6;
		i2cAcquireBus(&I2CD1);
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, rawTemperature, 2, 100);
		i2cReleaseBus(&I2CD1);
		
		return (((uint16_t)rawTemperature[0]<<8)|((uint16_t)rawTemperature[1]));	
}

long readUncompensatedPressure()
{
		uint8_t txbuf[2] = {0};
		uint8_t rawPressure[3] = {0};

		txbuf[0] = 0xF4; //Registrer address //Calibration data ac1
		txbuf[1] = 0x34+(OSS<<6); //Temperature
		i2cAcquireBus(&I2CD1);	
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 100);
		i2cReleaseBus(&I2CD1);
		chThdSleepMilliseconds(26);

		txbuf[0] = 0xF6;
		i2cAcquireBus(&I2CD1);
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, rawPressure, 3, 100);
		i2cReleaseBus(&I2CD1);
		
		return (((long)rawPressure[0]<<16)|((long)rawPressure[1]<<8)|(long)rawPressure[2])>>(8-OSS);	
}

long calculateTemperature(long_BMP085_reg long_registres, PressureVar *variables, int *uncompensatedTemperature)
{
	variables->X1 = (*uncompensatedTemperature-long_registres.ac6)*long_registres.ac5>>15;
	variables->X2 = (long_registres.mc<<11)/(variables->X1+long_registres.md);
	variables->B5 = variables->X1+variables->X2;
	return (variables->B5+8)>>4;
}

long calculatePressure(long_BMP085_reg long_registres, PressureVar *variables)
{
	int uncompensatedTemperature = 0;
	long uncompensatedPressure = 0;
	int temperature = 0;
	long pressure = 0; 

	uncompensatedTemperature = readUncompensatedTemperature();
	uncompensatedPressure = readUncompensatedPressure();
	temperature = calculateTemperature(long_registres, variables, &uncompensatedTemperature);

	variables->B6 = variables->B5-4000;
	variables->X1 = (long_registres.b2*(variables->B6*variables->B6>>12))/pow(2,11);
	variables->X2 = long_registres.ac2*variables->B6/pow(2,11);
	variables->X3 = variables->X1+variables->X2;
	variables->B3 = ((((int32_t)long_registres.ac1*4+variables->X3)<<OSS)+2)/4;
	variables->X1 = long_registres.ac3*variables->B6/pow(2,13);
	variables->X2 = (long_registres.b1*(variables->B6*variables->B6/pow(2,12)))/pow(2,16);
	variables->X3 = ((variables->X1+variables->X2)+2)/pow(2,2);
	variables->B4 = long_registres.ac4*(unsigned long)(variables->X3+32768)/pow(2,15);
	variables->B7 = ((unsigned long)uncompensatedPressure-variables->B3)*(50000>>OSS);

	if(variables->B7 < 0x80000000)
	{
		pressure = (variables->B7*2)/variables->B4;
	}
	else
	{
		pressure = (variables->B7/variables->B4)*2;
	}
	variables->X1 = (pressure/pow(2,8))*(pressure/pow(2,8));
	variables->X1 = (variables->X1*3038)/pow(2,16);
	variables->X2 = (-7357*pressure)/pow(2,16);
	pressure = pressure+(variables->X1+variables->X2+3791)/pow(2,4);	
	return pressure;
}

static WORKING_AREA(waAltitude, 128);
static msg_t ThreadAltitude(void *arg)
{
	// gestion de l'altitude (pression athmosph�rique???)
	
	//Mailbox Altitude
	//static msg_t b_Altitude[MAILBOX_SIZE];
	//tableau stockage donn�es re�ues sur l'I�C
	uint8_t txbuf[10] = {0};

	BMP085_reg registres;
	long_BMP085_reg long_registres;
	PressureVar variablesPression;

	long pressure = 0;
	long altitude = 0;
	long pressure0 = 0;

//	msg_t retour;

//	uint8_t ac1[2] = {0}; //Registre 0xAA
//	uint8_t ac2[2] = {0}; //Registre 0xAC
//	uint8_t ac3[2] = {0}; //Registre 0xAE
//	uint8_t ac4[2] = {0}; //Registre 0xB0
//	uint8_t ac5[2] = {0}; //Registre 0xB2
//	uint8_t ac6[2] = {0}; //Registre 0xB4

//	uint8_t b1[2] = {0}; //Registre 0xB6
//	uint8_t b2[2] = {0}; //Registre 0xB8

//	uint8_t mb[2] = {0}; //Registre 0xBA
//	uint8_t mc[2] = {0}; //Registre 0xBC
//	uint8_t md[2] = {0}; //Registre 0xBE
	
	//Initialisation du BMP085
	//r�cup�ration de ac1
	txbuf[0] = 0xAA; //Calibration data ac1
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac1, 2, 1000);
	i2cReleaseBus(&I2CD1);	
	long_registres.ac1 = (short)((registres.ac1[0]<<8)+registres.ac1[1]);		

	txbuf[0] = 0xAC; //Calibration data ac2
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac2, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac2 = (registres.ac2[0]<<8)+registres.ac2[1];
	//r�cup�ration de ac3
	txbuf[0] = 0xAE; //Calibration data ac3
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac3, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac3 = (registres.ac3[0]<<8)+registres.ac3[1];
	//r�cup�ration de ac4
	txbuf[0] = 0xB0; //Calibration data ac4
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac4, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac4 = (registres.ac4[0]<<8)+registres.ac4[1];
	//r�cup�ration de ac5
	txbuf[0] = 0xB2; //Calibration data ac5
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac5, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac5 = (registres.ac5[0]<<8)+registres.ac5[1];
	//r�cup�ration de ac6
	txbuf[0] = 0xB4; //Calibration data ac6
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac6, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.ac6 = (registres.ac6[0]<<8)+registres.ac6[1];
	
	//r�cup�ration de b1
	txbuf[0] = 0xB6; //Calibration data b1
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.b1, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.b1 = (registres.b1[0]<<8)+registres.b1[1];
	//r�cup�ration de b2
	txbuf[0] = 0xB8; //Calibration data b2
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.b2, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.b2 = (registres.b2[0]<<8)+registres.b2[1];

	//r�cup�ration de mb
	txbuf[0] = 0xBA; //Calibration data mb
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.mb, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.mb = (registres.mb[0]<<8)+registres.mb[1];
	//r�cup�ration de mc
	txbuf[0] = 0xBC; //Calibration data mc
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.mc, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.mc = (registres.mc[0]<<8)+registres.mc[1];
	//r�cup�ration de md
	txbuf[0] = 0xBE; //Calibration data md
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.md, 2, 1000);
	i2cReleaseBus(&I2CD1);
	long_registres.md = (registres.md[0]<<8)+registres.md[1];
	
	pressure0 = calculatePressure(long_registres, &variablesPression);
	while(TRUE)
	{
		pressure = calculatePressure(long_registres, &variablesPression);
		altitude = 44330.75*(1-pow((double)pressure/pressure0, 0.19029))*100;
		g_altitude = altitude;
	}

//		int i = 0;
//		char addr = 0xEF; // define the I2C Address
//		uint8_t rReg[3] = {0,0,0};
//		char wReg[2] = {0,0};
//		uint8_t cmd = 0x00;
//		
//		static i2cflags_t erreur = 0;
//		char data[22];
//		cmd = 0xAA;
//		
// 
//		for (i = 0; i < 22; i++)
//		{
//			i2cAcquireBus(&I2CD1);	
//			i2cMasterTransmitTimeout(&I2CD1, addr, &cmd, 1, rReg, 1, 10000000);
//			erreur = i2cGetErrors(&I2CD1);
//			i2cReleaseBus(&I2CD1);
//			data[i] =	rReg[0];
//			cmd += 2;
//		}
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
	 *	 and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *	 RTOS is active.
	 */
	halInit();
	chSysInit();

	/*
	* Initialisation de l'I�C
	*/
	//I�C 2
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SCL */
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SDA */
	//I�C 1
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SCL */
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SDA */
	
	i2cInit();
	i2cObjectInit(&I2CD1);
	i2cStart(&I2CD1, &i2ccfg1);

	pwmStart(&PWMD4, &pwmcfg4);
//	pwmStart(&PWMD3, &pwmcfg3);
//	pwmStart(&PWMD2, &pwmcfg2);
//	pwmStart(&PWMD9, &pwmcfg1);

	palSetPadMode(GPIOB, GPIOB_PIN12, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN13, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN14, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN15, PAL_MODE_OUTPUT_PUSHPULL);

//  pwmEnableChannel(&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 7500));
//  pwmEnableChannel(&PWMD4, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 5000));
//  pwmEnableChannel(&PWMD4, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 2500));
//  pwmEnableChannel(&PWMD4, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 1000));
  pwmEnableChannel(&PWMD4, 0, 20-10);
  pwmEnableChannel(&PWMD4, 1, 20-10);
  pwmEnableChannel(&PWMD4, 2, 20-10);
  pwmEnableChannel(&PWMD4, 3, 20-10);
//	/*
//	 * If the user button is pressed after the reset then the test suite is
//	 * executed immediately before activating the various device drivers in
//	 * order to not alter the benchmark scores.
//	 */
//	if (palReadPad(GPIOA, GPIOA_BUTTON))
//		TestThread(&SD1);

//	/*
//	 * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
//	 * PB12 - NSS.
//	 * PB13 - SCK.
//	 * PB14 - MISO.
//	 * PB15 - MOSI.
//	 */
//	spiStart(&SPID2, &spicfg);
//	palSetPad(GPIOB, 12);
//	palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |
//													 PAL_STM32_OSPEED_HIGHEST);					 /* NSS.		 */
//	palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) |
//													 PAL_STM32_OSPEED_HIGHEST);					 /* SCK.		 */
//	palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));							/* MISO.		*/
//	palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
//													 PAL_STM32_OSPEED_HIGHEST);					 /* MOSI.		*/

//	/*
//	 * Initializes the ADC driver 1 and enable the thermal sensor.
//	 * The pin PC0 on the port GPIOC is programmed as analog input.
//	 */
//	adcStart(&ADCD1, NULL);
//	adcSTM32EnableTSVREFE();
//	palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG);

//	/*
//	 * Initializes the PWM driver 4, routes the TIM4 outputs to the board LEDs.
//	 */
//	pwmStart(&PWMD4, &pwmcfg1);
//	palSetPadMode(GPIOB, GPIOB_LED4, PAL_MODE_ALTERNATE(2));
//	palSetPadMode(GPIOB, GPIOB_LED3, PAL_MODE_ALTERNATE(2));

	/*
	 * Creates the example thread.
	 */
	chThdCreateStatic(waComRcv, sizeof(waComRcv), NORMALPRIO, ThreadComRcv, NULL);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
//	chThdCreateStatic(waLacet, sizeof(waLacet), NORMALPRIO, ThreadLacet, NULL);
	chThdCreateStatic(waRoulisTangage, sizeof(waRoulisTangage)+2000, NORMALPRIO, ThreadRoulisTangage, NULL);
//chThdCreateStatic(waAltitude, sizeof(waAltitude)+1000, NORMALPRIO, ThreadAltitude, NULL);
//	chThdCreateStatic(waIntelligence, sizeof(waIntelligence), NORMALPRIO, ThreadIntelligence, NULL);
//	/*
//	 * Normal main() thread activity, in this demo it does nothing except
//	 * sleeping in a loop and check the button state, when the button is
//	 * pressed the test procedure is launched with output on the serial
//	 * driver 1.

//	 */
	while (TRUE) {
//		if (palReadPad(GPIOA, GPIOA_BUTTON))
//			TestThread(&SD1);
		pwmEnableChannel(&PWMD4, 0, 20-5);
		pwmEnableChannel(&PWMD4, 1, 20-5);
		pwmEnableChannel(&PWMD4, 2, 20-5);
		pwmEnableChannel(&PWMD4, 3, 20-5);
		chThdSleepMilliseconds(500);
	}
}
