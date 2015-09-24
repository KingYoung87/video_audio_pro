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

//opengl
#include "freeglut.h"

#define COLOR_BLACK	 RGB(0, 0, 0)
#define AUDIO_BUF_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define FF_ALLOC_EVENT   (SDL_USEREVENT)
#define FF_AUDIO_REFRESH_EVENT (SDL_USEREVENT + 1)
#define FF_VIDEO_REFRESH_EVENT (SDL_USEREVENT + 2)
#define FF_BREAK_EVENT (SDL_USEREVENT + 3)
#define FF_QUIT_EVENT    (SDL_USEREVENT + 10)

enum DeviceType{
	n_Video = 0,		//��Ƶ
	n_Audio = 1		//��Ƶ
};
typedef struct AudioParams {
	int freq;
	int channels;
	int channel_layout;
	enum AVSampleFormat fmt;
} AudioParams;

enum ShowMode {
	SHOW_MODE_NONE = -1, 
	SHOW_MODE_VIDEO = 0, 
	SHOW_MODE_WAVES = 1, 
	SHOW_MODE_RDFT = 2, 
	SHOW_MODE_NB = 3
};
typedef struct stream_info{
	AVFormatContext		*m_pFormatCtx;
	SDL_Window			*m_show_screen;			//����Ƶ��ʾSDL����
	SDL_Surface			*m_screen_surface;		//��screen�󶨵ı���
	int					 m_xleft;				//��ʾ��������꼰��С
	int					 m_ytop;
	int					 m_width;
	int					 m_height;
	int					 m_abort_request;		//�˳����
	int					 m_refresh;				//ˢ�±��
	int					 m_show_mode;			//��ʾģʽ
	int					 m_paused;				//��ͣ���
	int					 m_itest_start;			//���Ա��
	SDL_Renderer			*m_sdlRenderer;
	SDL_Texture			*m_sdlTexture;
	/************************��Ƶ��ز���-start*********************/
	SDL_Thread			*m_test_audio_tid;		//��Ƶ�����߳�
	SDL_Thread			*m_audio_refresh_tid;	//��Ƶˢ���߳̾��
	AVStream				*m_pAudioStream;			//��Ƶ��
	AVFrame				*m_pAudioFrame;			//��Ƶ֡
	int					 m_content_out_channels;	//��Ƶ������
	SwrContext			*m_audio_swr_ctx;
	AudioParams			 m_audio_src;
	AudioParams			 m_audio_tgt;
	int					 m_audio_hw_buf_size;
	uint8_t				*m_audio_buf;
	int					 m_audio_buf_size;
	int					 m_audio_buf_index;
	int					 m_aduio_pkt_size;
	int					 m_audio_write_buf_size;
	int					 m_audio_last_i_start;
	double				 m_audio_clock;
	DECLARE_ALIGNED(16, uint8_t, m_audio_buf2)[MAX_AUDIO_FRAME_SIZE * 4];
	uint8_t				 m_silence_buf[AUDIO_BUF_SIZE];
	int16_t				 m_sample_array[SAMPLE_ARRAY_SIZE];
	int					 m_sample_array_index;
	/************************��Ƶ��ز���-end***********************/

	/************************��Ƶ��ز���-satrt*********************/
	SDL_Thread			*m_test_video_tid;		//��Ƶ�����߳�
	SDL_Thread			*m_video_refresh_tid;	//��Ƶˢ���߳̾��
	AVStream				*m_pVideoStream;			//��Ƶ��
	AVFrame				*m_pVideoFrame;			//��Ƶ֡
	AVFrame				*m_pVideoFrameYUV;		//��Ƶ֡��YUV
	AVPacket				*m_pVideoPacket;			//��Ƶ��
	uint8_t				*m_pVideoOutBuffer;		//��Ƶ�������
	SwsContext			*m_video_sws_ctx;
	
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

	CWinThread *m_pThreadEvent;					//�¼������߳�

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
	afx_msg void OnBnClickedChkShowVideo();
	afx_msg void OnBnClickedBtnDeviceVideoTest();
	afx_msg void OnBnClickedBtnDeviceAudioTest();
	afx_msg void OnBnClickedBtnDeviceAudioTestStop();
	afx_msg void OnBnClickedBtnDeviceVideoTestStop();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//�ؼ�����
	CEdit	m_edtLocalFilePath;
	CEdit	m_edtPusherAddr;
	CButton m_chkSrcType;
	CButton m_chkShowVideo;
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

	/**********************
	method: ��ȡ����Ƶ��Ϣ�ṹ��
	param :	
	return: ����Ƶ��Ϣ�ṹ��ָ��
	**********************/
	struct_stream_info* GetStreamStrcInfo();

	/**********************
	method: ������ʾ����
	param : 
	return: 
	**********************/
	void	 FillDisplayRect();

	/**********************
	method: ͬ����Ƶ
	param : _pstrct_streaminfo:��������Ϣ
			_inb_samples:��������
	return:
	**********************/
	int SynAudio(struct_stream_info* _pstrct_streaminfo, int _inb_samples);

	/**********************
	method: ����ˢ��
	param : opaque:��������Ϣ
	return:
	**********************/
	void screen_refresh(void *opaque);

	/**********************
	method: ������ʾ
	param : _pstrct_streaminfo:��������Ϣ
	return:
	**********************/
	void screen_display(struct_stream_info *_pstrct_streaminfo);

	/**********************
	method: ������ʾ��Ƶ����
	param : _pstrct_streaminfo:��������Ϣ
	return:
	**********************/
	void audio_display(struct_stream_info *_pstrct_streaminfo);

	/**********************
	method: ������ʾ��Ƶ
	param : _pstrct_streaminfo:��������Ϣ
	return:
	**********************/
	void video_display(struct_stream_info *_pstrct_streaminfo);

	/**********************
	method: �����������
	param : screen:����
			x:���Ͻǵ������
			y:���Ͻǵ�������
			w:�����
			h:�����
			color:��ɫֵ
	return:
	**********************/
	static void fill_rec(SDL_Surface *screen,
		int x, int y, int w, int h, int color);

	/**********************
	method: ֹͣ����
	return:
	**********************/
	void stop_test();

	/**********************
	method: ֹͣ������
	param:	opaque:����Ϣ
	return:
	**********************/
	void stream_stop(void *opaque);

	CString									m_cstrFilePath;	//�����ļ�·��
	HBRUSH									m_bkBrush;		//����ˢ
	BOOL										m_blUrl;			//�Ƿ�����������

	std::map<int, std::vector<std::string>>	m_mapDeviceInfo;	//�豸��Ϣ����

public:
	/**********************
	method: ��ȡ��ǰѡ�������Ƶ�豸����
	param :	_iDeviceType:�豸����
	return: �豸����
	**********************/
	char*  GetDeviceName(int _iDeviceType);

	/**********************
	method: �¼�������
	param : _pstrct_streaminfo:��������Ϣ
	return:
	**********************/
	void event_loop(struct_stream_info *_pstrct_streaminfo);

	int	push_stream();

	struct_stream_info*						m_pStreamInfo;	//����Ƶȫ�ֽṹ��
	BOOL										m_blVideoShow;	//�Ƿ���ʾ��Ƶ
	BOOL										m_blAudioShow;	//�Ƿ���ʾ��Ƶ
	BOOL										m_blPushStream;	//�Ƿ��������
	CString									m_cstrPushAddr;	//������ַ
};
