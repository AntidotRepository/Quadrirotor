static void pwmc1cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, GPIOB_PIN15);
}

static void pwmc2cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, GPIOB_PIN14);
}

static void pwmc3cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, GPIOB_PIN13);
}

static void pwmpcb4(PWMDriver *pwmp) {

  (void)pwmp;
	palClearPad(GPIOB, GPIOB_PIN12);
	palClearPad(GPIOB, GPIOB_PIN13);
	palClearPad(GPIOB, GPIOB_PIN14);
	palClearPad(GPIOB, GPIOB_PIN15);
}

static void pwmc4cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIOB, GPIOB_PIN12);
}

static PWMConfig pwmcfg4 = {
  10000,                          /* 10kHz PWM clock frequency.   */
  20,                             /* PWM period 1S (in ticks).    */
  pwmpcb4,
  {
    {PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb},
    {PWM_OUTPUT_ACTIVE_HIGH, pwmc2cb},
    {PWM_OUTPUT_ACTIVE_HIGH, pwmc3cb},
    {PWM_OUTPUT_ACTIVE_HIGH, pwmc4cb}
  },
  /* HW dependent part.*/
  0,
  0
};

int main(void) 
{
	halInit();
	chSysInit();

	/*
	* Initialisation de l'I²C
	*/
	//I²C 2
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SCL */
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SDA */
	//I²C 1
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SCL */
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	 /* SDA */
	
	i2cInit();
	i2cObjectInit(&I2CD1);
	i2cStart(&I2CD1, &i2ccfg1);

	pwmStart(&PWMD4, &pwmcfg4);

	palSetPadMode(GPIOB, GPIOB_PIN12, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN13, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN14, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, GPIOB_PIN15, PAL_MODE_OUTPUT_PUSHPULL);

	  pwmEnableChannel(&PWMD4, 0, 20-10);
	  pwmEnableChannel(&PWMD4, 1, 20-10);
	  pwmEnableChannel(&PWMD4, 2, 20-10);
	  pwmEnableChannel(&PWMD4, 3, 20-10);
	
	chThdSleepMilliseconds(5000);


	/*
	 * Creates the example thread.
	 */
	chThdCreateStatic(waComRcv, sizeof(waComRcv), NORMALPRIO, ThreadComRcv, NULL);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
//	chThdCreateStatic(waLacet, sizeof(waLacet), NORMALPRIO, ThreadLacet, NULL);
	chThdCreateStatic(waRoulisTangage, sizeof(waRoulisTangage)+2000, NORMALPRIO, ThreadRoulisTangage, NULL);
	chThdCreateStatic(waAltitude, sizeof(waAltitude)+1000, NORMALPRIO, ThreadAltitude, NULL);

	while (TRUE) {

		pwmEnableChannel(&PWMD4, 0, 20-5);
		pwmEnableChannel(&PWMD4, 1, 20-5);
		pwmEnableChannel(&PWMD4, 2, 20-5);
		pwmEnableChannel(&PWMD4, 3, 20-5);
		chThdSleepMilliseconds(500);
	}
}
