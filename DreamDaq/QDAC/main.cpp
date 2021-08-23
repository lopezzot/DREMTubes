#include <QtGui/QApplication>
#include "daqcontrolinterface.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DaqControlInterface w;
    w.show();

    return a.exec();
}
