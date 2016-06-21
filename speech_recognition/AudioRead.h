
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
    int bit_;       // ����λ��

    int sample_;    // ��������

    int channel_;   // Ƶ����

    FILE *file_;    // �ļ�
};

#endif
