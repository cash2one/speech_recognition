/*------------------------------------------------------------------------------*\
    ��װ waveout api
    ֻʹ���� pcm ��ʽ
\*------------------------------------------------------------------------------*/

#ifndef _AUDIO_CAPTURE_H_
#define _AUDIO_CAPTURE_H_
#include "Chain.h"
#include <mmeapi.h>


class AudioCapture : public Chain
{
public:
    AudioCapture();

    ~AudioCapture();

    int open();

    int close();

    int start();

    int stop();

    void setBit(int bit) { bit_ = bit; }

    void setSample(int sample) { sample_ = sample; }

    void setChannel(int channel) { channel_ = channel; }

    static void CALLBACK procCallback(HWAVEOUT dev, UINT msg, DWORD instance, DWORD param1, DWORD param2);

protected:
    int bit_;       // ����λ��

    int sample_;    // ��������

    int channel_;   // Ƶ����

    bool run_;      // �Ƿ�����

    bool record_;   // �Ƿ�¼��

    HWAVEIN dev_;   // ���þ��

    LPWAVEHDR hdr_; // ������
};

#endif