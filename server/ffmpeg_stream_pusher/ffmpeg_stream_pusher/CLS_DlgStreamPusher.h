/*************************************************************
//module_name��	������
//module_func��	�������ļ�/ʵʱ����Ƶ�����������URL��ַ��
				ͨ��rtmp����������
//module_owner:	Young
//module_time:	2015-08-17 15:43
**************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <math.h>
#include <string>
#include <map>
#include <vector>
using namespace std;

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/mathematics.h"
#include "libswresample/swresample.h" 
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"  
#include "libavutil/time.h"  
#include "libavdevice/avdevice.h" 
#include "inttypes.h"
#include "SDL.h"
#include "SDL_thread.h"
};
#include "afxcmn.h"
#include "afxwin.h"
#undef main

//DirectShow
#include "qedit.h"
#include "dshow.h"  //����ICreateDevEnum

#define COLOR_BLACK	 RGB(0, 0, 0)
#define AUDIO_BUF_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

enum DeviceType{
	n_Video = 0,		//��Ƶ
	n_Audio = 1		//��Ƶ
};

typedef struct stream_info{
	AVFormatContext		*m_pFormatCtx;
	SDL_Thread			*m_test_audio_tid;		//��Ƶ�����߳�
	SDL_Thread			*m_test_video_tid;		//��Ƶ�����߳�
	SDL_Window			*m_show_screen;			//����Ƶ��ʾSDL����

	/************************��Ƶ��ز���-start*********************/
	int					 m_content_out_channels;	//��Ƶ������
	Uint8				*m_audio_chunk;			//��ƵPCM
	Uint32				 m_audio_len;			//��Ƶ������
	Uint8				*m_audio_pos;			//��Ƶ��λ��
	/************************��Ƶ��ز���-end***********************/
}struct_stream_info;

// CLS_DlgStreamPusher �Ի���
class CLS_DlgStreamPusher : public CDialog
{
// ����
public:
	CLS_DlgStreamPusher(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FFMPEG_STREAM_PUSHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedChkSrcType();
	afx_msg void OnBnClickedBtnOpenLocalFile();
	afx_msg void OnBnClickedBtnPreview();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnDeviceVideoTest();
	afx_msg void OnBnClickedBtnDeviceAudioTest();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//�ؼ�����
	CEdit	m_edtLocalFilePath;
	CEdit	m_edtPuserAddr;
	CButton m_chkSrcType;
	CButton m_btnOpenLocalFile;
	CStatic m_stcPreview;
	CComboBox m_cboDeviceVideo;
	CComboBox m_cboDeviceAudio;

private:
	/**********************
	method: ������Ϣ��ʼ��
	return: void
	**********************/
	void InitDlgItem();

	/**********************
	method: ������Ϣ��ʼ��
	return: void
	**********************/
	void InitData();

	/**********************
	method: ������Ϣ����ʼ��
	return: void
	**********************/
	void UnInitInfo();

	/**********************
	method: �������ɼ�����
	return: >=0:�ɹ�
			<0:ʧ��
	**********************/
	int	 UrlStreamColParse();

	/**********************
	method: �����ļ�����
	return: >=0:�ɹ�
			<0:ʧ��
	**********************/
	int	 LocalFileParse();

	/**********************
	method: ��ȡ���ӵ���Ƶ����Ƶ�豸
	param :	_iDeviceType:�豸����
	return: >=0:�ɹ�
			<0:ʧ��
	**********************/
	int  GetDeviceInfo(int _iDeviceType);


	CString m_cstrFilePath;	//�����ļ�·��
	BOOL		m_blUrl;			//�Ƿ��������������
	HBRUSH	m_bkBrush;		//����ˢ

	std::map<int, std::vector<std::string>>	m_mapDeviceInfo;	//�豸��Ϣ����

public:
	/**********************
	method: ��ȡ��ǰѡ�������Ƶ�豸����
	param :	_iDeviceType:�豸����
	return: �豸����
	**********************/
	char*  GetDeviceName(int _iDeviceType);

	static void  fill_audio(void *udata, Uint8 *stream, int len);

	struct_stream_info*						m_pStreamInfo;	//����Ƶȫ�ֽṹ��
};
