#include "dtmfmagix.h"
#include <QApplication>
#include <thread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DTMFMagix w;
    w.show();
    return a.exec();
}
