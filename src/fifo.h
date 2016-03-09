#pragma once

#include <QIODevice>
#include <QThread>
#include <QQueue>
#include <algorithm> // for std::min
#include <QMutex>
#include <QWaitCondition>


class CircularBuffer
{
public:
  CircularBuffer(size_t capacity);
  ~CircularBuffer();

  size_t size() const { return size_; }
  size_t capacity() const { return capacity_; }
  // Return number of bytes written.
  size_t write(const char *data, size_t bytes);
  // Return number of bytes read.
  size_t read(char *data, size_t bytes);

private:
  size_t beg_index_, end_index_, size_, capacity_;
  char *data_;
};



class Fifo : public QIODevice, public QThread {
public:
    Fifo(size_t bufsize = 1024*512);
    ~Fifo();
    virtual qint64 readData(char *data, qint64 maxlen) override ;
    virtual qint64 writeData(const char *data, qint64 len) override ;
    bool isSequential() const override {
        return true;
    }

private:
    QMutex m_mutex;
    QWaitCondition m_condvar;
    CircularBuffer m_cb;

};
