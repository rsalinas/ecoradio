#include <QObject>
#include <QCoreApplication>
#include <gtest/gtest.h>
#include <QTimer>
#include <QDebug>

class Wrapper : public QObject
{
    Q_OBJECT
public:
    Wrapper(int argc, char *argv[]) : app(argc, argv) {
        ::testing::InitGoogleTest(&argc, argv);
        QTimer::singleShot(0, this, SLOT(runTests()));
    }

private slots:
    void runTests() {
        int ret = RUN_ALL_TESTS();
//        QCoreApplication::instance()->exit(ret);
//        QCoreApplication::instance()->exit(app.exec());
    }

private:
    QCoreApplication app;
};


int main(int argc, char *argv[])
{
    qDebug() << "main test";
    Wrapper wrapper(argc, argv);
    return QCoreApplication::instance()->exec();
}


#include  "testutil.moc"
