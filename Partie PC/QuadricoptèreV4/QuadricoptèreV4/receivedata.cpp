#include "receivedata.h"

/*
 * Faire une structure qui contiendra les données
 */

receiveData::receiveData()
{
}

void receiveData::run()
{
    //lire les données sur le port COM
    //les extraires
    //les passer à l'interface graphique
    emit displayData(sesDonneesRecues);
}
