
// FFMPEG_SDL_MFC_PLAYER_1Dlg.h : header file
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavdevice/avdevice.h"
#include "libavutil/mathematics.h"
#include "libswresample/swresample.h" 
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"  
#include "libavutil/time.h"  
#include "inttypes.h"
#include "SDL.h"
#include "SDL_thread.h"
};
#include "afxcmn.h"
#include "afxwin.h"
#undef main

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")

#pragma comment(lib,"SDL2.lib")
//这里不用这个东东 木有用 还会有错误
//#pragma comment(lib,"SDLmain.lib")


#define SDL_AUDIO_BUFFER_SIZE 1024
#define VIDEO_PICTURE_QUEUE_SIZE 1
#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES      5
#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0
#define VIDEO_ID 100
#define AUDIO_ID 101
#define MFCMINWINDOWSIZE_WIDTH  600
#define MFCMINWINDOWSIZE_HEIGHT 300
#define BMPPATHNAME _T("..//BMP_RESOURCES/22.bmp")
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

#define VIDEO_AUDIO_NOSYNC 4.0


/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20
#define SAMPLE_CORRECTION_PERCENT_MAX 10

enum {
	AV_SYNC_AUDIO_MASTER = 0,
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_CLOCK
};

//?? 代表代码中可优化的地方
//1:?? seek后的文件结束时间可以优化用video->clock代表文件当前播放的时间 和文件结束时间不匹配
//2:?? 有时2次以上打开文件会卡住或中断 （优化后已经很少）
//3:?? 暂停时改变窗口大小，图像不会跟着改变 

typedef struct PacketQueue 
{
	AVPacketList *first_pkt, *last_pkt;
	int nb_packets;					//为包的总数
	int size;						//为所有包的大小
	int abort_request;
	SDL_mutex *mutex;				//互斥锁
	SDL_cond *cond;					//条件变量
} PacketQueue;

typedef struct VideoPicture
{
	AVFrame *bmp;
	int width, height; /* source height & width */
	int allocated;
	double pts;
	int skip;
} VideoPicture;


typedef struct StreamState_t 
{
	AVFormatContext *pFormatCtx;
	AVStream        *audio_st;
	AVStream        *video_st;
	AVFrame			*pFrameYuv;
	AVFrame			*pFrameAudio;
	PacketQueue     audioq;
	PacketQueue     videoq;
	uint8_t         audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
	unsigned int    audio_buf_size;
	unsigned int    audio_buf_index;
	AVPacket        audio_pkt;
	AVPacket			audio_pkt_tmp;
	uint8_t         *audio_pkt_data;
	int             audio_pkt_size;
	VideoPicture    pictq[VIDEO_PICTURE_QUEUE_SIZE];
	int             pictq_size, pictq_rindex, pictq_windex;
	SDL_mutex       *pictq_mutex;     //互斥变量
	SDL_cond        *pictq_cond;      //条件变量
	SDL_Thread      *read_tid;        //这是从文件读取数据放入队列的线程
	SDL_Thread      *video_tid;       //这是视频读取放入队列线程
	SDL_Thread      *refresh_tid;     //这是刷新线程
	double          audio_clock;      //音频的时间戳 
	double          video_clock;      //视频的时间戳
	double          seek_time;        //要移动的时间（秒）
	int             seek_req;         //seek的标志 是否需要seek
	int             seek_flags;       //seek的方式 AVSEEK_FLAG_FRAME等
	int64_t         seek_pos;         //seek过后的时间
	int				force_refresh;	 //强制刷新
	int				av_sync_type;	 //同步类型
	int				video_pkt_pts;	 //视频包PTS
	int				paused;          //0表示暂停 1表示播放
	int				quit;			 //是否推出
	double			video_current_pts;
	double			video_current_pts_drift;
	double			external_clock; /* external clock base */
	int64_t			external_clock_time;
	double          frame_timer;
	double          frame_last_pts;
	double          frame_last_delay;
	double			audio_diff_cum;
	double			audio_diff_avg_coef;
	double			audio_diff_threshold;
	int				audio_diff_avg_count;
	SDL_TimerID		refresh_time_id;
	int				refresh;
	SDL_mutex		*refresh_mutex;
	struct			SwsContext *img_convert_ctx;
} StreamState;



// CFFMPEG_SDL_MFC_PLAYER_1Dlg dialog
class CFFMPEG_SDL_MFC_PLAYER_1Dlg : public CDialog
{
// Construction
public:
	CFFMPEG_SDL_MFC_PLAYER_1Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FFMPEG_SDL_MFC_PLAYER_1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	//MFC默认的

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	//自己添加的

public:
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonPlayPause();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy); //当窗口大小调整时调用
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI); //这里是控制窗口最小值的，当窗口缩小时会导致控件重叠
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point); //双击CSTATIC控件全屏
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar); //进度条控制
	afx_msg void OnBnClickedCheckMute();     //设置静音复选框
	//获取控件在文件中的位置
	int m_IsAccess_control_position;        //是否已经获取了控件的初始化位置

	int m_width_MAINFRAME;                  //主窗口的宽
	int m_height_BMAINFRAME;                //主窗口的高

	CRect m_rect_BUTTON_LOAD;               //加载按钮初始化的位置
	int m_width_BUTTON_LOAD;                //加载按钮的宽
	int m_height__BUTTON_LOAD;              //加载按钮的高

	CRect m_rect_BUTTON_OPEN;               //打开按钮初始化的位置
	int m_width_BUTTON_OPEN;                //打开按钮的宽
	int m_height__BUTTON_OPEN;              //打开按钮的高

	CRect m_rect_BUTTON_PLAY_PAUSE;         //播放暂停按钮初始化的位置
	int m_width_BUTTON_PLAY_PAUSE;          //播放暂停按钮的宽
	int m_height__BUTTON_PLAY_PAUSE;        //播放暂停按钮的高

	CRect m_rect_BUTTON_STOP;               //停止按钮初始化的位置
	int m_width_BUTTON_STOP;                //停止按钮的宽
	int m_height__BUTTON_STOP;              //停止按钮的高

	CRect m_rect_BUTTON_CANCEL;             //退出按钮初始化的位置
	int m_width_BUTTON_CANCEL;              //退出按钮的宽
	int m_height__BUTTON_CANCEL;            //退出按钮的高

	CRect m_rect_CHECK_MUTE;                //静音复选框初始化的位置
	int m_width_CHECK_MUTE;                 //静音复选框的宽
	int m_height_CHECK_MUTE;                //静音复选框的高

	CRect m_rect_SLIDER_VOLUME;             //音量控制初始化的位置
	int m_width_SLIDER_VOLUME;              //音量控制的宽
	int m_height_SLIDER_VOLUME;             //音量控制的高

	CRect m_rect_LIST_CONTROL;              //显示视频信息的列表框控初始化的位置
	int m_width_LIST_CONTROL;               //显示视频信息的列表框控的宽
	int m_height_LIST_CONTROL;              //显示视频信息的列表框控的高

	CRect m_rect_CURRENT_TIME;              //当前播放时间初始化的位置
	int m_width_CURRENT_TIME;               //当前播放时间的宽
	int m_height_CURRENT_TIME;              //当前播放时间的高

	CRect m_rect_ALL_TIME;                  //文件总时长初始化的位置
	int m_width_ALL_TIME;                   //文件总时长的宽
	int m_height_ALL_TIME;                  //文件总时长的高

	CRect m_rect_SLIDER_PLAYL;              //进度条初始化的位置
	int m_width_SLIDER_PLAYL;               //进度条的宽
	int m_height_SLIDER_PLAY;               //进度条的高

	CRect m_rect_PICURE_CONTROL;            //播放区域初始化的位置
	int m_width_PICURE_CONTROL;             //播放区域的宽
	int m_height_PICURE_CONTROL;            //播放区域的高

	//获取控件初始化位置 //只调用一次
	int Access_control_position();      
	//程序启动做的初始化 打开编解码库 SDL初始化等,创建默认窗口  //只调用一次
	int InitProgram(); 
	//程序退出做的析构关闭库销毁窗口等  //只调用一次
	int UinitProgram();
	//程序所需要的变量分配等
	int InitVariable();
	//程序所需要的变量析构等
	int UinitVariable();
	//在文件中找到流标号
	int Open_codec_context(int * stream_idx, AVFormatContext * fmt_ctx, enum AVMediaType type); 
	//创建 创建音频回调函数，视频线程
	int stream_component_open(StreamState *is, int stream_index);
	//关闭视频 关闭音频设备
	int stream_component_close(StreamState *is, int stream_index); 
	//始化队列
	void packet_queue_init(PacketQueue *q) ;   
	//将packet填入队列（这里只是做的和刷新packet判断）
	int packet_queue_put(PacketQueue *q, AVPacket *pkt);  
	int packet_queue_put_nullpacket(PacketQueue *q, int stream_index);
	//从队列里面取出数据
	int packet_queue_get(PacketQueue *q, AVPacket *pkt, int queue_type);
	//终止向队列输入输出
	void packet_queue_abort(PacketQueue *q);             
	//真正的填入队列函数
	int packet_queue_put_private(PacketQueue *q, AVPacket *pkt);    
	//刷新packet的初始化
	void packet_queue_start(PacketQueue *q); 
	//销毁队列 
	void packet_queue_destroy(PacketQueue *q);     
	//清空队列
	void packet_queue_flush(PacketQueue *q); 
	//刷新操作（视频）
	void video_refresh(void *opaque);  
	//解码音频数据
	int audio_decode_frame(StreamState *is, uint8_t *audio_buf, int buf_size,double *pts_ptr);  
	//将解码后的数据添加到VideoPicture结构体中
	int queue_picture(StreamState *is, AVFrame *pFrame, double pts); 
	//显示视频
	void video_display(StreamState *is);  
	//创建输出SDL_Overlay yuv容器
	void alloc_picture(void *userdata) ;
	//创建打开窗口设置大小
	int video_open(StreamState *is);    
	void schedule_refresh(StreamState *is, int delay);
public:
	StreamState  * m_streamstate;            //音视频全局结构体
	int m_fs_screen_width;                   //全屏的宽
	int m_fs_screen_height;                  //全屏的高
	AVPacket m_flush_pkt;                    //刷新packet
	CString m_sourceFile;                    //要打开的文件路径
	double m_file_duration;                  //文件总时常
	AVCodecContext * m_video_dec_ctx ;       //视频解码context
	AVCodecContext * m_audio_dec_ctx ;       //音频解码context
	AVCodec * m_pvideo_codec;                //视频解码器
	AVCodec * m_paudio_codec;                //音频解码器
	SDL_Window *m_show_screen;				//sdl
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Surface* m_screen_surface;
	SDL_Rect sdlRect;
	SDL_Rect sdlRect_2;
	int m_is_full_screen;                    //是否全屏播放
	int m_screen_width ;                     //改变大小后的宽和高
	int m_screen_height;                     //改变大小后的宽和高
	int m_Isstop;                            //是否停止 0表示播放 1表示停止
	int m_slider_pos ;                       //播放滚动条要到的位置
	int m_stream_type;                       //文件类型 1：音视频都包含，2：只包含音频 3：只包含视频
	//video pram
	int m_video_stream_idx;                  //视频在文件中的流标号
	double m_dbFrameRate;                    //视频帧率
	double m_video_duration;                 //视频单帧持续时间 秒（s）
	int m_dwWidth;                           //宽
	int m_dwHeight;                          //高
	AVCodecID m_video_codecID;               //视频编码类型
	AVPixelFormat m_video_pixelfromat;       //视频yuv类型
	char m_spspps[1000];                      //spspps_buf
	int m_spspps_size;                       //spspps_size
	//audio pram
	int m_audio_stream_idx;                  //音频在文件中的流标号
	double m_audio_duration;                 //音频单帧持续时间 秒（s）
	int m_dwChannelCount;                    //声道
	int m_dwBitsPerSample;                   //样本
	int m_dwFrequency;                       //采样率
	AVCodecID m_audio_codecID;               //视频编码类型
	int m_audio_frame_size;                  //一帧数据大小
	int m_IsEnd_audio;                       //音频是否播放完毕 0 未播放完毕 1播放完毕
	int m_Volume_pos;                        //音量控制变量 0-128

	//音频相关-start				
	AVSampleFormat out_sample_fmt;
	struct SwrContext *au_convert_ctx;
	uint8_t *out_buffer;
	int out_nb_samples;
	int out_buffer_size;
	int out_channels;
	//音频相关-end
public:
	CListCtrl m_listControl;                 //显示视频信息的列表框控件变量
	CSliderCtrl m_sliderPlay;                //进度条变量
	CStatic m_picture_static;                //播放区域指针
	CRect m_Picture_rect;                    //显示图像的区域大小
	CSliderCtrl m_sliderVolume;              //音量进度条

private:
	BOOL m_blUrl;							//是否启用播放网络流
	BOOL m_blOpen;							//是否打开文件
	int InitParamInfo();						//初始化视频参数信息
	void InitDlgParamInfo();					//初始化界面显示信息
	void SetCurScreenStyle(int iWidthNum, int iHeightNum);//设置窗体的播放形态
public:
	CComboBox m_cboScreenStyle;
	afx_msg void OnCbnSelchangeComboShowType();
	CButton m_chkUrl;
	afx_msg void OnBnClickedChkUrl();
};

int read_thread( LPVOID lpParam );           //从文件读取音视频 创建刷新线程
int refresh_thread(LPVOID lpParam);          //刷新线程
void audio_callback(void *userdata, unsigned char *stream, int len) ;   //音频回调函数
int video_thread(void *arg);                                            //视频解码线程将解码后的数据传入VideoPicture结构体