
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

    // COM��ʼ��
    ::CoInitialize(NULL);

    CLSID CLSID_SpVoice;
    CLSIDFromProgID(_T("SAPI.SpVoice"), &CLSID_SpVoice);

    ISpVoice *pSpVoice = NULL;
    IEnumSpObjectTokens *pSpEnumTokens = NULL;

    // ��ȡISpVoice�ӿ�
    if (FAILED(CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)&pSpVoice)))
    {
        ::CoUninitialize();
        return;
    }

    // �о����е�����token������ͨ��pSpEnumTokensָ��Ľӿڵõ�
    if (SUCCEEDED(SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pSpEnumTokens)))
    {
        ISpObjectToken *pSpToken = NULL;

        // ���λ�ȡÿ��token���ʶ��ַ���
        while (SUCCEEDED(pSpEnumTokens->Next(1, &pSpToken, NULL)) && pSpToken != NULL)
        {
            pSpVoice->SetVoice(pSpToken);                   // ���õ�ǰ����tokenΪpSpToken
            pSpVoice->Speak(A2W(text), SPF_DEFAULT, NULL);  // �ʶ����ĺ�Ӣ�ĵĻ���ַ���
            pSpToken->Release();                            // �ͷ�token
        }

        pSpEnumTokens->Release();                           // �ͷ�pSpEnumTokens�ӿ�
    }

    pSpVoice->Release();
    ::CoUninitialize();

    return Chain::next(data, len);
}
