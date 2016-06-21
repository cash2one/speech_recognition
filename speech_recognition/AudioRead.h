
#ifndef _AUDIO_READ_H_
#define _AUDIO_READ_H_
#include "Chain.h"


class AudioRead : public Chain
{
public:
    AudioRead();

    ~AudioRead();

    int open(const char *filename);

    int close();

    int read();

    int getBit() { return bit_; }

    int getSample() { return sample_; }

    int getChannel() { return channel_; }

protected:
    int bit_;       // 采样位数

    int sample_;    // 采样速率

    int channel_;   // 频道数

    FILE *file_;    // 文件
};

#endif
