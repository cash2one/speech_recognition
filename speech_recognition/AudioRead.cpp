
#include "stdafx.h"
#include "AudioRead.h"
#include <mmeapi.h>
#pragma comment(lib, "Winmm")

AudioRead::AudioRead()
    :bit_(16),
    channel_(1),
    sample_(16000),
    file_(NULL)
{
}

AudioRead::~AudioRead()
{
    close();
}

int AudioRead::open(const char *filename)
{
    if (NULL == filename)
    {
        return -1;
    }

    file_ = fopen(filename, "rb");

    if (NULL == file_)
    {
        return -2;
    }

    fseek(file_, 20, SEEK_SET);

    WAVEFORMATEX wfx = { 0 };

    fread(&wfx, 1, sizeof(wfx), file_);

    bit_ = wfx.wBitsPerSample;
    sample_ = wfx.nSamplesPerSec;
    channel_ = wfx.nChannels;

    return 0;
}

int AudioRead::close()
{
    if (NULL != file_)
    {
        fclose(file_);
    }

    return 0;
}

int AudioRead::read()
{
    fseek(file_, 0, SEEK_END);

    int size = ftell(file_);

    size -= 44;

    fseek(file_, 44, SEEK_SET);

    int len = 0;
    int read = 0;
    int remain = size;
    char data[1024];
    bool last = false;

    while (size > 0)
    {
        if (size >= sizeof(data))
        {
            read = sizeof(data);
        }
        else
        {
            read = size;
            last = true;
        }

        len = (int)fread(data, 1, read, file_);
        size -= len;

        next(last ? 999 : 0, data, len);
    }

    fclose(file_);

    file_ = NULL;

    return 0;
}
