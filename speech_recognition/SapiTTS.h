
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
    int bit_;       // ����λ��

    int sample_;    // ��������

    int channel_;   // Ƶ����

    FILE *file_;    // �ļ����
};

#endif
