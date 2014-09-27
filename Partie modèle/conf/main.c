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
#include "motor.h"
#include "gyro.h"
#include "XBee.h"
#include "alti.h"

#define MAILBOX_SIZE	1

static Mailbox mb_Comm;
static Mailbox mb_Gyro;
//static msg_t b_Comm[MAILBOX_SIZE];

MemoryPool comPool;
MemoryPool gyroPool;

static const I2CConfig g_i2ccfg = 
{
	OPMODE_I2C,
	I2C_CLK_SPEED,
	FAST_DUTY_CYCLE_16_9
};


static WORKING_AREA(waMotor, 128);
static msg_t ThreadMotor( void *arg )
{
	DATA_COMM *data;
	
	initMotor();

	while(TRUE)
	{
		chMBFetch(&mb_Comm, (msg_t*)&data, TIME_INFINITE);
		data = (chPoolAlloc(&comPool));
		setSpeed(data->battery, MOTOR_1);
		setSpeed(data->battery, MOTOR_2);
		setSpeed(data->battery, MOTOR_3);
		setSpeed(data->battery, MOTOR_4);
	}
}

static WORKING_AREA(waComSnd, 128);
static msg_t ThreadComSnd( void *arg )
{
	DATA_GYRO *dataGyro;
	DATA_COMM dataComm;
	while(TRUE)
	{		
		chMBFetch(&mb_Gyro, (msg_t*)&dataGyro, TIME_INFINITE);
		dataGyro = (chPoolAlloc(&gyroPool));
		
		dataComm.tangage = dataGyro->angleTangage;
		dataComm.roulis = dataGyro->angleRoulis;
		dataComm.lacet = dataGyro->angleLacet;
/*		dataComm.tangage = 15;
		dataComm.roulis = 12;
		dataComm.lacet = 10;*/
		dataComm.altitude = 0;
		dataComm.battery = 95;
		dataComm.signal = 51;
		
		sendData ( &dataComm );
		chThdSleepMilliseconds( 10 );
	}
}

static WORKING_AREA(waComRcv, 512);
static msg_t ThreadComRcv( void *arg )
{
	//DATA_COMM data;
	while(TRUE)
	{
		DATA_COMM data __attribute__((aligned(sizeof(stkalign_t))));
		
		data = rcvData();
		
		chPoolFree(&comPool, &data);
		chMBPost(&mb_Comm, (msg_t)&data, TIME_INFINITE);
		
		chThdSleepMilliseconds( 10 );
	}
}

static WORKING_AREA(waAlti, 256);
static msg_t ThreadAlti( void *arg )
{
	float alti = 0;
	initAlti();
	
	while(TRUE)
	{
		alti = getAltitude();
		chThdSleepMilliseconds( 500 );
	}
}

static WORKING_AREA(waGyro, 256);
static msg_t ThreadGyro( void *arg )
{
	initGyro();
	
	while(TRUE)
	{
		DATA_GYRO data __attribute__((aligned(sizeof(stkalign_t))));
		data.angleRoulis = getAngle(REG_GYRO_X);
		data.angleTangage = getAngle(REG_GYRO_Y);
		data.angleLacet = getAngle(REG_GYRO_Z);
		data.accTangage = getAcceleration(REG_ACCEL_X);
		data.accRoulis = getAcceleration(REG_ACCEL_Y);
		data.accLacet = getAcceleration(REG_ACCEL_Z);
		
//		chPoolFree(&gyroPool, &data);
//		chMBPost(&mb_Gyro, (msg_t)&data, TIME_INFINITE);
//		chThdSleepMilliseconds(2); //Rafraichissement à 400 Hz
	}
}

/*
 * Application entry point.
 */
int main(void) 
{
	
	msg_t msg_Comm_Tx;
	msg_t msg_Gyro;
	
	halInit();
	chSysInit();
	
	/*
	* I²C initialization
	*/
	//I²C 2
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SCL */
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SDA */
	//palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SCL */
	//palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SDA */
	
	//chThdSleepMilliseconds( 500 );
	i2cInit();
	i2cObjectInit(&I2CD1);
	i2cStart(&I2CD1, &g_i2ccfg);
	
	chPoolInit(&comPool, sizeof(DATA_COMM), NULL);
	chPoolInit(&gyroPool, sizeof(DATA_GYRO), NULL);
	
	chMBInit(&mb_Comm, &msg_Comm_Tx, sizeof(msg_Comm_Tx));
	chMBInit(&mb_Gyro, &msg_Gyro, sizeof(msg_Gyro));
	initXBee();

//	initAlti();
	
	//chThdCreateStatic(waMotor, sizeof(waMotor), NORMALPRIO, ThreadMotor, NULL);
	//chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
	//chThdCreateStatic(waComRcv, sizeof(waComRcv), NORMALPRIO, ThreadComRcv, NULL);
	chThdCreateStatic(waGyro, sizeof(waGyro), NORMALPRIO, ThreadGyro, NULL);
	chThdCreateStatic(waAlti, sizeof(waAlti), NORMALPRIO, ThreadAlti, NULL);
	while(1)
	{
//		getAngle( AXIS_X );
//		temp = getAltitude();
		chThdSleepMilliseconds( 500 );
	}
}
