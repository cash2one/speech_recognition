
#include "stdafx.h"
#include "AudioPlay.h"
#pragma comment(lib, "Winmm")

AudioPlay::AudioPlay()
    :bit_(16),
    channel_(1),
    sample_(16000),
    run_(false),
    dev_(NULL)
{
}

AudioPlay::~AudioPlay()
{
    close();
}

int AudioPlay::open()
{
    if (run_)
    {
        return 0;
    }

    // ���ò��������ʽ
    WAVEFORMATEX wfx = { 0 };
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = channel_;
    wfx.wBitsPerSample = bit_;
    wfx.nSamplesPerSec = sample_;
    wfx.nBlockAlign = bit_ * channel_ / 8;
    wfx.nAvgBytesPerSec = channel_ * sample_ * bit_ / 8;
    wfx.cbSize = 0;

    // ��鲥���豸�Ƿ�֧��������ʽ
    if (waveOutOpen(NULL, WAVE_MAPPER, &wfx, NULL, NULL, WAVE_FORMAT_QUERY))
    {
        return 1;
    }

    // �򿪲����豸
    if (waveOutOpen(&dev_, WAVE_MAPPER, &wfx, (DWORD_PTR)procCallback, (DWORD_PTR)this, CALLBACK_FUNCTION))
    {
        return 2;
    }

    run_ = true;

    return 0;
}

int AudioPlay::close()
{
    if (!run_)
    {
        return 0;
    }

    waveOutReset(dev_);
    waveOutClose(dev_);

    run_ = false;

    return 0;
}

int AudioPlay::play(const void *data, int len)
{
    if (NULL == data || len <= 0)
    {
        return -1;
    }

    if (!run_)
    {
        return -2;
    }

    if (NULL != data && 0 == strcmp("close", (const char *)data) && 999 == len)
    {
        return 0;
    }

    // �½�WAVEHDR�ṹ�����ڽ��ղ�����ʽ
    LPWAVEHDR pwh = new WAVEHDR;

    if (NULL == pwh)
    {
        return -3;
    }

    memset(pwh, 0, sizeof(WAVEHDR));
    pwh->lpData = new char[len];
    pwh->dwBufferLength = len;
    memcpy(pwh->lpData, data, len);

    // Ϊ�ط���������׼��
    if (waveOutPrepareHeader(dev_, pwh, sizeof(WAVEHDR)))
    {
        delete pwh->lpData;
        delete pwh;
        return -4;
    }

    // �����ݷ��������豸
    if (waveOutWrite(dev_, pwh, sizeof(WAVEHDR)))
    {
        delete pwh->lpData;
        delete pwh;
        return -5;
    }

    return 0;
}

void CALLBACK AudioPlay::procCallback(HWAVEOUT dev, UINT msg, DWORD instance, DWORD param1, DWORD param2)
{
    if (WOM_DONE != msg) // �������
    {
        return;
    }

    WAVEHDR *pwh = (WAVEHDR*)param1;
    AudioPlay *pthis = (AudioPlay*)instance;
    waveOutUnprepareHeader(pthis->dev_, pwh, sizeof(WAVEHDR));  // �ͷŲ�������
    delete[] pwh->lpData;
    delete pwh;
}

int AudioPlay::next(unsigned int cmd, void *data, int len)
{
    if (0 == cmd)
    {
        play(data, len);
    }

    return Chain::next(cmd, data, len);
}
