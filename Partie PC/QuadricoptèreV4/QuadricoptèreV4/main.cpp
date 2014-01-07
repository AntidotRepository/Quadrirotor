#include <QApplication>
#include "interface.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    interface *uneInterface = new interface();
    //uneInterface->show();
    return app.exec();
}
