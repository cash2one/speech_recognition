
#include "stdafx.h"
#include "AudioSave.h"

typedef struct _tagRIFF_HEADER
{
    char  szRiffID[4];     // 'R','I','F','F'
    DWORD dwRiffSize;      // file size - 8
    char  szRiffFormat[4]; // 'W','A','V','E'
}RIFF_HEADER;

typedef struct _tagWAVE_FORMAT
{
    WORD  wFormatTag;
    WORD  wChannels;
    DWORD dwSamplesPerSec;
    DWORD dwAvgBytesPerSec;
    WORD  wBlockAlign;
    WORD  wBitsPerSample;
}WAVE_FORMAT;

typedef struct _tagFMT_BLOCK
{
    char  szFmtID[4];       // 'f','m','t',' '
    DWORD dwFmtSize;        // 数值为16或18，18则最后又附加信息
    WAVE_FORMAT wavFormat;
}FMT_BLOCK;

typedef struct _tagFACT_BLOCK   // 可选
{
    char  szFactID[4];      // 'f','a','c','t'
    DWORD dwFactSize;
    DWORD dwDataSize;
}FACT_BLOCK;

typedef struct _tagDATA_BLOCK
{
    char szDataID[4];       // 'd','a','t','a'
    DWORD dwDataSize;
}DATA_BLOCK;

AudioSave::AudioSave()
    :bit_(16),
    channel_(1),
    sample_(16000),
    file_(NULL)
{
}

AudioSave::~AudioSave()
{
}

int AudioSave::open(const char *filename, bool create)
{
    if (NULL == filename)
    {
        return 1;
    }

    file_ = fopen(filename, (create ? "wb+" : "wb"));

    if (NULL == file_)
    {
        return 2;
    }

    if (create)
    {
        RIFF_HEADER riff_header = { 0 };
        FMT_BLOCK   fmt_block = { 0 };
        DATA_BLOCK  data_block = { 0 };

        memcpy(riff_header.szRiffID, "RIFF", 4);
        memcpy(riff_header.szRiffFormat, "WAVE", 4);
        riff_header.dwRiffSize = sizeof(RIFF_HEADER) + sizeof(FMT_BLOCK) + sizeof(DATA_BLOCK) - 8;

        memcpy(fmt_block.szFmtID, "fmt ", 4);
        fmt_block.dwFmtSize = sizeof(WAVE_FORMAT);
        fmt_block.wavFormat.wFormatTag = 1; // WAVE_FORMAT_PCM;
        fmt_block.wavFormat.wChannels = channel_;
        fmt_block.wavFormat.wBitsPerSample = bit_;
        fmt_block.wavFormat.dwSamplesPerSec = sample_;
        fmt_block.wavFormat.wBlockAlign = bit_ * channel_ / 8;
        fmt_block.wavFormat.dwAvgBytesPerSec = channel_ * sample_ * bit_ / 8;

        memcpy(data_block.szDataID, "data", 4);
        data_block.dwDataSize = 0;

        fwrite(&riff_header, 1, sizeof(RIFF_HEADER), file_);
        fwrite(&fmt_block, 1, sizeof(FMT_BLOCK), file_);
        fwrite(&data_block, 1, sizeof(DATA_BLOCK), file_);
    }

    return 0;
}

int AudioSave::close()
{
    if (NULL != file_)
    {
        fclose(file_);
    }

    return 0;
}

int AudioSave::save(const void *data, int len)
{
    if (NULL == data || len <= 0)
    {
        return 1;
    }

    if (NULL == file_)
    {
        return 2;
    }

    fseek(file_, 0, SEEK_END);
    long size = ftell(file_);
    fwrite(data, 1, len, file_);

    fseek(file_, 4, SEEK_SET); // 从下个地址开始到文件尾的总字节数
    unsigned long riffSize = size - 8 + len;
    fwrite((void*)&riffSize, 1, 4, file_);

    fseek(file_, 40, SEEK_SET); // 总数据大小
    unsigned long dataSize = size - 44 + len;
    fwrite((void*)&dataSize, 1, 4, file_);

    return 0;
}

int AudioSave::next(unsigned int cmd, void *data, int len)
{
    if (0 == cmd)
    {
        save(data, len);
    }

    return Chain::next(cmd, data, len);
}
