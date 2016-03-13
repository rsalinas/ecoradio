#pragma once

#include <QIODevice>
#include <QThread>
#include <QQueue>
#include <algorithm> // for std::min
#include <QMutex>
#include <QWaitCondition>

//from http://asawicki.info/news_1468_circular_buffer_of_raw_binary_data_in_c.html
class CircularBuffer : public QIODevice
{
public:
  CircularBuffer(size_t size);
  ~CircularBuffer();

  qint64 bytesAvailable() const override { return size_; }
  qint64 size() const { return capacity_; }
  qint64 writeData(const char *data, qint64 bytes) override;
  qint64 readData(char *data, qint64 bytes) override;
  bool isSequential() const override {
      return true;
  }


private:
  size_t beg_index_, end_index_;
  qint64 size_;
  const qint64 capacity_;
  char *const data_;
};



class Fifo : public QIODevice, public QThread {
public:
    Fifo(size_t bufsize = 1024*512);
    ~Fifo();
    virtual qint64 readData(char *data, qint64 maxlen) override ;
    virtual qint64 writeData(const char *data, qint64 len) override ;
    qint64 bytesAvailable() const override {
        return m_cb.bytesAvailable();
    }

private:
    QMutex m_mutex;
    QWaitCondition m_condvar;
    CircularBuffer m_cb;

};
