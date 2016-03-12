#include "radioconsole.h"
#include <QApplication>

#include <csignal>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    
    RadioConsole w;
    w.show();

    return a.exec();
}
