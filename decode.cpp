

00238 static void audio_decode_example(const char *outfilename, const char *filename)
00239 {
00240     AVCodec *codec;
00241     AVCodecContext *c= NULL;
00242     int len;
00243     FILE *f, *outfile;
00244     uint8_t inbuf[AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
00245     AVPacket avpkt;
00246     AVFrame *decoded_frame = NULL;
00247 
00248     av_init_packet(&avpkt);
00249 
00250     printf("Decode audio file %s to %s\n", filename, outfilename);
00251 
00252     /* find the mpeg audio decoder */
00253     codec = avcodec_find_decoder(AV_CODEC_ID_MP2);
00254     if (!codec) {
00255         fprintf(stderr, "Codec not found\n");
00256         exit(1);
00257     }
00258 
00259     c = avcodec_alloc_context3(codec);
00260     if (!c) {
00261         fprintf(stderr, "Could not allocate audio codec context\n");
00262         exit(1);
00263     }
00264 
00265     /* open it */
00266     if (avcodec_open2(c, codec, NULL) < 0) {
00267         fprintf(stderr, "Could not open codec\n");
00268         exit(1);
00269     }
00270 
00271     f = fopen(filename, "rb");
00272     if (!f) {
00273         fprintf(stderr, "Could not open %s\n", filename);
00274         exit(1);
00275     }
00276     outfile = fopen(outfilename, "wb");
00277     if (!outfile) {
00278         av_free(c);
00279         exit(1);
00280     }
00281 
00282     /* decode until eof */
00283     avpkt.data = inbuf;
00284     avpkt.size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);
00285 
00286     while (avpkt.size > 0) {
00287         int got_frame = 0;
00288 
00289         if (!decoded_frame) {
00290             if (!(decoded_frame = avcodec_alloc_frame())) {
00291                 fprintf(stderr, "Could not allocate audio frame\n");
00292                 exit(1);
00293             }
00294         } else
00295             avcodec_get_frame_defaults(decoded_frame);
00296 
00297         len = avcodec_decode_audio4(c, decoded_frame, &got_frame, &avpkt);
00298         if (len < 0) {
00299             fprintf(stderr, "Error while decoding\n");
00300             exit(1);
00301         }
00302         if (got_frame) {
00303             /* if a frame has been decoded, output it */
00304             int data_size = av_samples_get_buffer_size(NULL, c->channels,
00305                                                        decoded_frame->nb_samples,
00306                                                        c->sample_fmt, 1);
00307             fwrite(decoded_frame->data[0], 1, data_size, outfile);
00308         }
00309         avpkt.size -= len;
00310         avpkt.data += len;
00311         avpkt.dts =
00312         avpkt.pts = AV_NOPTS_VALUE;
00313         if (avpkt.size < AUDIO_REFILL_THRESH) {
00314             /* Refill the input buffer, to avoid trying to decode
00315              * incomplete frames. Instead of this, one could also use
00316              * a parser, or use a proper container format through
00317              * libavformat. */
00318             memmove(inbuf, avpkt.data, avpkt.size);
00319             avpkt.data = inbuf;
00320             len = fread(avpkt.data + avpkt.size, 1,
00321                         AUDIO_INBUF_SIZE - avpkt.size, f);
00322             if (len > 0)
00323                 avpkt.size += len;
00324         }
00325     }
00326 
00327     fclose(outfile);
00328     fclose(f);
00329 
00330     avcodec_close(c);
00331     av_free(c);
00332     avcodec_free_frame(&decoded_frame);
00333 }
