#pragma once

#include <QString>
#include <QObject>

class SoundSource : public QObject
{
public:
    virtual void play() = 0;
    virtual void stop() = 0;

    size_t readBytes(char * buf, int len) {

    }

protected:
    SoundSource(QObject *parent = NULL);

signals:
    void ending(int ttf);
};

class FileSoundSource : public SoundSource {
public:
    FileSoundSource(const QString &filename) {

    }
    void play();
    void stop();
};

class LiveSoundSource : public SoundSource {
public:
    void play();
    void stop();

};
