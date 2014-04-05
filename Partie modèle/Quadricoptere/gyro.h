#ifndef GYRO_H
#define GYRO_H

#include "hal.h"
#include "GLOBALES.h"

void initGyro( void );
int getAngle( int axis );
int getAcceleration( int axis );

#endif // GYRO_H
