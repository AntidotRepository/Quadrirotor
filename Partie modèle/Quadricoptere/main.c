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
static msg_t b_Comm[MAILBOX_SIZE];

MemoryPool comPool;

static const I2CConfig g_i2ccfg = 
{
	OPMODE_I2C,
	I2C_CLK_SPEED,
	FAST_DUTY_CYCLE_2
};

static WORKING_AREA(waMotor, 128);
static msg_t ThreadMotor( void *arg )
{
	DATA_COMM *data;
	msg_t msg;
	
	initMotor();

	while(TRUE)
	{
		chMBFetch(&mb_Comm, &msg, TIME_INFINITE);
		data = (DATA_COMM*)msg;

		setSpeed(data->battery, MOTOR_1);
		setSpeed(data->battery, MOTOR_2);
		setSpeed(data->battery, MOTOR_3);
		setSpeed(data->battery, MOTOR_4);
//		setSpeed(50, MOTOR_1);
//		setSpeed(50, MOTOR_2);
//		setSpeed(50, MOTOR_3);
//		setSpeed(50, MOTOR_4);
//		chThdSleepMilliseconds( 5000 );
//		setSpeed(70, MOTOR_1);
//		setSpeed(70, MOTOR_2);
//		setSpeed(70, MOTOR_3);
//		setSpeed(70, MOTOR_4);
//		chThdSleepMilliseconds( 5000 );
	}
}

static WORKING_AREA(waComSnd, 128);
static msg_t ThreadComSnd( void *arg )
{
	DATA_COMM data;
	
	while(TRUE)
	{
		data.tangage = 15;
		data.roulis = 46;
		data.lacet = 13;
		data.altitude = 1245;
		data.battery = 95;
		data.signal = 51;

		sendData ( &data );
		chThdSleepMilliseconds( 100 );
	}
}

static WORKING_AREA(waComRcv, 512);
static msg_t ThreadComRcv( void *arg )
{
	DATA_COMM *data = NULL;
	msg_t msg;
	while(TRUE)
	{
		*data = rcvData();
		
		msg = (msg_t)data;
		chMBPost(&mb_Comm, msg, TIME_INFINITE);
		chThdSleepMilliseconds( 100 );
	}
}

/*
 * Application entry point.
 */
int main(void) {
	
	msg_t msg;
	
	halInit();
	chSysInit();
	
	/*
	* I²C initialization
	*/
	//I²C 2
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SCL */
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SDA */
	
	i2cInit();
	i2cObjectInit(&I2CD1);
	i2cStart(&I2CD1, &g_i2ccfg);
	
	chMBInit(&mb_Comm, (msg_t*)msg, sizeof(msg));
	
	initXBee();

//	initGyro();
  
//	initAlti();
	
	chThdCreateStatic(waMotor, sizeof(waMotor), NORMALPRIO, ThreadMotor, NULL);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
	chThdCreateStatic(waComRcv, sizeof(waComRcv), NORMALPRIO, ThreadComRcv, NULL);

	while(1)
	{
//		getAngle( AXIS_X );
//		temp = getAltitude();
		chThdSleepMilliseconds( 500 );
	}
}
