#pragma once

#include "sinks/sink.h"
#include <memory>


#include <vorbis/vorbisenc.h>

class QIODevice;


class OggEncoder : public SndSink
{
public:
    OggEncoder(std::unique_ptr<QIODevice> output);
    ~OggEncoder();

    bool writePcm(char * buffer, int length) override;
    class OggEncoderException : public std::exception {
    };
private:
    int commit();

    ogg_stream_state os; /* take physical pages, weld into a logical
                            stream of packets */
    ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
    ogg_packet       op; /* one raw packet of data for decode */

    vorbis_info      vi; /* struct that stores all the static vorbis bitstream
                            settings */
    vorbis_comment   vc; /* struct that stores all the user comments */

    vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
    vorbis_block     vb; /* local working space for packet->PCM decode */

    int eos=0;

    std::unique_ptr<QIODevice> m_output;
};
