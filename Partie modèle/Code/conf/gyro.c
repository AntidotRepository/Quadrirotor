#include "gyro.h"

void initGyro()
{
	//tableau stockage donn�es envoy�es sur l'I�C
	uint8_t txbuf[10] = {0};

	//Initialisation du gyroscope
	txbuf[0] = 0x6B;																											//Registre
	txbuf[1] = 0x00;																											//Donn�e
	i2cAcquireBus(&I2CD1);																									
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Etat de dormance
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x19;// registre fr�quence �chantillonage																										//Registre
	txbuf[1] = 0x14;// Echantillonage � 400Hz																											//Donn�e
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//R�glage de la fr�quence d'�chantillonage (125Hz)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1A;																											//Registre
	txbuf[1] = 0x06;																											//Donn�e
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//R�glage de la fr�quence du filtre passe-bas (5Hz)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1B;																											//Registre
	txbuf[1] = 0x18;																											//Donn�e
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Soi et la plage de mesure du gyroscope, des valeurs typiques: 0x18 (pas d'auto, 2000deg / s)
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 0x1C;																											//Registre
	txbuf[1] = 0x00;																											//Donn�e
	i2cAcquireBus(&I2CD1);	
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 2, NULL, 0, I2C_TIMEOUT);	//Gamme d'acc�l�rom�tre d'auto-test et mesure la fr�quence du filtre passe-haut, les valeurs typiques: 0x01 (non auto, 2G, 5 Hz)
	i2cReleaseBus(&I2CD1);
}

int16_t getAngle( int axis )
{
	double m_forceVector;
	
	int16_t angle = 0;
	
	uint8_t txbuf[10] = {0};
	uint8_t rxbuf[10] = {0};

//	switch(axis)
//	{
//		case(AXIS_X):
//			txbuf[0] = REG_GYRO_X;
//			break;
//		case(AXIS_Y):
//			txbuf[0] = REG_GYRO_Y;
//			break;
//		case(AXIS_Z):
//			txbuf[0] = REG_AXIS_Z;
//			break;
//	}
	txbuf[0] = axis;
	
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 1, rxbuf, 2, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	angle = rxbuf[0]<<8;
	angle = angle | (unsigned int)rxbuf[1];
	
	// calculer vecteur de force
	m_forceVector = sqrt( pow(getAcceleration(REG_ACCEL_X), 2 )
											+ pow(getAcceleration(REG_ACCEL_Y), 2 )
											+ pow(getAcceleration(REG_ACCEL_Z), 2 ));
	return acos(getAcceleration(axis)/m_forceVector)*57.295779506;
	
//	if ( angle < 0xFFFF && angle > 0xBFFF )
//	{
//		floatAngle = ( angle - 0xBFFF)/0xB6-0x5A;
//	}
//	else if ( angle > 0x0000 && angle < 0x4000)
//	{
//		floatAngle = angle/0xB6;
//	}
//	else // angle = 0
//	{
//		floatAngle = 0;
//	}
//	floatAngle = angle*131/2000;
		
//	return floatAngle;
}

double getAcceleration( int axis )
{
	int16_t accel = 0;
		
	uint8_t txbuf[10] = {0};
	uint8_t rxbuf[10] = {0};
	
	txbuf[0] = axis;
	
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, 0x69, txbuf, 1, rxbuf, 2, I2C_TIMEOUT);
	i2cReleaseBus(&I2CD1);
	
	accel = rxbuf[0]<<8;
	accel = accel | (unsigned int)rxbuf[1];
	
	return accel/2;
}
