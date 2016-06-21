
#include "stdafx.h"
#include "AudioCapture.h"
#pragma comment(lib, "Winmm")

#define AUDIO_BUFF_NUM  16      // ����������
#define AUDIO_BUFF_SIZE 16*1024 // ��������С


AudioCapture::AudioCapture()
    :bit_(16),
    channel_(1),
    sample_(16000),
    run_(false),
    record_(false),
    dev_(NULL),
    hdr_(NULL)
{
}

AudioCapture::~AudioCapture()
{
    close();
}

int AudioCapture::open()
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

    // ���¼���豸�Ƿ�֧��������ʽ
    if (waveInOpen(NULL, WAVE_MAPPER, &wfx, NULL, NULL, WAVE_FORMAT_QUERY))
    {
        return 1;
    }

    // ��¼���豸
    if (waveInOpen(&dev_, WAVE_MAPPER, &wfx, (DWORD_PTR)procCallback, (DWORD_PTR)this, CALLBACK_FUNCTION))
    {
        return 2;
    }

    // ����¼���豸
    if (waveInReset(dev_))
    {
        return 3;
    }

    hdr_ = new WAVEHDR[AUDIO_BUFF_NUM];

    for (int i = 0; i < AUDIO_BUFF_NUM; i++)
    {
        memset(&hdr_[i], 0, sizeof(WAVEHDR));
        hdr_[i].lpData = new char[AUDIO_BUFF_SIZE];
        hdr_[i].dwBufferLength = AUDIO_BUFF_SIZE;
        hdr_[i].dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
        hdr_[i].dwLoops = 1;

        // ׼������
        waveInPrepareHeader(dev_, &hdr_[i], sizeof(WAVEHDR));

        // ��ӻ��浽¼���豸
        waveInAddBuffer(dev_, &hdr_[i], sizeof(WAVEHDR));
    }

    run_ = true;

    return 0;
}

int AudioCapture::close()
{
    if (!run_)
    {
        return 0;
    }

    stop();

    for (int i = 0; i < AUDIO_BUFF_NUM; i++)
    {
        waveInUnprepareHeader(dev_, &hdr_[i], sizeof(WAVEHDR));
        delete[] hdr_[i].lpData;
    }

    waveInReset(dev_);
    waveInClose(dev_);

    delete[] hdr_;
    hdr_ = NULL;
    run_ = false;

    return 0;
}

int AudioCapture::start()
{
    if (!run_)
    {
        return 1;
    }

    if (record_)
    {
        return 0;
    }

    // ��ʼ¼��
    if (waveInStart(dev_))
    {
        return 1;
    }

    record_ = true;

    return 0;
}

int AudioCapture::stop()
{
    if (!run_)
    {
        return 0;
    }

    if (!record_)
    {
        return 0;
    }

    // ֹͣ¼��
    if (waveInStop(dev_))
    {
        return 1;
    }

    record_ = false;

    return 0;
}

void CALLBACK AudioCapture::procCallback(HWAVEOUT dev, UINT msg, DWORD instance, DWORD param1, DWORD param2)
{
    if (WIM_DATA != msg)
    {
        return;
    }

    WAVEHDR *hdr = (WAVEHDR*)param1;

    if (AUDIO_BUFF_SIZE == hdr->dwBytesRecorded)
    {
        AudioCapture *pthis = (AudioCapture*)instance;
        pthis->next(0, hdr->lpData, hdr->dwBytesRecorded);
        waveInAddBuffer(pthis->dev_, hdr, sizeof(WAVEHDR));
    }
}
