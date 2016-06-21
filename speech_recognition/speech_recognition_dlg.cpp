
// speech_recognition_dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "speech_recognition.h"
#include "speech_recognition_dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



speech_recognition_dlg::speech_recognition_dlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_SPEECH_RECOGNITION_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void speech_recognition_dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(speech_recognition_dlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_SAPI_PLAY, &speech_recognition_dlg::OnBnClickedBtnSapiPlay)
    ON_BN_CLICKED(IDC_BTN_SAPI_RECOGNIZE, &speech_recognition_dlg::OnBnClickedBtnSapiRecognize)
    ON_BN_CLICKED(IDC_BTN_IFLY_PLAY, &speech_recognition_dlg::OnBnClickedBtnIflyPlay)
    ON_BN_CLICKED(IDC_BTN_IFLY_RECOGNIZE, &speech_recognition_dlg::OnBnClickedBtnIflyRecognize)
END_MESSAGE_MAP()

BOOL speech_recognition_dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);     // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);    // ����Сͼ��

    GetDlgItem(IDC_EDIT_INPUT_TEXT)->SetWindowText(L"���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��,���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�ִ�д˲���");

    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void speech_recognition_dlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ����������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR speech_recognition_dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

#include "sapi.h"
#include "sphelper.h"
#pragma comment(lib, "sapi.lib")

void speech_recognition_dlg::OnBnClickedBtnSapiPlay()
{
    CString text;
    GetDlgItem(IDC_EDIT_INPUT_TEXT)->GetWindowText(text);

    // COM��ʼ��
    ::CoInitialize(NULL);

    CLSID CLSID_SpVoice;
    CLSIDFromProgID(_T("SAPI.SpVoice"), &CLSID_SpVoice);

    ISpVoice *pSpVoice = NULL;
    IEnumSpObjectTokens *pSpEnumTokens = NULL;

    // ��ȡISpVoice�ӿ�
    if (FAILED(CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)&pSpVoice)))
    {
        return;
    }

    // �о����е�����token������ͨ��pSpEnumTokensָ��Ľӿڵõ�
    if (SUCCEEDED(SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pSpEnumTokens)))
    {
        ISpObjectToken *pSpToken = NULL;

        // ���λ�ȡÿ��token���ʶ��ַ���
        while (SUCCEEDED(pSpEnumTokens->Next(1, &pSpToken, NULL)) && pSpToken != NULL)
        {
            pSpVoice->SetVoice(pSpToken);               // ���õ�ǰ����tokenΪpSpToken
            pSpVoice->Speak(text, SPF_DEFAULT, NULL);   // �ʶ����ĺ�Ӣ�ĵĻ���ַ���
            pSpToken->Release();                        // �ͷ�token
        }

        pSpEnumTokens->Release();                       // �ͷ�pSpEnumTokens�ӿ�
    }

    pSpVoice->Release();
    ::CoUninitialize();
}

const WCHAR * StopWord()
{
    const WCHAR * pchStop;

    LANGID LangId = ::SpGetUserDefaultUILanguage();

    switch (LangId)
    {
    case MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT):
        pchStop = L"}42N86\0b70e50fc0ea0e70fc/05708504608a087046";;
        break;

    default:
        pchStop = L"Stop";
        break;
    }

    return pchStop;
}

inline HRESULT BlockForResult(ISpRecoContext * pRecoCtxt, ISpRecoResult **ppResult)
{
    HRESULT hr = S_OK;
    CSpEvent event;

    while (SUCCEEDED(hr) &&
        SUCCEEDED(hr = event.GetFrom(pRecoCtxt)) &&
        hr == S_FALSE)
    {
        hr = pRecoCtxt->WaitForNotifyEvent(INFINITE);
    }

    *ppResult = event.RecoResult();
    if (*ppResult)
    {
        (*ppResult)->AddRef();
    }

    return hr;
}

bool g_recognize_thread_run = false;

void recognize_thread_func(void *param)
{
    speech_recognition_dlg *pDlg = (speech_recognition_dlg*)param;

    USES_CONVERSION;
    g_recognize_thread_run = true;

    ::CoInitialize(NULL);

    CComPtr<ISpRecognizer> cpRecognizer;    // ����ʶ������ 
    CComPtr<ISpRecoContext> cpRecoCtxt;     // ����ʶ�������Ľӿ�
    CComPtr<ISpRecoGrammar> cpGrammar;      // �﷨����ӿ�
    CComPtr<ISpAudio> cpAudio;              // ��������
    CComPtr<ISpVoice> cpVoice;              // �����ϳɽӿ�

    HRESULT hr = cpRecoCtxt.CoCreateInstance(CLSID_SpSharedRecoContext);
    //HRESULT hr = cpRecognizer.CoCreateInstance(CLSID_SpInprocRecognizer);           // ����ʽ CLSID_SpInprocRecognizer-����ʽ

    hr = cpRecoCtxt->GetRecognizer(&cpRecognizer);

    hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);              // ����Ĭ�ϵ���Ƶ�������
    hr = cpRecognizer->SetInput(cpAudio, TRUE);                                     // ����ʶ����������Դ
    hr = cpRecognizer->SetRecoState(SPRST_ACTIVE);                                  // ����ʶ������״̬
    //hr = cpRecognizer->CreateRecoContext(&cpRecoCtxt);                              // ����������

    hr = cpRecoCtxt->GetVoice(&cpVoice);
    hr = cpRecoCtxt->SetNotifyWin32Event();
    hr = cpRecoCtxt->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION)); // �������Ǹ���Ȥ���¼�
    hr = cpRecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL);
    hr = cpRecoCtxt->CreateGrammar(0, &cpGrammar);                                  // �����﷨����

    hr = cpGrammar->LoadDictation(NULL, SPLO_STATIC);
    hr = cpGrammar->SetDictationState(SPRS_ACTIVE);
    //hr = cpGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);              // �����﷨����ʶ�� 

    const WCHAR * const pchStop = StopWord();
    CComPtr<ISpRecoResult> cpResult;

    CString text;
    text.Format(L"I will repeat everything you say. \nSay %s to exit.", pchStop);
    pDlg->GetDlgItem(IDC_EDIT_INPUT_TEXT)->SetWindowText(text);

    while (g_recognize_thread_run && SUCCEEDED(BlockForResult(cpRecoCtxt, &cpResult)))
    {
        cpGrammar->SetDictationState(SPRS_INACTIVE);
        //cpGrammar->SetRuleState(NULL, NULL, SPRS_INACTIVE);

        CSpDynamicString dstrText;

        if (SUCCEEDED(cpResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &dstrText, NULL)))
        {
            TRACE(W2A(dstrText));

            pDlg->GetDlgItem(IDC_EDIT_INPUT_TEXT)->SetWindowText(dstrText);

            cpResult.Release();
        }

        if (_wcsicmp(dstrText, pchStop) == 0)
        {
            break;
        }

        cpGrammar->SetDictationState(SPRS_ACTIVE);
        //cpGrammar->SetRuleState(NULL, NULL, SPRS_INACTIVE);
    }

    ::CoUninitialize();

    g_recognize_thread_run = false;
}

void speech_recognition_dlg::OnBnClickedBtnSapiRecognize()
{
    if (!g_recognize_thread_run)
    {
        _beginthread(recognize_thread_func, 0, this);
    }
    else
    {
        g_recognize_thread_run = false;
    }
}

void speech_recognition_dlg::OnBnClickedBtnIflyPlay()
{
    USES_CONVERSION;

    CString text;
    GetDlgItem(IDC_EDIT_INPUT_TEXT)->GetWindowText(text);

    audioPlay_.setBit(16);
    audioPlay_.setChannel(1);
    audioPlay_.setSample(16000);
    audioPlay_.setNext(&audioSave_);
    audioPlay_.open();

    audioSave_.setBit(16);
    audioSave_.setChannel(1);
    audioSave_.setSample(16000);
    audioSave_.setNext(NULL);
    audioSave_.open("ifly_tts.wav", true);

    iflyTts_.setNext(&audioPlay_);
    iflyTts_.tts(W2A(text));

    audioSave_.close();
}

void speech_recognition_dlg::OnBnClickedBtnIflyRecognize()
{
    audioRead_.setNext(&audioPlay_);

    audioPlay_.setNext(&iflyStt_);

    //audioCapture_.setBit(16);
    //audioCapture_.setChannel(1);
    //audioCapture_.setSample(16000);
    //audioCapture_.setNext(&iflyStt_);

    iflyStt_.setNext(NULL);

    static bool run = false;

    if (!run)
    {
        iflyStt_.open();

        //audioCapture_.open();
        //audioCapture_.start();

        audioRead_.open("ifly_tts.wav");

        audioPlay_.setBit(audioRead_.getBit());
        audioPlay_.setSample(audioRead_.getSample());
        audioPlay_.setChannel(audioRead_.getChannel());
        audioPlay_.open();

        audioRead_.read();
    }
    else
    {
        //audioCapture_.stop();
        //audioCapture_.close();
        iflyStt_.close();
        audioPlay_.close();
    }

    run = !run;
}
