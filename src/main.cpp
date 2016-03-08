#include <QCoreApplication>
#include "ecoradio.h"
#include <QTimer>

int main(int argc, char *argv[])
{    
    QCoreApplication app(argc, argv);
    Ecoradio * ecoradio = new Ecoradio(&app);

    // This will cause the application to exit when
    // the task signals finished.
    QObject::connect(ecoradio, SIGNAL(finished()), &app, SLOT(quit()));

    // This will run the task from the application event loop.
    QTimer::singleShot(0, ecoradio, SLOT(run()));

    return app.exec();
}
