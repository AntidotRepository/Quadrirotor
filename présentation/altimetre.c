/**
* Déclaration des variables et des types de donnés nécessaires
**/

#define OSS 3 //Oversampling settings (taux d'échantillonage du BMP085 compris entre 0 (faible taux mais peu énergivore) et 3 (8 échantillons avant envois mais très énergivore))
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
/**
* Lecture de la température comme définis dans la doc en
* allant interroger les registres associés
**/
long readUncompensatedTemperature()
{
    uint8_t txbuf[2] = {0};
    uint8_t rawTemperature[2] = {0};
    txbuf[0] = 0xF4; //Registrer address //Calibration data ac1
    txbuf[1] = 0x2E; //Temperature
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 1000);
    i2cReleaseBus(&I2CD1);
    chThdSleepMilliseconds(5);

    txbuf[0] = 0xF6;
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, rawTemperature, 2, 100);
    i2cReleaseBus(&I2CD1);
    return (((uint16_t)rawTemperature[0]<<8)|((uint16_t)rawTemperature[1]));
}
/**
* Lecture de la pression atmosphérique comme définis dans la doc
* en allant interroger les registres associés
**/
long readUncompensatedPressure()
{
    uint8_t txbuf[2] = {0};
    uint8_t rawPressure[3] = {0};
    txbuf[0] = 0xF4; //Registrer address //Calibration data
    ac1 txbuf[1] = 0x34+(OSS<<6); //Temperature
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 2, NULL, 0, 100);
    i2cReleaseBus(&I2CD1);
    chThdSleepMilliseconds(26);
    txbuf[0] = 0xF6;
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, rawPressure, 3, 100);
    i2cReleaseBus(&I2CD1);
    return((long)rawPressure[0]<<16)| ((long)rawPressure[1]<<8)| (long)rawPressure[2])>>(8-OSS);
}
/**
* Transcription de l'algorithme de calcul de la température
* comme définis dans la doc
**/
long calculateTemperature(long_BMP085_reg long_registres, PressureVar *variables, int *uncompensatedTemperature)
{
    variables->X1 = (*uncompensatedTemperature- long_registres.ac6)*long_registres.ac5>>15;
    variables->X2 = (long_registres.mc<<11)/(variables-> X1+long_registres.md);
    variables->B5 = variables->X1+variables->X2;
    return (variables->B5+8)>>4;
}
/**
* Transcription de l'algorithme de calcul de la pression
* atmosphérique comme définis dans la doc
**/
long calculatePressure(long_BMP085_reg long_registres, PressureVar *variables)
{
    int uncompensatedTemperature = 0;
    long uncompensatedPressure = 0;
    int temperature = 0;
    long pressure = 0;
// récupération des valeurs brute de la température 
	uncompensatedTemperature = readUncompensatedTemperature();
// récupération des valeurs brute de la pression athmosphérique
    uncompensatedPressure = readUncompensatedPressure();
// Calcul de la température (même si ce n'est pas le résultat qui nous intéresse mais un calcul intermédiaire
    temperature = calculateTemperature(long_registres, variables, &uncompensatedTemperature);
    variables->B6 = variables->B5-4000;
    variables->X1 = (long_registres.b2*(variables->B6*variables-> B6>>12))/pow(2,11);
    variables->X2 = long_registres.ac2*variables->B6/pow(2,11);
    variables->X3 = variables->X1+variables->X2;
    variables->B3 = ((((int32_t)long_registres.ac1*4+variables-> X3)<<OSS)+2)/4;
    variables->X1 = long_registres.ac3*variables->B6/pow(2,13);
    variables->X2 = (long_registres.b1*(variables->B6*variables-> B6/pow(2,12)))/pow(2,16);
    variables->X3 = ((variables->X1+variables->X2)+2)/pow(2,2);
    variables->B4 = long_registres.ac4*(unsigned long)(variables-> X3+32768)/pow(2,15);
    variables->B7 = ((unsigned long)uncompensatedPressure-variables-> B3)*(50000>>OSS);
    if(variables->B7 < 0x80000000)
    {
        pressure = (variables->B7*2)/variables->B4;
    }
    else
    {
        pressure = (variables->B7/variables->B4)*2;
    }
    variables->X1 = (pressure/pow(2,8))*(pressure/pow(2,8));
    variables->X1 = (variables->X1*3038)/pow(2,16);
    variables->X2 = (-7357*pressure)/pow(2,16);
    pressure = pressure+(variables->X1+variables->X2+3791)/pow(2,4);
    return pressure;
}
static WORKING_AREA(waAltitude, 128);
static msg_t ThreadAltitude(void *arg)
{
    uint8_t txbuf[10] = {0};
    BMP085_reg registres;
    long_BMP085_reg long_registres;
    PressureVar variablesPression;
    long pressure = 0;
    long altitude = 0;
    long pressure0 = 0;
    //Initialisation du BMP085 en récupérant les valeurs des registres définis dans la doc

    //récupération de ac1
    txbuf[0] = 0xAA; //Calibration data ac1
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac1, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.ac1 = (short)((registres.ac1[0]<<8)+registres.ac1[1]);
    //récupération de ac2
    txbuf[0] = 0xAC; //Calibration data
    ac2 i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac2, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.ac2 = (registres.ac2[0]<<8)+registres.ac2[1];
    //récupération de ac3
    txbuf[0] = 0xAE; //Calibration data
    ac3 i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac3, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.ac3 = (registres.ac3[0]<<8)+registres.ac3[1];
    //récupération de ac4
    txbuf[0] = 0xB0; //Calibration data ac4
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac4, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.ac4 = (registres.ac4[0]<<8)+registres.ac4[1];
    //récupération de ac5
    txbuf[0] = 0xB2; //Calibration data ac5
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac5, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.ac5 = (registres.ac5[0]<<8)+registres.ac5[1];
    //récupération de ac6
    txbuf[0] = 0xB4; //Calibration data ac6
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.ac6, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.ac6 = (registres.ac6[0]<<8)+registres.ac6[1];
    //récupération de b1
    txbuf[0] = 0xB6; //Calibration data b1
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.b1, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.b1 = (registres.b1[0]<<8)+registres.b1[1];
    //récupération de b2
    txbuf[0] = 0xB8; //Calibration data b2
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.b2, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.b2 = (registres.b2[0]<<8)+registres.b2[1];
    //récupération de mb
    txbuf[0] = 0xBA; //Calibration data mb
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.mb, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.mb = (registres.mb[0]<<8)+registres.mb[1];
    //récupération de mc
    txbuf[0] = 0xBC; //Calibration data mc
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.mc, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.mc = (registres.mc[0]<<8)+registres.mc[1];
    //récupération de md
    txbuf[0] = 0xBE; //Calibration data md
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x77, txbuf, 1, (uint8_t*)registres.md, 2, 1000);
    i2cReleaseBus(&I2CD1);
    long_registres.md = (registres.md[0]<<8)+registres.md[1];
    pressure0 = calculatePressure(long_registres, &variablesPression);
    while(TRUE)
    {
        //Calcul de la pression athmosphérique
        pressure = calculatePressure(long_registres, &variablesPression); //Calcul de l'altitude en fonction de la pression athmosphérique
        altitude = 44330.75*(1-pow((double)pressure/pressure0, 0.19029))*100;
        //Stockage de l'altitude dans une variable globale
        g_altitude = altitude;
    }
}

