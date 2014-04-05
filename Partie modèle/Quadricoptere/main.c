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
	
	/*
	* Serial initialization
	*/
	//SD2
//	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
//	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

//	sdStart(&SD2, &uartCfg);
	
//	initGyro();
//  initMotor();
	initAlti();
	while(TRUE)
	{
//		getAngle( AXIS_X );
		temp = getAltitude();
	}
}
