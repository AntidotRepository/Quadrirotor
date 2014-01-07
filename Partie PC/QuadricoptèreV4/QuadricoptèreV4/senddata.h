#ifndef SENDDATA_H
#define SENDDATA_H

#include <QThread>

class sendData : public QThread
{
private:
    void run();
public:
    sendData();
};

#endif // SENDDATA_H
