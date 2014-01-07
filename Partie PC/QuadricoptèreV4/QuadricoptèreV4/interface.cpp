#include "interface.h"

interface::interface() : QWidget()
{
    QWidget *centralWidget = new QWidget(this);
    QMainWindow *fenetre = new QMainWindow(this);
    QMenu *menuParametres = new QMenu("Paramètres");
    QAction *setLiaisonSerie = new QAction("Liaison série", menuParametres);
    liaisonSerie *uneLiaisonSerie = new liaisonSerie();
    QObject::connect(setLiaisonSerie, SIGNAL(triggered()), this, SLOT(slotliaisonSerie()));
    menuParametres->addAction(setLiaisonSerie);
    fenetre->menuBar()->addMenu(menuParametres);

    QGridLayout *mainLayout = new QGridLayout();
    QHBoxLayout *lay_RealPosition = new QHBoxLayout();
    QHBoxLayout *lay_SourisPosition = new QHBoxLayout();
    QHBoxLayout *lay_Backgrounds = new QHBoxLayout();

    QLabel *lab_TangageBG = new QLabel();
    QLabel *lab_Tangage = new QLabel();
    QLabel *lab_TangageSouris = new QLabel();
    QLabel *lab_RoulisBG = new QLabel();
    QLabel *lab_Roulis = new QLabel();
    QLabel *lab_RoulisSouris = new QLabel();
    QLabel *lab_LacetBG = new QLabel();
    QLabel *lab_Lacet = new QLabel();
    QLabel *lab_LacetSouris = new QLabel();

    QImage *ima_TangageBG = new QImage("res/TangageBG.png");
    QImage *ima_Tangage = new QImage("res/AvTangage.png");
    QImage *ima_TangageSouris = new QImage("res/AvTangageSouris.png");
    QImage *ima_trans_Tangage = new QImage();
    QImage *ima_trans_TangageSouris = new QImage();

    QImage *ima_RoulisBG = new QImage("res/RoulisBG.png");
    QImage *ima_Roulis = new QImage("res/AvRoulis.png");
    QImage *ima_RoulisSouris = new QImage("res/AvRoulisSouris.png");
    QImage *ima_trans_Roulis = new QImage();
    QImage *ima_trans_RoulisSouris = new QImage();

    QImage *ima_LacetBG = new QImage("res/LacetBG.png");
    QImage *ima_Lacet = new QImage("res/AvLacet.png");
    QImage *ima_LacetSouris = new QImage("res/AvLacetSouris.png");
    QImage *ima_trans_Lacet = new QImage();
    QImage *ima_trans_LacetSouris = new QImage();

    QTransform myTransform;
    myTransform.rotate(90);

    lab_TangageBG->setPixmap(QPixmap::fromImage(*ima_TangageBG));
    lay_Backgrounds->addWidget(lab_TangageBG);
    *ima_trans_Tangage = ima_Tangage->transformed(myTransform, Qt::SmoothTransformation);
    lab_Tangage->setPixmap(QPixmap::fromImage(*ima_trans_Tangage));
    lay_RealPosition->addWidget(lab_Tangage);
    *ima_trans_TangageSouris = ima_TangageSouris->transformed(myTransform, Qt::SmoothTransformation);
    lab_TangageSouris->setPixmap(QPixmap::fromImage(*ima_trans_TangageSouris));
    lay_SourisPosition->addWidget(lab_TangageSouris);

    lab_RoulisBG->setPixmap(QPixmap::fromImage(*ima_RoulisBG));
    lay_Backgrounds->addWidget(lab_RoulisBG);
    *ima_trans_Roulis = ima_Roulis->transformed(myTransform, Qt::SmoothTransformation);
    lab_Roulis->setPixmap(QPixmap::fromImage(*ima_trans_Roulis));
    lay_RealPosition->addWidget(lab_Roulis);
    *ima_trans_RoulisSouris = ima_RoulisSouris->transformed(myTransform, Qt::SmoothTransformation);
    lab_RoulisSouris->setPixmap(QPixmap::fromImage(*ima_trans_RoulisSouris));
    lay_SourisPosition->addWidget(lab_RoulisSouris);

    lab_LacetBG->setPixmap(QPixmap::fromImage(*ima_LacetBG));
    lay_Backgrounds->addWidget(lab_LacetBG);
    *ima_trans_Lacet = ima_Lacet->transformed(myTransform, Qt::SmoothTransformation);
    lab_Lacet->setPixmap(QPixmap::fromImage(*ima_trans_Lacet));
    lay_RealPosition->addWidget(lab_Lacet);
    *ima_trans_LacetSouris = ima_LacetSouris->transformed(myTransform, Qt::SmoothTransformation);
    lab_LacetSouris->setPixmap(QPixmap::fromImage(*ima_trans_LacetSouris));
    lay_SourisPosition->addWidget(lab_LacetSouris);

    mainLayout->addLayout(lay_Backgrounds,0,0);
    mainLayout->addLayout(lay_RealPosition,0,0);
    mainLayout->addLayout(lay_SourisPosition,0,0);

    centralWidget->setLayout(mainLayout);
    fenetre->setCentralWidget(centralWidget);
    fenetre->show();
}

void interface::slotliaisonSerie()
{
    QWidget *wid_serialCom = new QWidget();
    QVBoxLayout *l_ComboBox = new QVBoxLayout();
    QVBoxLayout *l_Labels = new QVBoxLayout();
    QHBoxLayout *l_Params = new QHBoxLayout();
    QHBoxLayout *l_Confirmation = new QHBoxLayout();
    QVBoxLayout *l_global = new QVBoxLayout();

    QComboBox *Combo_ComPort = new QComboBox();
    QComboBox *Combo_Baudrate = new QComboBox();
    QComboBox *Combo_DataSize = new QComboBox();
    QComboBox *Combo_Parity = new QComboBox();
    QComboBox *Combo_Handshake = new QComboBox();

    QStringList liste_ComPort;
    liste_ComPort<<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"<<"COM7"<<"COM8"<<"COM9"<<"COM10"<<"COM11"<<"COM12"<<"COM13"<<"COM14"<<"COM15"<<"COM16"<<"COM18"<<"COM19";
    Combo_ComPort->addItems(liste_ComPort);

    QStringList liste_Baudrate;
    liste_Baudrate<<"600"<<"1200"<<"2400"<<"4800"<<"9600"<<"14400"<<"19200"<<"38400"<<"56000"<<"57600"<<"115200";
    Combo_Baudrate->addItems(liste_Baudrate);

    QStringList liste_DataSize;
    liste_DataSize<<"7"<<"8";
    Combo_DataSize->addItems(liste_DataSize);

    QStringList liste_Parity;
    liste_Parity<<"none"<<"even"<<"odd"<<"mark";
    Combo_Parity->addItems(liste_Parity);

    QStringList liste_Handshake;
    liste_Handshake<<"Off"<<"RTS/CTS"<<"Xon/Xoff";
    Combo_Handshake->addItems(liste_Handshake);

    QLabel *label_ComPort = new QLabel("Port Com:");
    QLabel *label_Baudrate = new QLabel("Baudrate:");
    QLabel *label_DataSize = new QLabel("Taille Données:");
    QLabel *label_Parity = new QLabel("Parité:");
    QLabel *label_Handshake = new QLabel("Handshake:");

    l_Labels->addWidget(label_ComPort);
    l_Labels->addWidget(label_Baudrate);
    l_Labels->addWidget(label_DataSize);
    l_Labels->addWidget(label_Parity);
    l_Labels->addWidget(label_Handshake);

    l_ComboBox->addWidget(Combo_ComPort);
    l_ComboBox->addWidget(Combo_Baudrate);
    l_ComboBox->addWidget(Combo_DataSize);
    l_ComboBox->addWidget(Combo_Parity);
    l_ComboBox->addWidget(Combo_Handshake);

    l_Params->addLayout(l_Labels);
    l_Params->addLayout(l_ComboBox);

    QPushButton *b_Ok = new QPushButton("Ok");
    QPushButton *b_Cancel = new QPushButton("Annuler");

    l_Confirmation->addWidget(b_Ok);
    l_Confirmation->addWidget(b_Cancel);

    l_global->addLayout(l_Params);
    l_global->addLayout(l_Confirmation);
    wid_serialCom->setLayout(l_global);
    wid_serialCom->show();
}
