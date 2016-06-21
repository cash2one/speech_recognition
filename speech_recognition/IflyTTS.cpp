
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

    // rdn:           合成音频数字发音方式
    // volume:        合成音频的音量
    // pitch:         合成音频的音调
    // speed:         合成音频对应的语速
    // voice_name:    合成发音人
    // sample_rate:   合成音频采样率
    // text_encoding: 合成文本编码格式
    // 详细参数说明请参阅《iFlytek MSC Reference Manual》

    int          ret = MSP_SUCCESS;
    int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
    unsigned int audio_len = 0;
    const char*  login_params = "appid = 5743ed12, work_dir = .";// 登录参数,appid与msc库绑定,请勿随意改动
    const char*  session_begin_params = "voice_name = xiaoyan, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
    const char*  sessionID = NULL;

    // 用户登录
    ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数

    if (MSP_SUCCESS != ret)
    {
        return ret;
    }

    /* 开始合成 */
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
        // 获取合成音频
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

        Sleep(100); // 防止频繁占用CPU
    }

    if (MSP_SUCCESS != ret)
    {
        QTTSSessionEnd(sessionID, "AudioGetError");
        return ret;
    }

    QTTSSessionEnd(sessionID, "Normal");

    MSPLogout(); // 退出登录

    return 0;
}
