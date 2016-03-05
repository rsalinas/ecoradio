#include "pcm_player.h"
#include <cstring>
#include <QDebug>
#include <climits>

PcmPlayer::PcmPlayer() {
    ao_initialize();
    /* -- Setup for default driver -- */

    int default_driver = ao_default_driver_id();    
    memset(&format, 0, sizeof(format));
    format.bits = 16;
    format.channels = 2;
    format.rate = 44100;
    format.byte_format = AO_FMT_LITTLE;

    buf_size = format.bits/8 * format.channels * format.rate;
    buffer = static_cast<char*>(calloc(buf_size, sizeof(char)));

    /* -- Open driver -- */
    device = ao_open_live(default_driver, &format, NULL /* no options */);
    if (device == NULL) {
        fprintf(stderr, "Error opening device.\n");
        throw new PcmPlayerException;
    }

}

PcmPlayer::~PcmPlayer() {
    if (buffer)
        free(buffer);
    /* -- Close and shutdown -- */
    ao_close(device);

    ao_shutdown();

}

int PcmPlayer::add(char * newBuffer) {
    auto b = reinterpret_cast<signed short int *>(buffer);
    auto b1 = reinterpret_cast<signed short int *>(newBuffer);
    for (int i=0; i < buf_size/ (format.bits/8 ); i++) {
        int sample = b[i] + b1[i];
        if (sample > SHRT_MAX)
            sample=SHRT_MAX;
        if (sample < SHRT_MIN)
            sample = SHRT_MIN;
        b[i] = (short int) sample;
    }
}


size_t PcmPlayer::write(char * buf, int len) {
    if (!ao_play(device, buf, len)) {
        throw new PcmPlayerException;
    }

}
