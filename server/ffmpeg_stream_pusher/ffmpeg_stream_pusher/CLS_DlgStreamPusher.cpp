
// ffmpeg_stream_pusherDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CLS_StreamPusher.h"
#include "CLS_DlgStreamPusher.h"
#include ".\common\Public_Function.h"
//#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

FILE *pFile = NULL;
static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;
int tempsize = 4608;
char *tempcontenx = (char*)malloc(4608);//4096~4608
int  tempcontenxlen;

static char *dup_wchar_to_utf8(wchar_t *w)
{
	char *s = NULL;
	int l = WideCharToMultiByte(CP_UTF8, 0, w, -1, 0, 0, 0, 0);
	s = (char *)av_malloc(l);
	if (s)
		WideCharToMultiByte(CP_UTF8, 0, w, -1, s, l, 0, 0);
	return s;
}

// CLS_DlgStreamPusher 对话框
CLS_DlgStreamPusher::CLS_DlgStreamPusher(CWnd* pParent /*=NULL*/)
	: CDialog(CLS_DlgStreamPusher::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_cstrFilePath = "";
	m_blUrl = FALSE;
	m_bkBrush = NULL;
	m_pStreamInfo = NULL;
	m_mapDeviceInfo.clear();
}

void CLS_DlgStreamPusher::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_SRC_TYPE, m_chkSrcType);
	DDX_Control(pDX, IDC_EDT_LOACL_FILE_PATH, m_edtLocalFilePath);
	DDX_Control(pDX, IDC_BTN_OPEN_LOCAL_FILE, m_btnOpenLocalFile);
	DDX_Control(pDX, IDC_EDT_PUSHER_ADDR, m_edtPuserAddr);
	DDX_Control(pDX, IDC_STC_PREVIEW, m_stcPreview);
	DDX_Control(pDX, IDC_COB_DEVICE_VIDEO, m_cboDeviceVideo);
	DDX_Control(pDX, IDC_COB_DEVICE_AUDIO, m_cboDeviceAudio);
}

BEGIN_MESSAGE_MAP(CLS_DlgStreamPusher, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CLS_DlgStreamPusher::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_SRC_TYPE, &CLS_DlgStreamPusher::OnBnClickedChkSrcType)
	ON_BN_CLICKED(IDC_BTN_OPEN_LOCAL_FILE, &CLS_DlgStreamPusher::OnBnClickedBtnOpenLocalFile)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, &CLS_DlgStreamPusher::OnBnClickedBtnPreview)
	ON_BN_CLICKED(IDCANCEL, &CLS_DlgStreamPusher::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_DEVICE_VIDEO_TEST, &CLS_DlgStreamPusher::OnBnClickedBtnDeviceVideoTest)
	ON_BN_CLICKED(IDC_BTN_DEVICE_AUDIO_TEST, &CLS_DlgStreamPusher::OnBnClickedBtnDeviceAudioTest)
END_MESSAGE_MAP()


// CLS_DlgStreamPusher 消息处理程序

BOOL CLS_DlgStreamPusher::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//数据结构初始化
	InitData();

	//界面信息初始化
	InitDlgItem();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLS_DlgStreamPusher::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLS_DlgStreamPusher::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLS_DlgStreamPusher::OnBnClickedOk()
{
	//确定进行推流TODO::进行推流操作
	//ffmpeg - r 25 - f dshow - s 640×480 - i video = ”video source name” : audio = ”audio source name” - vcodec libx264 - b 600k - vpre slow - acodec libfaac - ab 128k - f flv rtmp ://server/application/stream_name
}


void CLS_DlgStreamPusher::OnBnClickedChkSrcType()
{
	//选择推送源类型
	if (m_chkSrcType.GetCheck()){
		m_blUrl = TRUE;
		m_edtLocalFilePath.EnableWindow(FALSE);
		m_btnOpenLocalFile.EnableWindow(FALSE);
	}
	else{
		m_blUrl = FALSE;
		m_edtLocalFilePath.EnableWindow(TRUE);
		m_btnOpenLocalFile.EnableWindow(TRUE);
	}
}


void CLS_DlgStreamPusher::OnBnClickedBtnOpenLocalFile()
{
	//打开本地文件
	CString szFilter = _T("All Files (*.*)|*.*|avi Files (*.avi)|*.avi|rmvb Files (*.rmvb)|*.rmvb|3gp Files (*.3gp)|*.3gp|mp3 Files (*.mp3)|*.mp3|mp4 Files (*.mp4)|*.mp4|mpeg Files (*.ts)|*.ts|flv Files (*.flv)|*.flv|mov Files (*.mov)|*.mov||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilter, NULL);
	if (IDOK == dlg.DoModal())
	{
		m_cstrFilePath = dlg.GetPathName();
	}
	else
	{
		MessageBox(_T("获取文件名称失败 请重新加载"), NULL, MB_OK);
		m_cstrFilePath = "";
		return;
	}
}


void CLS_DlgStreamPusher::OnBnClickedBtnPreview()
{
	//源视频流预览
	if (m_cstrFilePath == ""){
		MessageBox("请选择进行推送的文件!");
		return;
	}

	//进行文件解析进行显示
	if (m_blUrl){
		if (UrlStreamColParse() < 0){
			goto END;
		}
	}
	else{
		if (LocalFileParse() < 0){
			goto END;
		}
	}

END:
	MessageBox("解析失败，请确认网络连接无误!");
	return;
}


void CLS_DlgStreamPusher::OnBnClickedCancel()
{
	//先释放相关资源
	UnInitInfo();

	CoUninitialize();

	//退出程序
	CDialog::OnCancel();
}

void CLS_DlgStreamPusher::InitDlgItem()
{
	//进行界面信息的初始化操作
	//默认是网络流推送
	m_chkSrcType.SetCheck(1);

	m_blUrl = TRUE;
	m_edtLocalFilePath.EnableWindow(FALSE);
	m_btnOpenLocalFile.EnableWindow(FALSE);

	if (NULL == m_bkBrush)
	{
		m_bkBrush = CreateSolidBrush(COLOR_BLACK);
	}

	//初始化网络库
	av_register_all();
	avformat_network_init();
	avdevice_register_all();

	//初始化directshow库
	if (FAILED(CoInitialize(NULL))){
		TRACE("CoInitialize Failed!\r\n");
		return;
	}

	//获取当前连接的视频设备
	int iRet= GetDeviceInfo(n_Video);
	if (iRet < 0){
		TRACE("获取视频设备失败！");
		return;
	}

	//获取当前连接输入的音频设备
	iRet = GetDeviceInfo(n_Audio);
	if (iRet < 0){
		TRACE("获取音频设备失败！");
		return;
	}

	//将获取到的设备信息插入下拉框中
	std::map<int, std::vector<std::string>>::iterator iter = m_mapDeviceInfo.begin();
	for (; iter != m_mapDeviceInfo.end(); iter ++){
		if (n_Video == iter->first){
			//视频设备
			for (int i = 0; i < iter->second.size(); i ++){
				int iCount = m_cboDeviceVideo.GetCount();
				m_cboDeviceVideo.InsertString(iCount, iter->second[i].c_str());
			}
			m_cboDeviceVideo.SetCurSel(0);
		}
		else if (n_Audio == iter->first){
			//音频设备
			for (int i = 0; i < iter->second.size(); i ++){
				int iCount = m_cboDeviceAudio.GetCount();
				m_cboDeviceAudio.InsertString(iCount, iter->second[i].c_str());
			}
			m_cboDeviceAudio.SetCurSel(0);
		}
	}

	//SDL初始化
	int flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
	int  sdlinit = SDL_Init(flags);
	if (sdlinit)
	{
		char * sss = (char*)SDL_GetError();
		fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
		fprintf(stderr, "(Did you set the DISPLAY variable?)\n");
		return;
	}

	//将CSTATIC控件和sdl显示窗口关联 
	HWND hWnd = this->GetDlgItem(IDC_STC_PREVIEW)->GetSafeHwnd();
	if (hWnd != NULL)
	{
		if (m_pStreamInfo != NULL){
			m_pStreamInfo->m_show_screen = SDL_CreateWindowFrom((void*)hWnd);
			if (m_pStreamInfo->m_show_screen == NULL){
				TRACE("SDL_CreateWindowFrom ERR(%d)\n", SDL_GetError());
				return;
			}
		} 
	}

	//设置SDL事件状态
	SDL_EventState(SDL_WINDOWEVENT, SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	return;
}

void CLS_DlgStreamPusher::UnInitInfo()
{
	//释放相关资源
	if (m_cstrFilePath != ""){
		m_cstrFilePath.ReleaseBuffer();
		m_cstrFilePath = "";
	}
	if (NULL != m_bkBrush)
	{
		DeleteObject(m_bkBrush);
		m_bkBrush = NULL;
	}

	m_mapDeviceInfo.clear();
	return;
}

HBRUSH CLS_DlgStreamPusher::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (nCtlColor){
	case CTLCOLOR_STATIC:
		if (pWnd->GetDlgCtrlID() == IDC_STC_PREVIEW){
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(COLOR_BLACK);
			return m_bkBrush;
		}
	}
	return   hbr;
}

int CLS_DlgStreamPusher::UrlStreamColParse()
{
	int iRet = -1;

	return iRet;
}

int CLS_DlgStreamPusher::LocalFileParse()
{
	int iRet = -1;

	return iRet;
}

int CLS_DlgStreamPusher::GetDeviceInfo(int _iDeviceType)
{
	int iRet = -1;
	ICreateDevEnum *pDevEnum;
	IEnumMoniker   *pEnumMon;
	IMoniker	   *pMoniker;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (LPVOID*)&pDevEnum);
	if (SUCCEEDED(hr))
	{
		if (_iDeviceType == n_Video){
			hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMon, 0);
		}
		else{
			hr = pDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEnumMon, 0);
		}
		if (hr == S_FALSE)
		{
			TRACE("没有找到合适的音视频设备！");
			hr = VFW_E_NOT_FOUND;
			return hr;
		}
		pEnumMon->Reset();
		ULONG cFetched;
		while (hr = pEnumMon->Next(1, &pMoniker, &cFetched), hr == S_OK)
		{
			IPropertyBag *pProBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (LPVOID*)&pProBag);
			if (SUCCEEDED(hr))
			{
				VARIANT varTemp;
				varTemp.vt = VT_BSTR;
				hr = pProBag->Read(L"FriendlyName", &varTemp, NULL);
				if (SUCCEEDED(hr))
				{
					iRet = 0;
					_bstr_t bstr_t(varTemp.bstrVal);
					std::string strDeviceName = bstr_t;

					//将取到的设备名称存入容器中
					std::vector<std::string> vecDeviceName;
					vecDeviceName.clear();
					vecDeviceName.push_back(strDeviceName);
					SysFreeString(varTemp.bstrVal);

					//如果容器相应的键值有值就量进行相应的添加处理，没值的话整体进行添加处理
					std::map<int, std::vector<std::string>>::iterator iterInfo = m_mapDeviceInfo.find(_iDeviceType);
					if (iterInfo == m_mapDeviceInfo.end()){
						m_mapDeviceInfo.insert(map<int, std::vector<std::string>>::value_type(_iDeviceType, vecDeviceName));
					}
					else{
						m_mapDeviceInfo[_iDeviceType].push_back(strDeviceName);
					}
				}
				pProBag->Release();
			}
			pMoniker->Release();
		}
		pEnumMon->Release();
	}

	return iRet;
}

int video_thr(LPVOID lpParam)
{
	int iRet = -1;
	AVFormatContext		*	pFmtCtx				= NULL;
	AVFormatContext		*	pFOutmtCtx			= NULL;
	AVInputFormat		*	pVideoInputFmt		= NULL;
	AVOutputFormat		*	pVideoOutputFmt		= NULL;
	AVStream				*	pVideoStream		= NULL;
	AVCodecContext		*	pOutputCodecCtx		= NULL;
	AVPacket				*	pVideoPacket		= NULL;
	int						iVideoIndex			= -1;
	int						iVideo_Height		= 0;
	int						iVideo_Width		= 0;
	CLS_DlgStreamPusher* pThis = (CLS_DlgStreamPusher*)lpParam;
	if (pThis == NULL){
		TRACE("video_thr--pThis == NULL\n");
		return iRet;
	}
	pVideoInputFmt = av_find_input_format("dshow");
	if (pVideoInputFmt == NULL){
		TRACE("pVideoInputFmt == NULL\n");
		return iRet;
	}

	char* psDevName = pThis->GetDeviceName(n_Video);
	if (psDevName == NULL){
		TRACE("video_thr--psDevName == NULL");
		return iRet;
	}

	if (avformat_open_input(&pFmtCtx, psDevName, pVideoInputFmt, NULL) != 0){
		TRACE("avformat_open_input err!\n");
		goto END;
	}
	if (avformat_find_stream_info(pFmtCtx, NULL) < 0){
		TRACE("avformat_find_stream_info(pFmtCtx, NULL) < 0\n");
		goto END;
	}

	for (int i = 0; i < pFmtCtx->nb_streams; i++){
		if (pFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			iVideoIndex = i;
			break;
		}
	}

	if (iVideoIndex < 0){
		TRACE("iVideoIndex < 0\n");
		goto END;
	}

	//获取视频的宽与高
	iVideo_Height = pFmtCtx->streams[iVideoIndex]->codec->height;
	iVideo_Width = pFmtCtx->streams[iVideoIndex]->codec->width;
	TRACE("video_thr--video_height[%d],video_width[%d]", iVideo_Height
	, iVideo_Width);

	//从摄像头获取数据
	if (0 == av_read_frame(pFmtCtx,pVideoPacket)){
		//找到解码器
		AVCodec* pVideo_Decode = avcodec_find_decoder(pFmtCtx->streams[iVideoIndex]->codec->codec_id);
		if (NULL == pVideo_Decode){
			TRACE("NULL == pVideo_Decode");
			goto END;
		}

		//找到之后打开解码器
		if (0 != avcodec_open2(pFmtCtx->streams[iVideoIndex]->codec, pVideo_Decode,NULL)){
			TRACE("!avcodec_open2");
			goto END;
		}
	}

	iRet = 1;
END:
	avformat_close_input(&pFmtCtx);
	return iRet;
}

void CLS_DlgStreamPusher::OnBnClickedBtnDeviceVideoTest()
{
	//视频测试，开启线程打开本地摄像头
	if (m_pStreamInfo->m_test_video_tid == NULL){
		m_pStreamInfo->m_test_video_tid = SDL_CreateThread(video_thr, NULL, (void*)this);
		if (m_pStreamInfo->m_test_video_tid == NULL){
			MessageBox("创建视频测试线程失败！");
			return;
		}
	}
}

void  CLS_DlgStreamPusher::fill_audio(void *udata, Uint8 *stream, int len)
{
	SDL_memset(stream, 0, len);
	memset(tempcontenx, 0, 4608);
	memcpy_s(tempcontenx, 4608, audio_pos, 4608);
	tempcontenxlen = len;

	if (audio_len == 0)		/*  Only  play  if  we  have  data  left  */
		return;
	len = (len>audio_len ? audio_len : len);	/*  Mix  as  much  data  as  possible  */

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}

int audio_thr(LPVOID lpParam)
{
	int iRet = -1;
	//音频测试，播放文件显示波形
	AVFormatContext		*	pFmtCtx = NULL;
	AVFormatContext		*	pFOutmtCtx = NULL;
	AVInputFormat		*	pAudioInputFmt = NULL;
	AVOutputFormat		*	pAudioOutputFmt = NULL;
	AVStream				*	pAudioStream = NULL;
	AVCodecContext		*	pOutputCodecCtx = NULL;
	AVPacket				*	pAudioPacket = NULL;
	int						iAudioIndex = -1;
	struct SwrContext	*	au_convert_ctx;
	uint8_t				*	out_buffer = 0;

	CLS_DlgStreamPusher* pThis = (CLS_DlgStreamPusher*)lpParam;
	if (pThis == NULL){
		TRACE("audio_thr--pThis == NULL\n");
		return iRet;
	}

	pAudioInputFmt = av_find_input_format("dshow");
	if (pAudioInputFmt == NULL){
		TRACE("pAudioInputFmt == NULL\n");
		return iRet;
	}

	char* psDevName = pThis->GetDeviceName(n_Audio);
	if (psDevName == NULL){
		TRACE("audio_thr--psDevName == NULL");
		return iRet;
	}

	if (avformat_open_input(&pFmtCtx, psDevName, pAudioInputFmt, NULL) != 0){
		TRACE("avformat_open_input err!\n");
		goto END;
	}
	if (avformat_find_stream_info(pFmtCtx, NULL) < 0){
		TRACE("avformat_find_stream_info(pFmtCtx, NULL) < 0\n");
		goto END;
	}

	for (int i = 0; i < pFmtCtx->nb_streams; i++){
		if (pFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
			iAudioIndex = i;
			AVCodec *tmpCodec = avcodec_find_decoder(pFmtCtx->streams[i]->codec->codec_id);
			if (0 > avcodec_open2(pFmtCtx->streams[i]->codec, tmpCodec, NULL)){
				TRACE("can not find or open decoder!\n");
			}
			break;
		}
	}
	AVCodec* audio_encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (audio_encoder == NULL){
		TRACE("audio_encoder == NULL\r\n");
		goto END;
	}

	pOutputCodecCtx = avcodec_alloc_context3(audio_encoder);
	if (pOutputCodecCtx == NULL){
		TRACE("pOutputCodecCtx == NULL");
		goto END;
	}

	pOutputCodecCtx->sample_rate = pFmtCtx->streams[0]->codec->sample_rate;
	pOutputCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
	pOutputCodecCtx->channels = av_get_channel_layout_nb_channels(pOutputCodecCtx->channel_layout);
	pOutputCodecCtx->sample_fmt = audio_encoder->sample_fmts[0];
	pOutputCodecCtx->codec = audio_encoder;
	pOutputCodecCtx->codec_tag = 0;
	if (avcodec_open2(pOutputCodecCtx, pOutputCodecCtx->codec, 0) < 0){
		//编码器打开失败，退出程序
		TRACE("音频编码器打开失败！\n");
		goto END;
	}

	//SDL_AudioSpec
	int out_nb_samples = AUDIO_BUF_SIZE;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_buffer_size = av_samples_get_buffer_size(NULL, pOutputCodecCtx->channels, out_nb_samples, out_sample_fmt, 1);
	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = pOutputCodecCtx->sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = pOutputCodecCtx->channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = out_nb_samples;
	wanted_spec.callback = &CLS_DlgStreamPusher::fill_audio;
	wanted_spec.userdata = pOutputCodecCtx;
	if (NULL == pThis->m_pStreamInfo){
		TRACE("NULL == pThis->m_pStreamInfo");
		goto END;
	}

	pThis->m_pStreamInfo->m_content_out_channels = pOutputCodecCtx->channels;

	if (SDL_OpenAudio(&wanted_spec, NULL)<0){
		TRACE("can't open audio.\n");
		goto END;
	}

	AVPacket pkt, pkt_out;
	AVFrame *frame;
	AVAudioFifo *fifo = NULL;
	fifo = av_audio_fifo_alloc(pOutputCodecCtx->sample_fmt, pOutputCodecCtx->channels, 1);

	int frameIndex = 0;
	out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	
	int64_t in_channel_layout = av_get_default_channel_layout(pOutputCodecCtx->channels);

	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, pOutputCodecCtx->channel_layout, out_sample_fmt, pOutputCodecCtx->sample_fmt,
		in_channel_layout, pOutputCodecCtx->sample_fmt, pOutputCodecCtx->sample_rate, 0, NULL);
	swr_init(au_convert_ctx);

	while (av_read_frame(pFmtCtx, &pkt) == 0 && _kbhit() == 0){
		if (pkt.stream_index != iAudioIndex){
			continue;
		}

		int gotframe = -1;
		frame = av_frame_alloc();
		if (avcodec_decode_audio4(pFmtCtx->streams[iAudioIndex]->codec, frame, &gotframe, &pkt) < 0){
			av_frame_free(&frame);
			TRACE("can not decoder a frame\n");
			break;
		}
		av_free_packet(&pkt);

		if (!gotframe){
			//没有获取到数据，继续下一次
			continue;
		}

		swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)frame->data, frame->nb_samples);

		//FIX:FLAC,MP3,AAC Different number of samples
		if (wanted_spec.samples != frame->nb_samples){
			SDL_CloseAudio();
			out_nb_samples = frame->nb_samples;
			out_buffer_size = av_samples_get_buffer_size(NULL, pOutputCodecCtx->channels, out_nb_samples, out_sample_fmt, 1);
			wanted_spec.samples = out_nb_samples;
			SDL_OpenAudio(&wanted_spec, NULL);
		}

		//设置PCM数据
		audio_chunk = (Uint8 *)out_buffer;
		audio_len = out_buffer_size;
		audio_pos = audio_chunk;

		//播放
		SDL_PauseAudio(0);
		while (audio_len > 0){
			//Wait until finish
			SDL_Delay(1);
		}

		av_free_packet(&pkt);

		//写PCM进行test
		//if (1){
		//	FILE *p = NULL;
		//	fopen_s(&p, "test.pcm", "a+b");
		//	if (p == NULL){
		//		continue;
		//	}
		//	int tempLenght = 2 * frame->nb_samples;//由于实验中知道这是16位深，所以才这么写
		//	uint8_t *tmpPtr = frame->data[0];
		//	if (NULL != p)
		//	{
		//		while (tempLenght > 0)
		//		{
		//			size_t temp = fwrite(tmpPtr, 1, tempLenght, p);
		//			tmpPtr += temp;
		//			tempLenght = tempLenght - temp;
		//		}
		//		fclose(p);
		//	}
		//}

		//if (pFmtCtx->streams[iAudioIndex]->codec->sample_fmt != pOutputCodecCtx->sample_fmt
		//	|| pFmtCtx->streams[iAudioIndex]->codec->channels != pOutputCodecCtx->channels
		//	|| pFmtCtx->streams[iAudioIndex]->codec->sample_rate != pOutputCodecCtx->sample_rate){
		//	//TODO如果输入和输出的音频格式不一样 需要重采样，这里是一样的就没做
		//}

		//av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame->nb_samples);
		//av_audio_fifo_write(fifo, (void **)frame->data, frame->nb_samples);

		////循环读取数据，直到buf里数据采样数不够
		//while (av_audio_fifo_size(fifo) >= (pOutputCodecCtx->frame_size > 0 ? pOutputCodecCtx->frame_size : AUDIO_BUF_SIZE))
		//{
		//	av_frame_free(&frame);
		//	frame = av_frame_alloc();
		//	frame->nb_samples = pOutputCodecCtx->frame_size>0 ? pOutputCodecCtx->frame_size : AUDIO_BUF_SIZE;
		//	frame->channel_layout = pOutputCodecCtx->channel_layout;
		//	frame->format = pOutputCodecCtx->sample_fmt;
		//	frame->sample_rate = pOutputCodecCtx->sample_rate;
		//	av_frame_get_buffer(frame, 0);

		//	av_audio_fifo_read(fifo, (void **)frame->data, (pOutputCodecCtx->frame_size > 0 ? pOutputCodecCtx->frame_size : AUDIO_BUF_SIZE));
		//	av_init_packet(&pkt_out);
		//	//frame->pts = pFrame->pts;
		//	int got_picture = -1;
		//	pkt_out.data = NULL;
		//	pkt_out.size = 0;
		//	if (avcodec_encode_audio2(pOutputCodecCtx, &pkt_out, frame, &got_picture) < 0){
		//		printf("can not decoder a frame");
		//	}
		//	av_frame_free(&frame);
		//	if (got_picture)
		//	{
		//		pkt_out.pts = frameIndex * pOutputCodecCtx->frame_size;
		//		pkt_out.dts = frameIndex * pOutputCodecCtx->frame_size;
		//		pkt_out.duration = pOutputCodecCtx->frame_size;

		//		//TODO将编码结果后续做合成处理[pkt_out]
		//		if (pFile != NULL){
		//			/*fwrite((uint8_t *)pDlg->m_streamstate->audio_buf + pDlg->m_streamstate->audio_buf_index, 1, len1, pFile);*/
		//		}
		//		frameIndex++;
		//	}
		//}
	}
	iRet = 1;

END:
	swr_free(&au_convert_ctx);
	SDL_CloseAudio();
	SDL_Quit();
	av_free(out_buffer);
	avcodec_close(pOutputCodecCtx);

	return iRet;
}
void CLS_DlgStreamPusher::OnBnClickedBtnDeviceAudioTest()
{
	//开启audio测试线程
	if (m_pStreamInfo->m_test_audio_tid == NULL){
		m_pStreamInfo->m_test_audio_tid = SDL_CreateThread(audio_thr,NULL,(void*)this);
		if (m_pStreamInfo->m_test_audio_tid == NULL){
			MessageBox("创建音频测试线程失败！");
			return;
		}
	}
}

void CLS_DlgStreamPusher::InitData()
{
	m_pStreamInfo = (struct_stream_info *)calloc(1, sizeof(struct_stream_info));
	if (NULL == m_pStreamInfo)
	{
		TRACE("m_streamstate is NULL!\n");
		return;
	}

	//初始化流媒体
	m_pStreamInfo->m_pFormatCtx = avformat_alloc_context();
	if (NULL == m_pStreamInfo->m_pFormatCtx){
		TRACE("NULL == m_pStreamInfo->m_pFormatCtx\n");
		return;
	}

	m_pStreamInfo->m_test_audio_tid = NULL;
	m_pStreamInfo->m_test_video_tid = NULL;
	m_pStreamInfo->m_show_screen = NULL;
}

char* CLS_DlgStreamPusher::GetDeviceName(int _iDeviceType)
{
	char* pDeviceName = NULL;
	CString cstrDevName = "";
	CString cstrOutDevName = "";
	if (_iDeviceType == n_Audio){
		m_cboDeviceAudio.GetWindowText(cstrDevName);
	}
	else if (_iDeviceType == n_Video){
		m_cboDeviceVideo.GetWindowText(cstrDevName);
	}

	AnsiToUTF8(cstrDevName.GetBuffer(), cstrOutDevName);
	if (_iDeviceType == n_Audio){
		cstrOutDevName = "audio=" + cstrDevName;
	}
	else if (_iDeviceType == n_Video){
		cstrOutDevName = "video=" + cstrDevName;
	}

	wchar_t* wstring = string_to_wstring(cstrOutDevName.GetBuffer());
	pDeviceName = dup_wchar_to_utf8(wstring);

	return pDeviceName;
}

struct_stream_info* CLS_DlgStreamPusher::GetStreamStrcInfo()
{
	return m_pStreamInfo;
}