#pragma once

#include <QIODevice>
#include <QThread>

class Fifo : public QIODevice, public QThread {
public:
    virtual qint64 readData(char *data, qint64 maxlen);
 virtual qint64 writeData(const char *data, qint64 len);

};
