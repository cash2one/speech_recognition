/*------------------------------------------------------------------------------*\
    封装 waveout api
    只使用了 pcm 方式
\*------------------------------------------------------------------------------*/

#ifndef _AUDIO_PLAY_H_
#define _AUDIO_PLAY_H_
#include "Chain.h"
#include <mmeapi.h>


class AudioPlay : public Chain
{
public:
    AudioPlay();

    ~AudioPlay();

    int open();

    int close();

    int play(const void *data, int len);

    int next(unsigned int cmd, void *data, int len);

    void setBit(int bit) { bit_ = bit; }

    void setSample(int sample) { sample_ = sample; }

    void setChannel(int channel) { channel_ = channel; }

    static void CALLBACK procCallback(HWAVEOUT dev, UINT msg, DWORD instance, DWORD param1, DWORD param2);

protected:
    int bit_;       // 采样位数

    int sample_;    // 采样速率

    int channel_;   // 频道数

    bool run_;      // 是否运行

    HWAVEOUT dev_;  // 设置句柄
};

#endif
