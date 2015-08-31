/*************************************************************
//module_name：	推流器
//module_func：	将本地文件/实时音视频流解码输出到URL地址，
				通过rtmp进行流推送
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
#include "dshow.h"  //包含ICreateDevEnum

#define COLOR_BLACK	 RGB(0, 0, 0)
#define AUDIO_BUF_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

enum DeviceType{
	n_Video = 0,		//视频
	n_Audio = 1		//音频
};

typedef struct stream_info{
	AVFormatContext		*m_pFormatCtx;
	SDL_Thread			*m_test_audio_tid;		//音频测试线程
	SDL_Thread			*m_test_video_tid;		//视频测试线程
	SDL_Window			*m_show_screen;			//音视频显示SDL窗口

	/************************音频相关参数-start*********************/
	int					 m_content_out_channels;	//音频音道数
	Uint8				*m_audio_chunk;			//音频PCM
	Uint32				 m_audio_len;			//音频包长度
	Uint8				*m_audio_pos;			//音频包位移
	/************************音频相关参数-end***********************/
}struct_stream_info;

// CLS_DlgStreamPusher 对话框
class CLS_DlgStreamPusher : public CDialog
{
// 构造
public:
	CLS_DlgStreamPusher(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FFMPEG_STREAM_PUSHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

	//控件定义
	CEdit	m_edtLocalFilePath;
	CEdit	m_edtPuserAddr;
	CButton m_chkSrcType;
	CButton m_btnOpenLocalFile;
	CStatic m_stcPreview;
	CComboBox m_cboDeviceVideo;
	CComboBox m_cboDeviceAudio;

private:
	/**********************
	method: 界面信息初始化
	return: void
	**********************/
	void InitDlgItem();

	/**********************
	method: 数据信息初始化
	return: void
	**********************/
	void InitData();

	/**********************
	method: 界面信息反初始化
	return: void
	**********************/
	void UnInitInfo();

	/**********************
	method: 网络流采集解析
	return: >=0:成功
			<0:失败
	**********************/
	int	 UrlStreamColParse();

	/**********************
	method: 本地文件解析
	return: >=0:成功
			<0:失败
	**********************/
	int	 LocalFileParse();

	/**********************
	method: 获取连接的视频与音频设备
	param :	_iDeviceType:设备类型
	return: >=0:成功
			<0:失败
	**********************/
	int  GetDeviceInfo(int _iDeviceType);


	CString m_cstrFilePath;	//推送文件路径
	BOOL		m_blUrl;			//是否进行网络流推送
	HBRUSH	m_bkBrush;		//背景刷

	std::map<int, std::vector<std::string>>	m_mapDeviceInfo;	//设备信息容器

public:
	/**********************
	method: 获取当前选择的音视频设备名臣
	param :	_iDeviceType:设备类型
	return: 设备名称
	**********************/
	char*  GetDeviceName(int _iDeviceType);

	static void  fill_audio(void *udata, Uint8 *stream, int len);

	struct_stream_info*						m_pStreamInfo;	//音视频全局结构体
};
