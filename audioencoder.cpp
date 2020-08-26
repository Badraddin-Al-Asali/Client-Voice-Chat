#include "audioencoder.h"

/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * audio encoding with libavcodec API example.
 *
 * @example encode_audio.c
 */





/* check that a given sample format is supported by the encoder */
static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

/* just pick the highest supported samplerate */
static int select_sample_rate(const AVCodec *codec)
{
    const int *p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p) {
        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
            best_samplerate = *p;
        p++;
    }
    return best_samplerate;
}

/* select layout with the highest channel count */
static int select_channel_layout(const AVCodec *codec)
{
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels   = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout    = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}

static QByteArray privateEncode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt)
{
    QByteArray encodedAudio;
    int ret;

    /* send the frame for encoding */
    ret = avcodec_send_frame(ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending the frame to the encoder\n");
        exit(1);
    }

    /* read all the available output packets (in general there may be any
     * number of them */
    while (ret >= 0)
    {
        ret = avcodec_receive_packet(ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return encodedAudio;
        else if (ret < 0) {
            fprintf(stderr, "Error encoding audio frame\n");
            exit(1);
        }
        encodedAudio.push_back(QByteArray((const char*)pkt->data, pkt->size));
    }
    return encodedAudio;
}

/*AudioEncoder::AudioEncoder(int argc, char **argv)
{
    const char *filename;
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <output file>\n", argv[0]);
        exit(1);
    }
    filename = argv[1];
    f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    f2 = fopen("Test.bin", "wb");
    if (!f2) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
}*/
AudioEncoder::AudioEncoder()
{

}

AudioEncoder::~AudioEncoder()
{
    /* flush the encoder */
    privateEncode(c, NULL, pkt);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&c);
    //fclose(f);
    //fclose(f2);
}

QByteArray AudioEncoder::encode(QByteArray& input)
{
    QByteArray ret;
    fifo.push_back(input);
    while(fifo.size() - maxFrameSize >= 0)
    {
        /* make sure the frame is writable -- makes a copy if the encoder
         * kept a reference internally */
        int isFrameWritable = av_frame_make_writable(frame);
        if (isFrameWritable < 0)
            exit(1);
        uint16_t *dst((uint16_t*)frame->data[0]), *src((uint16_t*)fifo.data());
        memcpy(dst, src, maxFrameSize);
        //fwrite(frame->data[0], 1, maxFrameSize, f2);
        ret.push_back(privateEncode(c, frame, pkt));
        fifo.remove(0, maxFrameSize);
    }
    //fwrite(ret.data(), 1, ret.size(), f);
    return ret;
}

int AudioEncoder::setAudio(const int& sampleRate, const int& channel_layout, const int& byteSize)
{
    const AVCodec *codec;
    c= NULL;
    int i, j, k, ret;

    uint16_t *samples;
    float t, tincr;

    /* find the MP2 encoder */
    codec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    /* put sample parameters */
    c->bit_rate = 64000;

    /* check that the encoder supports s16 pcm input */
    c->sample_fmt = AV_SAMPLE_FMT_S16;
    if (!check_sample_fmt(codec, c->sample_fmt)) {
        fprintf(stderr, "Encoder does not support sample format %s",
                av_get_sample_fmt_name(c->sample_fmt));
        exit(1);
    }

    /* select other audio parameters supported by the encoder */
    /*c->sample_rate    = select_sample_rate(codec);
    c->channel_layout = select_channel_layout(codec);
    c->channels       = av_get_channel_layout_nb_channels(c->channel_layout);*/
    c->sample_rate    = sampleRate;
    c->channel_layout = AV_CH_LAYOUT_MONO;
    c->channels       = av_get_channel_layout_nb_channels(c->channel_layout);

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    /* packet for holding encoded output */
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "could not allocate the packet\n");
        exit(1);
    }

    /* frame containing input raw audio */
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate audio frame\n");
        exit(1);
    }

    frame->nb_samples     = c->frame_size;
    frame->format         = c->sample_fmt;
    frame->channel_layout = c->channel_layout;
    maxFrameSize = c->frame_size * 2;
    /* allocate the data buffers */
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate audio data buffers\n");
        exit(1);
    }
    return 0;
}







/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * audio decoding with libavcodec API example
 *
 * @example decode_audio.c
 */

static void decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame,
                   FILE *outfile)
{
    int i, ch;
    int ret, data_size;

    /* send the packet with the compressed data to the decoder */
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error submitting the packet to the decoder\n");
        exit(1);
    }

    /* read all the output frames (in general there may be any number of them */
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
            fprintf(stderr, "Failed to calculate data size\n");
            exit(1);
        }
        for (i = 0; i < frame->nb_samples; i++)
            for (ch = 0; ch < dec_ctx->channels; ch++)
                fwrite(frame->data[ch] + data_size*i, 1, data_size, outfile);
    }
}

int AudioEncoder::testDecode(int argc, char **argv)
{
    const char *outfilename, *filename;
    const AVCodec *codec;
    AVCodecContext *decodeC= NULL;
    AVCodecParserContext *parser = NULL;
    int len, ret;
    FILE *f, *outfile;
    uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t   data_size;
    AVPacket *pkt;
    AVFrame *decoded_frame = NULL;

    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(0);
    }
    filename    = argv[1];
    outfilename = argv[2];

    pkt = av_packet_alloc();

    /* find the MPEG audio decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_MP2);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        fprintf(stderr, "Parser not found\n");
        exit(1);
    }

    decodeC = avcodec_alloc_context3(codec);
    if (!decodeC) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    /* open it */
    if (avcodec_open2(decodeC, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    outfile = fopen(outfilename, "wb");
    if (!outfile) {
        av_free(decodeC);
        exit(1);
    }

    /* decode until eof */
    data      = inbuf;
    data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);

    while (data_size > 0) {
        if (!decoded_frame) {
            if (!(decoded_frame = av_frame_alloc())) {
                fprintf(stderr, "Could not allocate audio frame\n");
                exit(1);
            }
        }

        ret = av_parser_parse2(parser, decodeC, &pkt->data, &pkt->size,
                               data, data_size,
                               AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            fprintf(stderr, "Error while parsing\n");
            exit(1);
        }
        data      += ret;
        data_size -= ret;

        if (pkt->size)
            decode(decodeC, pkt, decoded_frame, outfile);

        if (data_size < AUDIO_REFILL_THRESH) {
            memmove(inbuf, data, data_size);
            data = inbuf;
            len = fread(data + data_size, 1,
                        AUDIO_INBUF_SIZE - data_size, f);
            if (len > 0)
                data_size += len;
        }
    }

    /* flush the decoder */
    pkt->data = NULL;
    pkt->size = 0;
    decode(decodeC, pkt, decoded_frame, outfile);

    fclose(outfile);
    fclose(f);

    avcodec_free_context(&decodeC);
    av_parser_close(parser);
    av_frame_free(&decoded_frame);
    av_packet_free(&pkt);

    return 0;
}


