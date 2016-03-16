#include "ogg_encoder.h"

#include <QDebug>
#include <QIODevice>

OggEncoder::OggEncoder(std::unique_ptr<QIODevice> output) :
    m_output(std::move(output)) {
    if (!m_output) {
        qWarning() << "null output given";
        throw OggEncoderException();
    }
    vorbis_info_init(&vi);

    /* choose an encoding mode.  A few possibilities commented out, one
       actually used: */

    /*********************************************************************
     Encoding using a VBR quality mode.  The usable range is -.1
     (lowest quality, smallest file) to 1. (highest quality, largest file).
     Example quality mode .4: 44kHz stereo coupled, roughly 128kbps VBR

     ret = vorbis_encode_init_vbr(&vi,2,44100,.4);

     ---------------------------------------------------------------------

     Encoding using an average bitrate mode (ABR).
     example: 44kHz stereo coupled, average 128kbps VBR

     ret = vorbis_encode_init(&vi,2,44100,-1,128000,-1);

     ---------------------------------------------------------------------

     Encode using a quality mode, but select that quality mode by asking for
     an approximate bitrate.  This is not ABR, it is true VBR, but selected
     using the bitrate interface, and then turning bitrate management off:

     ret = ( vorbis_encode_setup_managed(&vi,2,44100,-1,128000,-1) ||
             vorbis_encode_ctl(&vi,OV_ECTL_RATEMANAGE2_SET,NULL) ||
             vorbis_encode_setup_init(&vi));

     *********************************************************************/

    auto ret = vorbis_encode_init_vbr(&vi,2,44100,0.1);

    /* do not continue if setup failed; this can happen if we ask for a
       mode that libVorbis does not support (eg, too low a bitrate, etc,
       will return 'OV_EIMPL') */

    if(ret)exit(1);

    /* add a comment */
    vorbis_comment_init(&vc);
    vorbis_comment_add_tag(&vc,"ENCODER","encoder_example.c");
    vorbis_comment_add_tag(&vc, "ARTIST", "Artist"); //FIXME


    /* set up the analysis state and auxiliary encoding storage */
    vorbis_analysis_init(&vd,&vi);
    vorbis_block_init(&vd,&vb);

    /* set up our packet->stream encoder */
    /* pick a random serial number; that way we can more likely build
       chained streams just by concatenation */
    srand(time(NULL));
    ogg_stream_init(&os,rand());

    /* Vorbis streams begin with three headers; the initial header (with
       most of the codec setup parameters) which is mandated by the Ogg
       bitstream spec.  The second header holds any comment fields.  The
       third header holds the bitstream codebook.  We merely need to
       make the headers, then pass them to libvorbis one at a time;
       libvorbis handles the additional Ogg bitstream constraints */

    {
        ogg_packet header;
        ogg_packet header_comm;
        ogg_packet header_code;

        vorbis_analysis_headerout(&vd,&vc,&header,&header_comm,&header_code);
        ogg_stream_packetin(&os,&header); /* automatically placed in its own
                                           page */
        ogg_stream_packetin(&os,&header_comm);
        ogg_stream_packetin(&os,&header_code);

        /* This ensures the actual
       * audio data will start on a new page, as per spec
       */
        while(!eos){
            int result=ogg_stream_flush(&os,&og);
            if(result==0)break;
            m_output->write(reinterpret_cast<const char*>(og.header), og.header_len);
            m_output->write(reinterpret_cast<const char*>(og.body), og.body_len);

        }

    }

}

int OggEncoder::commit()
{
    /* vorbis does some data preanalysis, then divvies up blocks for
       more involved (potentially parallel) processing.  Get a single
       block for encoding now */
    while(vorbis_analysis_blockout(&vd,&vb)==1){

        /* analysis, assume we want to use bitrate management */
        vorbis_analysis(&vb,NULL);
        vorbis_bitrate_addblock(&vb);

        while(vorbis_bitrate_flushpacket(&vd,&op)){

            /* weld the packet into the bitstream */
            ogg_stream_packetin(&os,&op);

            /* write out pages (if any) */
            while(!eos){
                int result=ogg_stream_pageout(&os,&og);
                if(result==0)break;
                m_output->write(reinterpret_cast<const char*>(og.header), og.header_len);
                m_output->write(reinterpret_cast<const char*>(og.body), og.body_len);

                /* this could be set above, but for illustrative purposes, I do
             it here (to show that vorbis does know where the stream ends) */

                if(ogg_page_eos(&og))eos=1;
            }
        }
    }
    return 0; //?
}

bool OggEncoder::writePcm(char * readbuffer, int length) {
    /* expose the buffer to submit data */
    float **buffer=vorbis_analysis_buffer(&vd,length);

    /* uninterleave samples */
    int i;
    for(i=0;i<length/4;i++){
        buffer[0][i]=((readbuffer[i*4+1]<<8)|
                (0x00ff&(int)readbuffer[i*4]))/32768.f;
        buffer[1][i]=((readbuffer[i*4+3]<<8)|
                (0x00ff&(int)readbuffer[i*4+2]))/32768.f;
    }

    /* tell the library how much we actually submitted */
    vorbis_analysis_wrote(&vd,i);

    commit();
    return true;
}

OggEncoder::~OggEncoder() {
    vorbis_analysis_wrote(&vd,0);
    commit();

    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);
}
