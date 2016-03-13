#include "gtest/gtest.h"

#include <QDebug>
#include "util/fifo.h"
#include <QDebug>

TEST(FifoTest, Fifo)
{
    size_t fifoSize = 100;
    const size_t writeBlockSize = 17;
    const size_t readBlockSize = 13;

    CircularBuffer fifo(fifoSize);
    ASSERT_EQ(fifoSize, fifo.size());

    int wData = 0;
    int rData = 0;
    for (int i = 0; i < 1000; ++i) {
        while (fifo.bytesAvailable()  < readBlockSize) {
            std::vector<char> writeVector(writeBlockSize);
            for (size_t i = 0; i < writeVector.size(); ++i) {
                writeVector[i] = char(wData++);
            }
            auto availableBefore = fifo.bytesAvailable();
            auto written = fifo.writeData(&writeVector[0], writeBlockSize);
            wData -= writeBlockSize-written;
            ASSERT_EQ(availableBefore + written, fifo.bytesAvailable());
        }
        auto availableBefore = fifo.bytesAvailable();
        char buf2[readBlockSize];
        auto n = fifo.readData(buf2, readBlockSize);
        for (int j = 0; j < n; ++j) {
            ASSERT_EQ(char(rData++), buf2[j]);
        }
        ASSERT_EQ(availableBefore - n, fifo.bytesAvailable());
    }

}


