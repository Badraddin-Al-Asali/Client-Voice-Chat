#ifndef AUDIOENCODER_H
#define AUDIOENCODER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/common.h>
    #include <libavutil/frame.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/mem.h>
}

#include <QByteArray>
#include <QDebug>

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

class AudioEncoder
{
public:
    //AudioEncoder(int argc, char **argv);
    AudioEncoder();
    ~AudioEncoder();
    QByteArray encode(QByteArray& input);
    int setAudio(const int& sampleRate, const int& channel_layout, const int& byteSize);
    int testDecode(int argc, char **argv);
private:
    AVFrame *frame;
    AVPacket *pkt;
    AVCodecContext *c;
    //FILE *f, *f2;
    QByteArray fifo;
    int maxFrameSize;
};

#endif // AUDIOENCODER_H
