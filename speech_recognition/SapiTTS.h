
#ifndef _SAPI_TTS_H_
#define _SAPI_TTS_H_
#include "Chain.h"


class SapiTTS : public Chain
{
public:
    SapiTTS();

    ~SapiTTS();

    int tts(const char *text);



    int next(void *data, int len);

protected:
    int bit_;       // 采样位数

    int sample_;    // 采样速率

    int channel_;   // 频道数

    FILE *file_;    // 文件句柄
};

#endif
