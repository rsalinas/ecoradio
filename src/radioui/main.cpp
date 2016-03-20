#include "radioconsole.h"
#include "connectdialog.h"
#include <QApplication>

#include <csignal>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    
//    RadioConsole w;
//    w.show();
    ConnectDialog d;
    d.show();

    return a.exec();
}
