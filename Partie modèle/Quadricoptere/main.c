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


static const I2CConfig g_i2ccfg = 
{
	OPMODE_I2C,
	I2C_CLK_SPEED,
	FAST_DUTY_CYCLE_2
};

static WORKING_AREA(waMotor, 128);
static msg_t ThreadMotor( void *arg )
{
	initMotor();

	while(TRUE)
	{

		setSpeed(10, MOTOR_1);
		setSpeed(10, MOTOR_2);
		setSpeed(10, MOTOR_3);
		setSpeed(10, MOTOR_4);
		chThdSleepMilliseconds( 5000 );
		setSpeed(20, MOTOR_1);
		setSpeed(20, MOTOR_2);
		setSpeed(20, MOTOR_3);
		setSpeed(20, MOTOR_4);
		chThdSleepMilliseconds( 5000 );
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

static WORKING_AREA(waComSnd, 256);
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

		chThdSleepMilliseconds( 50 );
	}
}

static WORKING_AREA(waComRcv, 128);
static msg_t ThreadComRcv( void *arg )
{
	DATA_COMM data;
	char bufRcv[40]={0};

	
	//initXBee();
	
	while(TRUE)
	{
		data.tangage = 15;
		data.roulis = 46;
		data.lacet = 13;
		data.altitude = 1245;
		data.battery = 95;
		data.signal = 51;

//		sendData ( &data );
			
		//sprintf(bufSend, "T%03dR%03dL%03dA%05dB%03dS%03d", data.tangage, data.roulis, data.lacet, data.altitude, data.battery, data.signal);
		//sprintf(bufSend, "test");
		//sdWrite(&SD2, (uint8_t*)bufSend, strlen(bufSend));
		

		chThdSleepMilliseconds( 10 );
	}
}

/*
 * Application entry point.
 */
int main(void) {
	
	long temp = 0;
	
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

	initXBee();

//	initGyro();
  
//	initAlti();
	
	chThdCreateStatic(waMotor, sizeof(waMotor), NORMALPRIO, ThreadMotor, NULL);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
	
	while(1)
	{
//		getAngle( AXIS_X );
//		temp = getAltitude();
		chThdSleepMilliseconds( 500 );
	}
}
