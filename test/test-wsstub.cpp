#include "gtest/gtest.h"

#include <QObject>
#include <QCoreApplication>
#include <QDebug>

#include "common/common.h"
#include "wsstub/radiostub.h"
#include "core/ecoradio.h"

class StubFixture : public QObject , public ::testing::Test {
    Q_OBJECT
public:
    StubFixture() :
        stub("ws://localhost:"+QString::number(c_ecoRadioWsPort )) {
QTimer::singleShot(2000, this, SLOT(timeout()));
    }

    void TearDown() {
        qDebug() << __FUNCTION__;
    }
public slots:
    void vuMeterUpdate(int c,int v) {
        qDebug() << __FUNCTION__ << c << v;

    }
    void timeout() {
        qDebug() << "timeout";
        QCoreApplication::instance()->exit(0);
    }

protected:
    RadioStub stub;
    bool finished = false;
};

TEST_F(StubFixture, Test0) {
    QObject::connect(&stub, SIGNAL(vuMeterUpdate(int,int)), this, SLOT(vuMeterUpdate(int,int)));

    qDebug() << "waiting...";
        QCoreApplication::instance()->exec();

}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Ecoradio * ecoradio = new Ecoradio(&app);

    // This will run the task from the application event loop.
    QTimer::singleShot(0, ecoradio, SLOT(run()));
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include "test-wsstub.moc"
