static msg_t ThreadRoulisTangage(void *arg)
{
//tableau stockage donn�es re�ues sur l'I�C
    uint8_t rxbuf[10] = {0};
    uint8_t txbuf[10] = {0};
    int tangage;
    int roulis;

//Initialisation du gyroscope
    txbuf[0] = 0x6B; //Registre
    txbuf[1] = 0x00; //Donn�e
    i2cAcquireBus(&I2CD1); //Acquisition bus I�C (�viter des collisions) - Mise en �tat de dormance
    i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);
    i2cReleaseBus(&I2CD1);

    txbuf[0] = 0x19; //Registre
    txbuf[1] = 0x07; //Donn�e
    i2cAcquireBus(&I2CD1); //R�glage de la fr�quence d'�chantillonage (125Hz)
    i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);
    i2cReleaseBus(&I2CD1);

    txbuf[0] = 0x1A; //Registre
    txbuf[1] = 0x06; //Donn�e
    i2cAcquireBus(&I2CD1); //R�glage de la fr�quence du filtre passe-bas (5Hz)
    i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);
    i2cReleaseBus(&I2CD1);

    txbuf[0] = 0x1B; //Registre
    txbuf[1] = 0x18; //Donn�e
    i2cAcquireBus(&I2CD1);
    i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000); //Pas vraiment compris� Correspondrait � la plage de mesure de l�acc�l�rom�tre�
    i2cReleaseBus(&I2CD1);

    txbuf[0] = 0x1C; //Registre
    txbuf[1] = 0x01; //Donn�e
    i2cAcquireBus(&I2CD1); //Pas vraiment compris� Gamme d'acc�l�rom�tre d'auto-test et mesure la fr�quence du filtre passe-haut, les valeurs typiques: 0x01 (non auto, 2G, 5 Hz)
    i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 2, NULL, 0, 1000);
    i2cReleaseBus(&I2CD1);

    while(TRUE)
    {
//r�cup�ration du tangage
        txbuf[0] = 0x3B; //Registre de GYRO_XOUT_H i2cAcquireBus(&I2CD1);
        i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 1, rxbuf, 2, 1000);
        i2cReleaseBus(&I2CD1); //R�union de l'octet de poids faible avec l'octet de poids fort
        tangage = rxbuf[0]<<8;
        tangage = tangage | (unsigned int)rxbuf[1];
//Conversion de l'angle
        if (tangage < 0xFFFF && tangage > 0xBFFF) //Si compris entre 49151 et 65535�
        {
            tangage = (tangage - 0xBFFF)/0xB6-0x5A; //�On divise par 182 (rapport entre la valeur brute et l�angle en degr�) et on soustrait 90
        }
        else if (tangage > 0x0000 && tangage < 0x4000) //Si compris entre 0 et 16384�
        {
            tangage = tangage/0xB6; //�On divise par 182
        }
        else // angle = 0
        {
            tangage = 0;
        }
//r�cup�ration du roulis
        txbuf[0] = 0x3D; //Registre de GYRO_YOUT_Y
        i2cAcquireBus(&I2CD1);
        i2cMasterTransmitTimeout(&I2CD1, 0x68, txbuf, 1, rxbuf, 2, 1000);
        i2cReleaseBus(&I2CD1); //R�union de l'octet de poids faible et de l'octet de poids fort
        roulis = rxbuf[0]<<8;
        roulis = roulis | (unsigned int)rxbuf[1]; //Conversion de l'angle
        if (roulis < 0xFFFF && roulis > 0xBFFF)
        {
            roulis = (roulis - 0xBFFF)/0xB6-0x5A;
        }
        else if (roulis > 0x0000 && roulis < 0x4000)
        {
            roulis = roulis/0xB6;
        }
        else // angle = 0
        {
            roulis = 0;

        }
//Stockage dans des variables globales
        g_roulis = roulis;
        g_tangage = tangage;
//R�initialisation des valeurs
        tangage = 0;
        roulis = 0;
    }
}
