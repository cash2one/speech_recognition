
#include "stdafx.h"
#include "SapiTTS.h"
#include "sapi.h"
#include "sphelper.h"
#pragma comment(lib, "sapi.lib")


SapiTTS::SapiTTS()
{
}

SapiTTS::~SapiTTS()
{
}

int SapiTTS::tts(const char *text)
{
    USES_CONVERSION;

    if (NULL == text)
    {
        return 1;
    }

    // COM初始化
    ::CoInitialize(NULL);

    CLSID CLSID_SpVoice;
    CLSIDFromProgID(_T("SAPI.SpVoice"), &CLSID_SpVoice);

    ISpVoice *pSpVoice = NULL;
    IEnumSpObjectTokens *pSpEnumTokens = NULL;

    // 获取ISpVoice接口
    if (FAILED(CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)&pSpVoice)))
    {
        ::CoUninitialize();
        return;
    }

    // 列举所有的语音token，可以通过pSpEnumTokens指向的接口得到
    if (SUCCEEDED(SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pSpEnumTokens)))
    {
        ISpObjectToken *pSpToken = NULL;

        // 依次获取每个token并朗读字符串
        while (SUCCEEDED(pSpEnumTokens->Next(1, &pSpToken, NULL)) && pSpToken != NULL)
        {
            pSpVoice->SetVoice(pSpToken);                   // 设置当前语音token为pSpToken
            pSpVoice->Speak(A2W(text), SPF_DEFAULT, NULL);  // 朗读中文和英文的混合字符串
            pSpToken->Release();                            // 释放token
        }

        pSpEnumTokens->Release();                           // 释放pSpEnumTokens接口
    }

    pSpVoice->Release();
    ::CoUninitialize();

    return Chain::next(data, len);
}
