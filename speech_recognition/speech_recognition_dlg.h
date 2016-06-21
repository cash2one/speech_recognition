
// speech_recognition_dlg.h : ͷ�ļ�
//

#pragma once

#include "AudioRead.h"
#include "AudioPlay.h"
#include "AudioSave.h"
#include "AudioPlay.h"
#include "AudioCapture.h"
#include "IflyTTS.h"
#include "IflySTT.h"

class speech_recognition_dlg : public CDialogEx
{

public:
    speech_recognition_dlg(CWnd* pParent = NULL);    // ��׼���캯��

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SPEECH_RECOGNITION_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    AudioRead audioRead_;
    AudioSave audioSave_;
    AudioPlay audioPlay_;
    AudioCapture audioCapture_;

    IflyTTS iflyTts_;
    IflySTT iflyStt_;

    // ʵ��
protected:
    HICON m_hIcon;

    // ���ɵ���Ϣӳ�亯��
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnSapiPlay();
    afx_msg void OnBnClickedBtnSapiRecognize();
    afx_msg void OnBnClickedBtnIflyPlay();
    afx_msg void OnBnClickedBtnIflyRecognize();
};
