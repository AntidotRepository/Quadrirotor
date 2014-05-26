#ifndef GLOBALES_H
#define GLOBALES_H

#include "chmtx.h"

// ------------------------------------
// -------------	Motors  -------------
// ------------------------------------
	#define MOTOR_1								0
	#define MOTOR_2								1
	#define MOTOR_3								2
	#define MOTOR_4								3
	
	#define PWM_CLK_FREQUENCY 		10000
	#define TICKS_CNT_PSC					20
	
	
// ------------------------------------
// -------------	  I²C   -------------
// ------------------------------------
	#define I2C_CLK_SPEED					400000
	#define I2C_TIMEOUT						5000

// ------------------------------------
// -----------	  Gyroscop   ----------
// ------------------------------------
	#define AXIS_X								0
	#define AXIS_Y								1
	#define AXIS_Z								2
	
	#define REG_AXIS_X						0x3B
	#define REG_AXIS_Y						0x3D
#warning vérifier ce registre
	#define REG_AXIS_Z						0x3E

// ------------------------------------
// ----------	  altimeter   -----------
// ------------------------------------
	#define OSS 3 //Oversampling settings (taux d'échantillonage du BMP085 compris entre 0 (faible taux mais peu énergivore) et 3 (8 échantillons avant envois mais très énergivore))
	
// ------------------------------------
// ----------  Communication ----------
// ------------------------------------
	#define BAUDRATE							57600
	
// ------------------------------------
// ------------- Mutexs ---------------
// ------------------------------------


#endif // GLOBALES_H

