#include "gtest/gtest.h"

#include "core/scheduler.h"
#include <QCoreApplication>

Scheduler sched("/home/rsalinas/Sync/Projects/ecoradio2/radio.sqlite");

TEST(TestSched, TestSched)
{
    sched.getCurrent();
    sched.getNext();
}
