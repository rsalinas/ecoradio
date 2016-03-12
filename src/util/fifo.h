#pragma once

#include <QIODevice>
#include <QThread>
#include <QQueue>
#include <algorithm> // for std::min
#include <QMutex>
#include <QWaitCondition>

//from http://asawicki.info/news_1468_circular_buffer_of_raw_binary_data_in_c.html
class CircularBuffer
{
public:
  CircularBuffer(size_t capacity);
  ~CircularBuffer();

  size_t size() const { return size_; }
  size_t capacity() const { return capacity_; }
  size_t write(const char *data, size_t bytes);
  size_t read(char *data, size_t bytes);  

private:
  size_t beg_index_, end_index_;
  size_t size_;
  const size_t capacity_;
  char *const data_;
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
    qint64 bytesAvailable() const override {
        return m_cb.size();
    }

private:
    QMutex m_mutex;
    QWaitCondition m_condvar;
    CircularBuffer m_cb;

};
