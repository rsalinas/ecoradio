#include <QObject>
#include <QCoreApplication>
#include <gtest/gtest.h>
#include <QTimer>


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
        QCoreApplication::instance()->exit(ret);
    }

private:
    QCoreApplication app;
};


int main(int argc, char *argv[])
{
    Wrapper wrapper(argc, argv);
    return QCoreApplication::instance()->exec();
}


#include  "testutil.moc"
