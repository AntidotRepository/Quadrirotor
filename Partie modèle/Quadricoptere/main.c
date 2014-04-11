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

 SerialConfig uartCfg=
{
	 BAUDRATE,									/* Baudrate																	 				*/
	 0,											 		/* cr1 register values															 */
	 0,											 		/* cr2 register values															 */
	 0													/* cr3 register values															 */
};


static WORKING_AREA(waMotor, 128);
static msg_t ThreadMotor( void *arg )
{
	initMotor();
	
	while(TRUE)
	{
		setSpeed(0, MOTOR_1);
		setSpeed(0, MOTOR_2);
		setSpeed(0, MOTOR_3);
		setSpeed(0, MOTOR_4);
		chThdSleepMilliseconds( 5000 );
		setSpeed(80, MOTOR_1);
		setSpeed(80, MOTOR_2);
		setSpeed(80, MOTOR_3);
		setSpeed(80, MOTOR_4);
		chThdSleepMilliseconds( 5000 );
	}
}

static WORKING_AREA(waCom, 128);
static msg_t ThreadCom( void *arg )
{
	DATA_COMM data;
	char bufSend[40]={0};
	//SD2						T015R046L013A01245B095.26S051.01         "T000R000L000A00000B000000000S00000000000"
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	sdStart(&SD2, &uartCfg);
	
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
			
		sprintf(bufSend, "T%03dR%03dL%03dA%05dB%03dS%03d", data.tangage, data.roulis, data.lacet, data.altitude, data.battery, data.signal);
		//sprintf(bufSend, "test");
		sdWrite(&SD2, (uint8_t*)bufSend, strlen(bufSend));
		

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

	
//	initGyro();
  
//	initAlti();
	
	chThdCreateStatic(waMotor, sizeof(waMotor), NORMALPRIO, ThreadMotor, NULL);
	chThdCreateStatic(waCom, sizeof(waCom), NORMALPRIO, ThreadCom, NULL);
	
	while(1)
	{
//		getAngle( AXIS_X );
//		temp = getAltitude();
		chThdSleepMilliseconds( 500 );
	}
}
