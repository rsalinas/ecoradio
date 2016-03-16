#pragma once


#include "soundsource.h"

class ProcessDecoder : public SoundSource {
public:
    ProcessDecoder(const QString &name, const QString& binary, QStringList args);
    virtual ~ProcessDecoder();
    virtual int readPcm(char * buf, const size_t length);
    bool waitForReadyReady(int ms = 0);

private:
    QProcess m_process;
};

class Mp3Decoder : public ProcessDecoder  {
public:
    Mp3Decoder(const QString &filename) : ProcessDecoder(filename, "mpg123", QStringList() << "-s" << "-r" << "44100" << "--stereo" << filename ) {

    }
};


class Arecord  : public ProcessDecoder {
public:
    Arecord() : ProcessDecoder("arecord", "arecord", QStringList() << "-f" << "cd" << "-t" << "raw") {
    }

};
