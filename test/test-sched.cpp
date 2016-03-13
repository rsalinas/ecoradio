#include "gtest/gtest.h"

#include "core/scheduler.h"
#include <QCoreApplication>

Scheduler sched("radio.sqlite");

TEST(TestSched, TestSched)
{
    auto current = sched.getCurrent();
    ASSERT_NE(nullptr, current.get());
    qDebug() << *current;
    auto next = sched.getNext();
    qDebug() << next.size();
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
