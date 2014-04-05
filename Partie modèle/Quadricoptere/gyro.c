#include "gyro.h"

void initGyro()
{
	//tableau stockage données envoyées sur l'I²C
	uint8_t txbuf[10] = {0};

	//Initialisation du gyroscope
	txbuf[0] = 0x6B;																											//Registre
	txbuf[1] = 0x00;																											//Donnée
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x19;																											//Registre
	txbuf[1] = 0x07;																											//Donnée
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Réglage de la fréquence d'échantillonage (125Hz)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1A;																											//Registre
	txbuf[1] = 0x06;																											//Donnée
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Réglage de la fréquence du filtre passe-bas (5Hz)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1B;																											//Registre
	txbuf[1] = 0x18;																											//Donnée
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Soi et la plage de mesure du gyroscope, des valeurs typiques: 0x18 (pas d'auto, 2000deg / s)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1C;																											//Registre
	txbuf[1] = 0x01;																											//Donnée
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Gamme d'accéléromètre d'auto-test et mesure la fréquence du filtre passe-haut, les valeurs typiques: 0x01 (non auto, 2G, 5 Hz)
	i2cReleaseBus(&I2CD1);
}

int getAngle( int axis )
{
	int angle = 0;
	
	uint8_t txbuf[10] = {0};
	uint8_t rxbuf[10] = {0};

	switch(axis)
	{
		case(AXIS_X):
			txbuf[0] = REG_AXIS_X;
			break;
		case(AXIS_Y):
			txbuf[0] = REG_AXIS_Y;
			break;
		case(AXIS_Z):
			txbuf[0] = REG_AXIS_Z;
			break;
	}
	
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 1, rxbuf, 2, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	angle = rxbuf[0]<<8;
	angle = angle | (unsigned int)rxbuf[1];
	
	if ( angle < 0xFFFF && angle > 0xBFFF )
	{
		angle = ( angle - 0xBFFF)/0xB6-0x5A;
	}
	else if ( angle > 0x0000 && angle < 0x4000)
	{
		angle = angle/0xB6;
	}
	else // angle = 0
	{
		angle = 0;
	}
		
	return angle;
}

int getAcceleration( int axis )
{
	int acceleration = 0;
	
	// ToDo
	
	return acceleration;
}
