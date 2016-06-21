
#include "stdafx.h"
#include "IflySTT.h"
#include "../ifly/include/qisr.h"
#include "../ifly/include/msp_cmn.h"
#include "../ifly/include/msp_errors.h"

#ifdef _WIN64
#pragma comment(lib,"../ifly/libs/msc_x64.lib")//x64
#else
#pragma comment(lib,"../ifly/libs/msc.lib")//x86
#endif

#define BUFFER_SIZE 4096
#define FRAME_LEN   640

void iflystt_get_text_thread(void *param)
{
    IflySTT *pthis = (IflySTT*)param;

    while (pthis->getRun())
    {
        pthis->getText();

        for (int i = 0; i < 500 && pthis->getRun(); i++)
        {
            Sleep(1);
        }
    }

    TRACE("%s error\n", __FUNCTION__);
}

IflySTT::IflySTT()
    :audio_stat_(MSP_AUDIO_SAMPLE_FIRST),
    session_id_(NULL),
    run_(false)
{
}

IflySTT::~IflySTT()
{
}

int IflySTT::open()
{
    // sub:             请求业务类型
    // domain:          领域
    // language:        语言
    // accent:          方言
    // sample_rate:     音频采样率
    // result_type:     识别结果格式
    // result_encoding: 结果编码格式
    // 详细参数说明请参阅《iFlytek MSC Reference Manual》

    int         ret = MSP_SUCCESS;
    int         errcode = MSP_SUCCESS;
    const char* login_params = "appid = 5743ed12, work_dir = ."; // 登录参数，appid与msc库绑定,请勿随意改动
    const char* session_begin_params = "sub = iat, domain = iat, language = zh_ch, accent = mandarin, sample_rate = 1411000, result_type = plain, result_encoding = gb2312";

    // 用户登录
    ret = MSPLogin(NULL, NULL, login_params); // 第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数

    if (MSP_SUCCESS != ret)
    {
        return ret;
    }

    session_id_ = QISRSessionBegin(NULL, session_begin_params, &errcode); // 听写不需要语法，第一个参数为NULL

    if (MSP_SUCCESS != errcode)
    {
        MSPLogout();
        return errcode;
    }

    run_ = true;

    _beginthread(iflystt_get_text_thread, 0, this);

    return 0;
}

int IflySTT::close()
{
    run_ = false;

    QISRSessionEnd(session_id_, "Normal");

    MSPLogout(); // 退出登录

    return 0;
}

int IflySTT::stt(void *data, int len)
{
    if (NULL == data || len <= 0)
    {
        return -1;
    }

    int ret = MSP_SUCCESS;
    int errcode = MSP_SUCCESS;
    int ep_stat = MSP_EP_LOOKING_FOR_SPEECH;    // 端点检测
    int rec_stat = MSP_REC_STATUS_SUCCESS;      // 识别状态

    ret = QISRAudioWrite(session_id_, data, len, audio_stat_, &ep_stat, &rec_stat);

    if (MSP_SUCCESS != ret)
    {
        QISRSessionEnd(session_id_, "QISRAudioWrite error");
        MSPLogout();
        return ret;
    }

    if (MSP_AUDIO_SAMPLE_FIRST == audio_stat_)
    {
        audio_stat_ = MSP_AUDIO_SAMPLE_CONTINUE;
    }

    return 0;
}

int IflySTT::getText()
{
    int errcode = MSP_SUCCESS;
    int rec_stat = MSP_REC_STATUS_SUCCESS;      // 识别状态
    const char *rslt = QISRGetResult(session_id_, &rec_stat, 0, &errcode);

    if (MSP_SUCCESS == errcode && NULL != rslt)
    {
        TRACE("%s %s\n", __FUNCTION__, rslt);
        Chain::next(0, (void*)rslt, (int)strlen(rslt));
    }

    return 0;
}

int IflySTT::next(unsigned int cmd, void *data, int len)
{
    if (0 == cmd)
    {
        return stt(data, len);
    }

    if (999 == cmd)
    {
        audio_stat_ = MSP_AUDIO_SAMPLE_LAST;
        return stt(data, len);
    }

    return Chain::next(cmd, data, len);
}
