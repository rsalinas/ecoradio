
#include "aosink.h"
#include <string.h>

#include <QDebug>

AoSink::AoSink(const Format &format)
{
    ao_initialize();
    /* -- Setup for default driver -- */

    int default_driver = ao_default_driver_id();
    memset(&m_format, 0, sizeof(m_format));
    m_format.bits = format.sampleSizeBits;
    m_format.channels = format.channels;
    m_format.rate = format.rate;
    m_format.byte_format = AO_FMT_LITTLE; //FIXME

    device = ao_open_live(default_driver, &m_format, NULL /* no options */);
    if (device == NULL) {
        fprintf(stderr, "Error opening device.\n");
        throw AoSinkException();
    }

}

AoSink::~AoSink()
{
    ao_close(device);
    ao_shutdown();
}


bool AoSink::writePcm(char *buffer, int length) {
    if (1 != ao_play(device, buffer, length)) {
        return true;
    }
    return false;
}
