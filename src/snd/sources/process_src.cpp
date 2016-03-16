#include "process_src.h"

#include <QDebug>

ProcessDecoder::ProcessDecoder(const QString &name, const QString& binary, QStringList args) : SoundSource(name) {
    m_process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
    m_process.setReadChannel(QProcess::StandardOutput);

    m_process.start(binary, args);
    if (!m_process.waitForStarted(1000)) {
        qDebug() << "error starting " << binary << args;
        throw Exception();
    }
}

bool ProcessDecoder::waitForReadyReady(int ms) {
    return m_process.waitForReadyRead(ms);
}


ProcessDecoder::~ProcessDecoder() {
    qDebug() << "killing process" << name();
    m_process.terminate();
    qDebug() << name() << "waitForFinished";
    m_process.waitForFinished(1000);
    qDebug() << name() << "waitForFinished done";
}


int ProcessDecoder::readPcm(char * buf, const size_t length) {

    if (m_status == Finished)
        return -1;

    if (m_status == Paused)
        return 0;

    if (m_process.bytesAvailable() < length) {
        qDebug() << "ProcessDecoder::readPcm: m_process.bytesAvailable() < length: "
                 << m_process.bytesAvailable() << "<" << length;
        return 0;
    }

    ssize_t remaining = length;
    do  {
        if (!m_process.waitForReadyRead(3000)) {
            qDebug() << "mp3: no data in time";
        }
        auto ret = m_process.read(buf, remaining);
        if (ret < 0) {
            qDebug() << "end of file";
            memset(buf, 0, remaining);
            if (length-remaining)
                return length-remaining;
            else
                return -1;
        }
        if (ret > 0) {
            remaining -= ret;
            buf+= ret;
        }

    } while (remaining > 0);

    return length;
}
