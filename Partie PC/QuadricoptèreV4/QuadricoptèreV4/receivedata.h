#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include <QThread>
#include "Globales.h"

class receiveData : public QThread
{
    Q_OBJECT

private:
    Data sesDonneesRecues;
    void run();
public:
    receiveData();
signals:
    void displayData(Data desDonneesRecues);

};

#endif // RECEIVEDATA_H
