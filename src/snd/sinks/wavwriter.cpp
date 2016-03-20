#include "wavwriter.h"

#include <QDebug>

WavWriter::WavWriter(const QString &filename)
    : m_sf((filename+"."+getExtension()).toStdString().c_str(),
           SFM_WRITE,
           SF_FORMAT_WAV | SF_FORMAT_PCM_16,
           2,
           44100)
{

}

bool WavWriter::writePcm(char *buffer, int length)
{
    int n = m_sf.write(reinterpret_cast<short*>(buffer), length/sizeof(short));
    if (n / length/sizeof(short)) {
        qWarning() << "could not write all audio";
        return false;
    }
    return true;

}

WavWriter::~WavWriter() {
}
