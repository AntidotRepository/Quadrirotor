#include "alti.h"


static long pressure0;
long altitude = 0;
long pressure = 0;
static long_BMP085_reg g_long_registres;
static PressureVar variables;

//long readUncompensatedTemperature()
//{
//		uint8_t txbuf[2] = {0};
//		uint8_t rawTemperature[2] = {0};
//		txbuf[0] = 0xF4; //Register address //Calibration data ac1
//		txbuf[1] = 0x2E; //Temperature
//		i2cAcquireBus(&I2CD1);
//		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 1000);
//		i2cReleaseBus(&I2CD1);
//		chThdSleepMilliseconds(5);

//		txbuf[0] = 0xF6;
//		i2cAcquireBus(&I2CD1);
//		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, rawTemperature, 2, 100);
//		i2cReleaseBus(&I2CD1);

//		return (((uint16_t)rawTemperature[0]<<8)|((uint16_t)rawTemperature[1]));
//}

//long readUncompensatedPressure()
//{
//		uint8_t txbuf[2] = {0};
//		uint8_t rawPressure[3] = {0};

//		txbuf[0] = 0xF4; //Registrer address //Calibration data ac1
//		txbuf[1] = 0x34+(OSS<<6); //Temperature
//		i2cAcquireBus(&I2CD1);	
//		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 100);
//		i2cReleaseBus(&I2CD1);
//		chThdSleepMilliseconds(26);

//		txbuf[0] = 0xF6;
//		i2cAcquireBus(&I2CD1);
//		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, rawPressure, 3, 100);
//		i2cReleaseBus(&I2CD1);

//		return (((long)rawPressure[0]<<16)|((long)rawPressure[1]<<8)|(long)rawPressure[2])>>(8-OSS);	
//}

long calculateTemperature(/*PressureVar *variables,*/)
{
	static int uncompensatedTemperature = 0;
	//uncompensatedTemperature = readUncompensatedTemperature();
	
		uint8_t txbuf[2] = {0};
		uint8_t rawTemperature[2] = {0};
		txbuf[0] = 0xF4; //Register address //Calibration data ac1
		txbuf[1] = 0x2E; //Temperature
		i2cAcquireBus(&I2CD1);
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 1000);
		i2cReleaseBus(&I2CD1);
		chThdSleepMilliseconds(5);

		txbuf[0] = 0xF6;
		i2cAcquireBus(&I2CD1);
		i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, rawTemperature, 2, 100);
		i2cReleaseBus(&I2CD1);

		//return (((uint16_t)rawTemperature[0]<<8)|((uint16_t)rawTemperature[1]));

	variables.X1 = (uncompensatedTemperature-g_long_registres.ac6)*g_long_registres.ac5>>15;
	variables.X2 = (g_long_registres.mc<<11)/(variables.X1+g_long_registres.md);
	variables.B5 = variables.X1+variables.X2;
	return (variables.B5+8)>>4;
}

 long calculatePressure(/*, PressureVar *variables*/)
{
	
	static long uncompensatedPressure = 0;
	//int temperature = 0;
	static long pressure = 0;

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

		//return (((long)rawPressure[0]<<16)|((long)rawPressure[1]<<8)|(long)rawPressure[2])>>(8-OSS);
	uncompensatedPressure = (((long)rawPressure[0]<<16)|((long)rawPressure[1]<<8)|(long)rawPressure[2])>>(8-OSS);
	calculateTemperature(/*variables,*/);

	variables.B6 = variables.B5-4000;
	variables.X1 = (g_long_registres.b2*(variables.B6*variables.B6>>12))/pow(2,11);
	variables.X2 = g_long_registres.ac2*variables.B6/pow(2,11);
	variables.X3 = variables.X1+variables.X2;
	variables.B3 = ((((int32_t)g_long_registres.ac1*4+variables.X3)<<OSS)+2)/4;
	variables.X1 = g_long_registres.ac3*variables.B6/pow(2,13);
	variables.X2 = (g_long_registres.b1*(variables.B6*variables.B6/pow(2,12)))/pow(2,16);
	variables.X3 = ((variables.X1+variables.X2)+2)/pow(2,2);
	variables.B4 = g_long_registres.ac4*(unsigned long)(variables.X3+32768)/pow(2,15);
	variables.B7 = ((unsigned long)uncompensatedPressure-variables.B3)*(50000>>OSS);

	if(variables.B7 < 0x80000000)
	{
		pressure = (variables.B7*2)/variables.B4;
	}
	else
	{
		pressure = (variables.B7/variables.B4)*2;
	}
	variables.X1 = (pressure/pow(2,8))*(pressure/pow(2,8));
	variables.X1 = (variables.X1*3038)/pow(2,16);
	variables.X2 = (-7357*pressure)/pow(2,16);
	pressure = pressure+(variables.X1+variables.X2+3791)/pow(2,4);	
	return pressure;
}

void initAlti()
{
	uint8_t txbuf[10] = {0};
	BMP085_reg registres;
	//PressureVar variablesPression;

	//récupération de ac1
	txbuf[0] = 0xAA; //Calibration data ac1
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac1, 2, 1000);
	i2cReleaseBus(&I2CD1);	
	g_long_registres.ac1 = (short)((registres.ac1[0]<<8)+registres.ac1[1]);		

	txbuf[0] = 0xAC; //Calibration data ac2
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac2, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.ac2 = (registres.ac2[0]<<8)+registres.ac2[1];
	//récupération de ac3
	txbuf[0] = 0xAE; //Calibration data ac3
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac3, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.ac3 = (registres.ac3[0]<<8)+registres.ac3[1];
	//récupération de ac4
	txbuf[0] = 0xB0; //Calibration data ac4
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac4, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.ac4 = (registres.ac4[0]<<8)+registres.ac4[1];
	//récupération de ac5
	txbuf[0] = 0xB2; //Calibration data ac5
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac5, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.ac5 = (registres.ac5[0]<<8)+registres.ac5[1];
	//récupération de ac6
	txbuf[0] = 0xB4; //Calibration data ac6
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac6, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.ac6 = (registres.ac6[0]<<8)+registres.ac6[1];

	//récupération de b1
	txbuf[0] = 0xB6; //Calibration data b1
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.b1, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.b1 = (registres.b1[0]<<8)+registres.b1[1];
	//récupération de b2
	txbuf[0] = 0xB8; //Calibration data b2
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.b2, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.b2 = (registres.b2[0]<<8)+registres.b2[1];

	//récupération de mb
	txbuf[0] = 0xBA; //Calibration data mb
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.mb, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.mb = (registres.mb[0]<<8)+registres.mb[1];
	//récupération de mc
	txbuf[0] = 0xBC; //Calibration data mc
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.mc, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.mc = (registres.mc[0]<<8)+registres.mc[1];
	//récupération de md
	txbuf[0] = 0xBE; //Calibration data md
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.md, 2, 1000);
	i2cReleaseBus(&I2CD1);
	g_long_registres.md = (registres.md[0]<<8)+registres.md[1];
	
	pressure0 = calculatePressure(/*g_long_registres, &variablesPression*/);
}

long getAltitude()
{
	
//	uint8_t txbuf[10] = {0};

	//BMP085_reg registres;
	
	//PressureVar variablesPression;

	
	//long altitude = 0;
	//long pressure0 = 0;

	pressure = calculatePressure(/*g_long_registres, &variablesPression*/);
	altitude = 44330.75*(1-pow((double)pressure/pressure0, 0.19029))*100;

	return altitude;
}
