#ifndef ALTI_H
#define ALTI_H

#include "math.h"
#include "hal.h"
#include "GLOBALES.h"
#include "alti.h"

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


long readUncompensatedTemperature( void );
long readUncompensatedPressure( void );
long calculateTemperature(/*PressureVar *variables, int *uncompensatedTemperature*/ void);
long calculatePressure( void /*, PressureVar *variables*/);
void initAlti( void );
long getAltitude( void );

#endif //ALTI_H
