#ifndef INTERFACE_H
#define INTERFACE_H
#include <QHBoxLayout>
#include <QGridLayout>
#include <QImage>
#include <QMatrix>
#include <QLabel>
#include <QPushButton>
#include <QTransform>
#include <QMenuBar>
#include "qextserialport.h"
#include <QMainWindow>
#include "liaisonserie.h"
#include <QWidget>
#include <QApplication>
#include <QComboBox>

class interface : public QWidget
{
    Q_OBJECT
public:
    interface();

signals:

public slots:
    void slotliaisonSerie();
};

#endif // INTERFACE_H
