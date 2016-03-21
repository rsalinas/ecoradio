
#include <QApplication>

#include "radioui/radioconsole.h"
#include "radioui/connectdialog.h"
#include "core/ecoradio.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    Ecoradio * ecoradio = new Ecoradio(&app);

    // This will cause the application to exit when
    // the task signals finished.
    QObject::connect(ecoradio, SIGNAL(finished()), &app, SLOT(quit()));

    // This will run the task from the application event loop.
    QTimer::singleShot(0, ecoradio, SLOT(run()));

//    RadioConsole w;
    ConnectDialog w;
    w.show();

    return app.exec();
}
