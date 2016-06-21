
#include "stdafx.h"
#include "IflyTTS.h"
#include "../ifly/include/qtts.h"
#include "../ifly/include/msp_cmn.h"
#include "../ifly/include/msp_errors.h"

#ifdef _WIN64
#pragma comment(lib,"../ifly/libs/msc_x64.lib")//x64
#else
#pragma comment(lib,"../ifly/libs/msc.lib")//x86
#endif

IflyTTS::IflyTTS()
{
}

IflyTTS::~IflyTTS()
{
}

int IflyTTS::tts(const char *text)
{
    USES_CONVERSION;

    if (NULL == text)
    {
        return -1;
    }

    // rdn:           �ϳ���Ƶ���ַ�����ʽ
    // volume:        �ϳ���Ƶ������
    // pitch:         �ϳ���Ƶ������
    // speed:         �ϳ���Ƶ��Ӧ������
    // voice_name:    �ϳɷ�����
    // sample_rate:   �ϳ���Ƶ������
    // text_encoding: �ϳ��ı������ʽ
    // ��ϸ����˵������ġ�iFlytek MSC Reference Manual��

    int          ret = MSP_SUCCESS;
    int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
    unsigned int audio_len = 0;
    const char*  login_params = "appid = 5743ed12, work_dir = .";// ��¼����,appid��msc���,��������Ķ�
    const char*  session_begin_params = "voice_name = xiaoyan, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
    const char*  sessionID = NULL;

    // �û���¼
    ret = MSPLogin(NULL, NULL, login_params); //��һ���������û������ڶ������������룬����NULL���ɣ������������ǵ�¼����

    if (MSP_SUCCESS != ret)
    {
        return ret;
    }

    /* ��ʼ�ϳ� */
    sessionID = QTTSSessionBegin(session_begin_params, &ret);

    if (MSP_SUCCESS != ret)
    {
        return ret;
    }

    ret = QTTSTextPut(sessionID, text, (unsigned int)strlen(text), NULL);

    if (MSP_SUCCESS != ret)
    {
        QTTSSessionEnd(sessionID, "QTTSTextPut error");
        return ret;
    }

    while (1)
    {
        // ��ȡ�ϳ���Ƶ
        const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);

        if (NULL != data && audio_len > 0)
        {
            TRACE("%d\n", audio_len);
            next(0, (void*)data, audio_len);
        }

        if (MSP_TTS_FLAG_DATA_END == synth_status || MSP_TTS_FLAG_CMD_CANCELED == synth_status)
        {
            break;
        }

        Sleep(100); // ��ֹƵ��ռ��CPU
    }

    if (MSP_SUCCESS != ret)
    {
        QTTSSessionEnd(sessionID, "AudioGetError");
        return ret;
    }

    QTTSSessionEnd(sessionID, "Normal");

    MSPLogout(); // �˳���¼

    return 0;
}
