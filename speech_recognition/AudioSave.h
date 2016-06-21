
#ifndef _AUDIO_SAVE_H_
#define _AUDIO_SAVE_H_
#include "Chain.h"


class AudioSave : public Chain
{
public:
    AudioSave();

    ~AudioSave();

    int open(const char *filename, bool create);

    int close();

    int save(const void *data, int len);

    int next(unsigned int cmd, void *data, int len);

    void setBit(int bit) { bit_ = bit; }

    void setSample(int sample) { sample_ = sample; }

    void setChannel(int channel) { channel_ = channel; }

protected:
    int bit_;       // 采样位数

    int sample_;    // 采样速率

    int channel_;   // 频道数

    FILE *file_;    // 文件句柄
};

#endif
