#include "fifo.h"

#include <QDebug>

Fifo::Fifo(size_t bufsize) : m_cb(bufsize)
{
    qDebug() << "FIFO initialized with buffer: "<< bufsize;
    this->setOpenMode(ReadWrite);
}

Fifo::~Fifo() {
}

qint64 Fifo::readData(char *data, qint64 maxlen)
{
    qDebug() << "Fifo::readData" << maxlen;
    QMutexLocker lock(&m_mutex);
    while (maxlen) {
        int n = m_cb.readData(data, maxlen);
        m_condvar.wakeAll();
        maxlen -= n;
        data += n;
        while (n==0) {
            qDebug() << "waiting for data n==" << n << "av: " << bytesAvailable();
            m_condvar.wait(&m_mutex);
        }
    }
    return maxlen;
}

qint64 Fifo::writeData(const char *data, qint64 len)
{
    size_t written = 0;
    qDebug() << "Fifo::writeData pre" << len << "av: "<< bytesAvailable();
    QMutexLocker lock(&m_mutex);
    while (len) {
        int n = m_cb.writeData(data, len);
        qDebug() << "Fifo::writeData done" << len << n << bytesAvailable() ;
        written += n;
        len -= n;
        data += n;
        m_condvar.wakeAll();
        while (n==0) {
            qDebug() << __FUNCTION__ << "FIFO lleno, waiting";
            m_condvar.wait(&m_mutex);
        }
    }
    return written;
}



/***********************/


CircularBuffer::CircularBuffer(size_t capacity)
    : beg_index_(0)
    , end_index_(0)
    , size_(0)
    , capacity_(capacity)
     , data_(new char[capacity])
{
    ;
}

CircularBuffer::~CircularBuffer()
{
    delete [] data_;
}

qint64 CircularBuffer::writeData(const char *data, qint64  bytes)
{
    if (bytes == 0) return 0;

    qint64  capacity = capacity_;
    qint64  bytes_to_write = std::min(bytes, capacity - size_);

    // Write in a single step
    if (bytes_to_write <= capacity - end_index_)
    {
        memcpy(data_ + end_index_, data, bytes_to_write);
        end_index_ += bytes_to_write;
        if (end_index_ == capacity) end_index_ = 0;
    }
    // Write in two steps
    else
    {
        size_t size_1 = capacity - end_index_;
        memcpy(data_ + end_index_, data, size_1);
        size_t size_2 = bytes_to_write - size_1;
        memcpy(data_, data + size_1, size_2);
        end_index_ = size_2;
    }

    size_ += bytes_to_write;
    return bytes_to_write;
}

qint64 CircularBuffer::readData(char *data, qint64  bytes)
{
    if (bytes == 0) return 0;

    size_t capacity = capacity_;
    size_t bytes_to_read = std::min(bytes, size_);

    // Read in a single step
    if (bytes_to_read <= capacity - beg_index_)
    {
        memcpy(data, data_ + beg_index_, bytes_to_read);
        beg_index_ += bytes_to_read;
        if (beg_index_ == capacity) beg_index_ = 0;
    }
    // Read in two steps
    else
    {
        size_t size_1 = capacity - beg_index_;
        memcpy(data, data_ + beg_index_, size_1);
        size_t size_2 = bytes_to_read - size_1;
        memcpy(data + size_1, data_, size_2);
        beg_index_ = size_2;
    }

    size_ -= bytes_to_read;
    return bytes_to_read;
}
