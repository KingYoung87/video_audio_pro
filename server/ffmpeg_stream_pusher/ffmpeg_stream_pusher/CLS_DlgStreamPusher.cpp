
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
static int64_t audio_callback_time;

static char *dup_wchar_to_utf8(wchar_t *w)
{
	char *s = NULL;
	int l = WideCharToMultiByte(CP_UTF8, 0, w, -1, 0, 0, 0, 0);
	s = (char *)av_malloc(l);
	if (s)
		WideCharToMultiByte(CP_UTF8, 0, w, -1, s, l, 0, 0);
	return s;
}

static inline int compute_mod(int a, int b)
{
	return a < 0 ? a%b + b : a%b;
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
static int refresh_thread(void *opaque)
{
	struct_stream_info *pstrct_stream = (struct_stream_info *)opaque;
	if (NULL == pstrct_stream){
		TRACE("NULL == pstrct_stream\n");
		return -1;
	}
	while (!pstrct_stream->m_abort_request) {
		SDL_Event event;
		event.type = FF_REFRESH_EVENT;
		event.user.data1 = opaque;
		if (!pstrct_stream->m_refresh) {
			pstrct_stream->m_refresh = 1;
			SDL_PushEvent(&event);
		}
		//FIXME ideally we should wait the correct time but SDLs event passing is so slow it would be silly
		av_usleep(pstrct_stream->m_pAudioStream && pstrct_stream->m_show_mode != SHOW_MODE_VIDEO ? 20 * 1000 : 5000);
	}
	return 0;
}

UINT Thread_Event(LPVOID lpParam){
	CLS_DlgStreamPusher *dlg = (CLS_DlgStreamPusher *)lpParam;
	if (dlg != NULL){
		dlg->event_loop(dlg->m_pStreamInfo);
	}
	return 0;
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

			RECT rectDisPlay;
			this->GetDlgItem(IDC_STC_PREVIEW)->GetWindowRect(&rectDisPlay);
			m_pStreamInfo->m_xleft = rectDisPlay.left;
			m_pStreamInfo->m_ytop = rectDisPlay.top;
			m_pStreamInfo->m_width = rectDisPlay.right - rectDisPlay.left;
			m_pStreamInfo->m_height = rectDisPlay.bottom - rectDisPlay.top;

			//处理显示区域
			FillDisplayRect();

			m_pStreamInfo->m_sdlRenderer = SDL_CreateRenderer(m_pStreamInfo->m_show_screen, -1, 0);
			if (m_pStreamInfo->m_sdlRenderer == NULL){
				TRACE("SDL_CreateRenderer--sdlRenderer == NULL err(%d)\n", SDL_GetError());
				return;
			}

			//创建纹理
			m_pStreamInfo->m_sdlTexture = SDL_CreateTexture(m_pStreamInfo->m_sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_pStreamInfo->m_width, m_pStreamInfo->m_height);

			m_pStreamInfo->m_refresh_tid = SDL_CreateThread(refresh_thread,NULL,m_pStreamInfo);
		} 
	}

	//设置SDL事件状态
	SDL_EventState(SDL_WINDOWEVENT, SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	if (m_pThreadEvent == NULL){
		m_pThreadEvent = AfxBeginThread(Thread_Event, this);//开启线程
	}

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
	m_pStreamInfo->m_itest_start = 1;
	m_pStreamInfo->m_show_mode = SHOW_MODE_VIDEO;
	//视频测试，开启线程打开本地摄像头
	if (m_pStreamInfo->m_test_video_tid == NULL){
		m_pStreamInfo->m_test_video_tid = SDL_CreateThread(video_thr, NULL, (void*)this);
		if (m_pStreamInfo->m_test_video_tid == NULL){
			MessageBox("创建视频测试线程失败！");
			return;
		}
	}
}

void UpdateSampleDisplay(struct_stream_info *_pstrct_streaminfo, short *samples, int samples_size)//CLS_DlgStreamPusher::
{
	int size, len;

	size = samples_size / sizeof(short);
	while (size > 0) {
		len = SAMPLE_ARRAY_SIZE - _pstrct_streaminfo->m_sample_array_index;
		if (len > size)
			len = size;
		memcpy(_pstrct_streaminfo->m_sample_array + _pstrct_streaminfo->m_sample_array_index, samples, len * sizeof(short));
		samples += len;
		_pstrct_streaminfo->m_sample_array_index += len;
		if (_pstrct_streaminfo->m_sample_array_index >= SAMPLE_ARRAY_SIZE)
			_pstrct_streaminfo->m_sample_array_index = 0;
		size -= len;
	}
}

void  fill_audio(void *udata, Uint8 *stream, int len)//CLS_DlgStreamPusher::
{
	struct_stream_info* struct_stream = (struct_stream_info*)udata;
	if (struct_stream == NULL){
		TRACE("struct_stream == NULL \n");
		return;
	}
	audio_callback_time = av_gettime();

	int frame_size = av_samples_get_buffer_size(NULL, struct_stream->m_audio_tgt.channels, 1, struct_stream->m_audio_tgt.fmt, 1);
	int len1 = 0;

	if (audio_len == 0)	/*  Only  play  if  we  have  data  left  */
		return;
	len = (len>audio_len ? audio_len : len);	/*  Mix as  much  data  as  possible  */

	while (len > 0 && audio_len > 0){
		if (struct_stream->m_audio_buf_index >= struct_stream->m_audio_buf_size){
			if (struct_stream->m_aduio_pkt_size < 0){
				struct_stream->m_audio_buf = struct_stream->m_silence_buf;
				struct_stream->m_audio_buf_size = sizeof(struct_stream->m_silence_buf) / frame_size * frame_size;
			}
			else{
				if (struct_stream->m_show_mode == SHOW_MODE_WAVES){
					UpdateSampleDisplay(struct_stream, (int16_t *)struct_stream->m_audio_buf, struct_stream->m_aduio_pkt_size);
				}
				struct_stream->m_audio_buf_size = struct_stream->m_aduio_pkt_size;
			}
		}

		len1 = struct_stream->m_audio_buf_size - struct_stream->m_audio_buf_index;
		if (len1 > len)
			len1 = len;
		memcpy(stream, (uint8_t *)struct_stream->m_audio_buf + struct_stream->m_audio_buf_index, len1);
		audio_len -= len;
		len -= len1;
		stream += len1;
		struct_stream->m_audio_buf_index += len1;
	}
	audio_len = 0;

	struct_stream->m_audio_write_buf_size = struct_stream->m_audio_buf_size - struct_stream->m_audio_buf_index;
}

int audio_thr(LPVOID lpParam)
{
	int iRet = -1;
	//音频测试，播放文件显示波形
	AVFormatContext		*	pFmtCtx = NULL;
	AVFormatContext		*	pFOutmtCtx = NULL;
	AVInputFormat		*	pAudioInputFmt = NULL;
	AVOutputFormat		*	pAudioOutputFmt = NULL;
	AVCodecContext		*	pOutputCodecCtx = NULL;
	AVPacket				*	pAudioPacket = NULL;
	int						iAudioIndex = -1;
	int						data_size = 0;
	int						resampled_data_size = 0;
	uint8_t				*	out_buffer = 0;
	int64_t					dec_channel_layout = 0;
	double					pts;

	CLS_DlgStreamPusher* pThis = (CLS_DlgStreamPusher*)lpParam;
	if (pThis == NULL || pThis->m_pStreamInfo == NULL){
		TRACE("audio_thr--pThis == NULL || pThis->m_pStreamInfo == NULL\n");
		return iRet;
	}

	struct_stream_info* strct_stream_info = pThis->m_pStreamInfo;

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

	//找到音频流信息
	strct_stream_info->m_pAudioStream = pFmtCtx->streams[iAudioIndex];
	if (strct_stream_info->m_pAudioStream == NULL){
		TRACE("strct_stream_info->m_pAudioStream == NULL\n");
		goto END;
	}

	AVCodecContext *pAudioDec = strct_stream_info->m_pAudioStream->codec;
	if (NULL == pAudioDec){
		TRACE("NULL == pAudioDec\n");
		goto END;
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
	SDL_AudioSpec wanted_spec, spec;
	wanted_spec.freq = pOutputCodecCtx->sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = pOutputCodecCtx->channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = out_nb_samples;
	wanted_spec.callback = fill_audio;//&CLS_DlgStreamPusher::
	wanted_spec.userdata = strct_stream_info;
	strct_stream_info->m_content_out_channels = pOutputCodecCtx->channels;

	if (SDL_OpenAudio(&wanted_spec, &spec)<0){
		TRACE("can't open audio.\n");
		goto END;
	}

	int audio_hw_buf_size = spec.size;
	if (audio_hw_buf_size < 0){
		TRACE("audio_hw_buf_size < 0\n");
		return -1;
	}
	strct_stream_info->m_audio_src.fmt = AV_SAMPLE_FMT_S16;
	strct_stream_info->m_audio_src.freq = spec.freq;
	strct_stream_info->m_audio_src.channel_layout = pOutputCodecCtx->channel_layout;
	strct_stream_info->m_audio_src.channels = spec.channels;
	strct_stream_info->m_audio_hw_buf_size = audio_hw_buf_size;
	strct_stream_info->m_audio_tgt = strct_stream_info->m_audio_src;

	/*int frameIndex = 0;
	AVPacket pkt_out;
	AVAudioFifo *fifo = NULL
	fifo = av_audio_fifo_alloc(pOutputCodecCtx->sample_fmt, pOutputCodecCtx->channels, 1);*/

	AVPacket pkt;
	//AVFrame *frame;
	out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	/*int64_t in_channel_layout = av_get_default_channel_layout(pOutputCodecCtx->channels);
	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, pOutputCodecCtx->channel_layout, out_sample_fmt, pOutputCodecCtx->sample_fmt,
		in_channel_layout, pOutputCodecCtx->sample_fmt, pOutputCodecCtx->sample_rate, 0, NULL);
	swr_init(au_convert_ctx);*/

	while (av_read_frame(pFmtCtx, &pkt) == 0 && _kbhit() == 0){
		if (pkt.stream_index != iAudioIndex){
			continue;
		}
		if (!strct_stream_info->m_pAudioFrame) {
			if (!(strct_stream_info->m_pAudioFrame = avcodec_alloc_frame())){
				TRACE("!(strct_stream_info->m_pAudioFrame = avcodec_alloc_frame())\n");
				goto END;
			}
		}
		else{
			avcodec_get_frame_defaults(strct_stream_info->m_pAudioFrame);
		}

		int gotframe = -1;
		strct_stream_info->m_pAudioFrame = av_frame_alloc();
		if (avcodec_decode_audio4(pAudioDec, strct_stream_info->m_pAudioFrame, &gotframe, &pkt) < 0){
			av_frame_free(&strct_stream_info->m_pAudioFrame);
			TRACE("can not decoder a frame\n");
			break;
		}
		av_free_packet(&pkt);

		if (!gotframe){
			//没有获取到数据，继续下一次
			continue;
		}
		strct_stream_info->m_pAudioFrame->nb_samples = 1024;//这里暂时写死值
		data_size = av_samples_get_buffer_size(NULL, pOutputCodecCtx->channels,
			strct_stream_info->m_pAudioFrame->nb_samples,
			pOutputCodecCtx->sample_fmt, 1);

		dec_channel_layout =
			(pOutputCodecCtx->channel_layout && pOutputCodecCtx->channels == av_get_channel_layout_nb_channels(pOutputCodecCtx->channel_layout)) ?
			pOutputCodecCtx->channel_layout : av_get_default_channel_layout(pOutputCodecCtx->channels);
		//wanted_nb_samples = SynAudio(strct_stream_info, strct_stream_info->m_pAudioFrame->nb_samples);

		/*if (pOutputCodecCtx->sample_fmt != strct_stream_info->m_audio_src.fmt ||
			dec_channel_layout != strct_stream_info->m_audio_src.channel_layout ||
			pOutputCodecCtx->sample_rate != strct_stream_info->m_audio_src.freq){*/
			swr_free(&strct_stream_info->m_swr_ctx);
			strct_stream_info->m_swr_ctx = swr_alloc_set_opts(NULL,
				strct_stream_info->m_audio_tgt.channel_layout, strct_stream_info->m_audio_tgt.fmt, strct_stream_info->m_audio_tgt.freq,
				dec_channel_layout, pOutputCodecCtx->sample_fmt, pOutputCodecCtx->sample_rate,
				0, NULL);
			if (!strct_stream_info->m_swr_ctx || swr_init(strct_stream_info->m_swr_ctx) < 0){
				TRACE("!pThis->m_pStreamInfstrct_stream_infoo->m_swr_ctx || swr_init(strct_stream_info->m_swr_ctx) < 0");
				break;
			}

			strct_stream_info->m_audio_src.channel_layout = dec_channel_layout;
			strct_stream_info->m_audio_src.channels = pOutputCodecCtx->channels;
			strct_stream_info->m_audio_src.freq = pOutputCodecCtx->sample_rate;
			strct_stream_info->m_audio_src.fmt = pOutputCodecCtx->sample_fmt;
		//}
		if (NULL != strct_stream_info->m_swr_ctx){
			const uint8_t **in = (const uint8_t **)strct_stream_info->m_pAudioFrame->extended_data;
			uint8_t *out[] = { strct_stream_info->m_audio_buf2 };
			int out_count = sizeof(strct_stream_info->m_audio_buf2) / strct_stream_info->m_audio_tgt.channels / av_get_bytes_per_sample(strct_stream_info->m_audio_tgt.fmt);
			int iRet = swr_convert(strct_stream_info->m_swr_ctx, out, out_count, in, strct_stream_info->m_pAudioFrame->nb_samples);
			if (iRet < 0){
				TRACE("swr_convert < 0\n");
				break;
			}
			if (iRet == out_count) {
				TRACE("warning: audio buffer is probably too small\n");
				swr_init(strct_stream_info->m_swr_ctx);
			}
			strct_stream_info->m_audio_buf = strct_stream_info->m_audio_buf2;
			resampled_data_size = iRet * strct_stream_info->m_audio_tgt.channels * av_get_bytes_per_sample(strct_stream_info->m_audio_tgt.fmt);
		}
		else{
			strct_stream_info->m_audio_buf = strct_stream_info->m_pAudioFrame->data[0];
			resampled_data_size = data_size;
		}

		/* if no pts, then compute it */
		pts = strct_stream_info->m_audio_clock;
		//*pts_ptr = pts;
		strct_stream_info->m_audio_clock += (double)data_size /
			(pAudioDec->channels * pAudioDec->sample_rate * av_get_bytes_per_sample(pAudioDec->sample_fmt));
#ifdef DEBUG
		{
			static double last_clock;
			/*printf("audio: delay=%0.3f clock=%0.3f pts=%0.3f\n",
				is->audio_clock - last_clock,
				is->audio_clock, pts);*/
			last_clock = strct_stream_info->m_audio_clock;
		}
#endif

		//FIX:FLAC,MP3,AAC Different number of samples
		/*if (wanted_spec.samples != strct_stream_info->m_pAudioFrame->nb_samples){
			SDL_CloseAudio();
			out_nb_samples = strct_stream_info->m_pAudioFrame->nb_samples;
			out_buffer_size = av_samples_get_buffer_size(NULL, pOutputCodecCtx->channels, out_nb_samples, out_sample_fmt, 1);
			wanted_spec.samples = out_nb_samples;
			SDL_OpenAudio(&wanted_spec, NULL);
		}*/

		//设置PCM数据
		TRACE("----out_buffer_size---is [%ld]\n",out_buffer_size);
		audio_chunk = (Uint8 *)out_buffer;
		audio_len = out_buffer_size;
		audio_pos = audio_chunk;

		strct_stream_info->m_aduio_pkt_size = resampled_data_size;//audio_len;//

		av_free_packet(&pkt);

		//写PCM进行test
		if (1){
			FILE *p = NULL;
			fopen_s(&p, "test.pcm", "a+b");
			if (p == NULL){
				continue;
			}
			int tempLenght = 2 * strct_stream_info->m_pAudioFrame->nb_samples;//由于实验中知道这是16位深，所以才这么写
			uint8_t *tmpPtr = strct_stream_info->m_pAudioFrame->data[0];
			if (NULL != p)
			{
				while (tempLenght > 0)
				{
					size_t temp = fwrite(tmpPtr, 1, tempLenght, p);
					tmpPtr += temp;
					tempLenght = tempLenght - temp;
				}
				fclose(p);
			}
		}

		SDL_PauseAudio(0);
		//while (audio_len > 0){
		//	//Wait until finish
		//	SDL_Delay(1);
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
	//swr_free(&au_convert_ctx);
	SDL_CloseAudio();
	SDL_Quit();
	av_free(out_buffer);
	avcodec_close(pOutputCodecCtx);

	return iRet;
}
void CLS_DlgStreamPusher::OnBnClickedBtnDeviceAudioTest()
{
	m_pStreamInfo->m_itest_start = 1;
	//开启audio测试线程
	m_pStreamInfo->m_show_mode = SHOW_MODE_WAVES;
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
	m_pStreamInfo->m_screen_surface = NULL;
	m_pStreamInfo->m_xleft = 0;
	m_pStreamInfo->m_ytop = 0;
	m_pStreamInfo->m_width = 0;
	m_pStreamInfo->m_height = 0;
	m_pStreamInfo->m_pAudioStream = NULL;
	m_pStreamInfo->m_pVideoStream = NULL;
	m_pStreamInfo->m_pAudioFrame = NULL;
	m_pStreamInfo->m_pVideoFrame = NULL;
	m_pStreamInfo->m_swr_ctx = NULL;
	m_pStreamInfo->m_audio_buf = NULL;
	m_pStreamInfo->m_audio_buf_size = 0;
	m_pStreamInfo->m_audio_buf_index = 0;
	m_pStreamInfo->m_aduio_pkt_size = 0;
	m_pStreamInfo->m_sample_array_index = 0;
	m_pStreamInfo->m_abort_request = 0;
	m_pStreamInfo->m_refresh = 0;
	m_pStreamInfo->m_show_mode = SHOW_MODE_NONE;
	m_pStreamInfo->m_refresh_tid = NULL;
	m_pStreamInfo->m_itest_start = 0;

	m_pThreadEvent = NULL;
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

void CLS_DlgStreamPusher::FillDisplayRect()
{
	struct_stream_info* streamInfo = GetStreamStrcInfo();
	if (NULL == streamInfo){
		TRACE("NULL == streamInfo");
		return;
	}

	streamInfo->m_screen_surface = SDL_GetWindowSurface(streamInfo->m_show_screen);
	if (NULL == streamInfo->m_screen_surface){
		TRACE("NULL == streamInfo->m_screen_surface");
		return;
	}

	//背景色
	int bgcolor = SDL_MapRGB(streamInfo->m_screen_surface->format, 0x00, 0x00, 0x00);
	fill_rec(streamInfo->m_screen_surface, m_pStreamInfo->m_xleft, m_pStreamInfo->m_ytop, m_pStreamInfo->m_width, m_pStreamInfo->m_height,bgcolor);

	if (SDL_UpdateWindowSurface(streamInfo->m_show_screen) != 0){
		TRACE("SDL_UpdateWindowSurface ERR");
	}
}

int CLS_DlgStreamPusher::SynAudio(struct_stream_info* _pstrct_streaminfo, int _inb_samples)
{
	int iRet = -1;
	if (NULL == _pstrct_streaminfo){
		TRACE("NULL == _pstrct_streaminfo\n");
		return iRet;
	}
	int wanted_nb_samples = _inb_samples;

	//if (((_pstrct_streaminfo->av_sync_type == AV_SYNC_VIDEO_MASTER && _pstrct_streaminfo->video_st) ||
	//	_pstrct_streaminfo->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {
	//	double diff, avg_diff;
	//	int min_nb_samples, max_nb_samples;

	//	diff = get_audio_clock(is) - get_master_clock(is);

	//	if (diff < AV_NOSYNC_THRESHOLD) {
	//		is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
	//		if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
	//			/* not enough measures to have a correct estimate */
	//			is->audio_diff_avg_count++;
	//		}
	//		else {
	//			/* estimate the A-V difference */
	//			avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);

	//			if (fabs(avg_diff) >= is->audio_diff_threshold) {
	//				wanted_nb_samples = nb_samples + (int)(diff * is->audio_src.freq);
	//				min_nb_samples = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
	//				max_nb_samples = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
	//				wanted_nb_samples = FFMIN(FFMAX(wanted_nb_samples, min_nb_samples), max_nb_samples);
	//			}
	//			av_dlog(NULL, "diff=%f adiff=%f sample_diff=%d apts=%0.3f vpts=%0.3f %f\n",
	//				diff, avg_diff, wanted_nb_samples - nb_samples,
	//				is->audio_clock, is->video_clock, is->audio_diff_threshold);
	//		}
	//	}
	//	else {
	//		/* too big difference : may be initial PTS errors, so
	//		reset A-V filter */
	//		is->audio_diff_avg_count = 0;
	//		is->audio_diff_cum = 0;
	//	}
	//}

	return wanted_nb_samples;
}

void CLS_DlgStreamPusher::event_loop(struct_stream_info *_pstrct_streaminfo)
{
	if (NULL == _pstrct_streaminfo){
		TRACE("NULL == _pstrct_streaminfo");
		return;
	}
	
	SDL_Event event;
	double incr, pos, frac;
	for (;;) {

		double x;
		//判断退出
		if (_pstrct_streaminfo->m_abort_request){
			break;
		}

		SDL_WaitEvent(&event);
		switch (event.type) {
		case FF_REFRESH_EVENT:
			screen_refresh(event.user.data1);
			_pstrct_streaminfo->m_refresh = 0;
			break;
		default:
			break;
		}
	}
}

void CLS_DlgStreamPusher::screen_refresh(void *opaque)
{
	struct_stream_info* strct_streaminfo = (struct_stream_info*)opaque;
	if (NULL == strct_streaminfo){
		TRACE("NULL == strct_streaminfo");
		return;
	}

	screen_display(strct_streaminfo);
}

void CLS_DlgStreamPusher::screen_display(struct_stream_info *_pstrct_streaminfo)
{
	if (_pstrct_streaminfo->m_pAudioFrame && _pstrct_streaminfo->m_show_mode != SHOW_MODE_VIDEO){
		audio_display(_pstrct_streaminfo);
	}
	else{
		video_display(_pstrct_streaminfo);
	}
}

void CLS_DlgStreamPusher::audio_display(struct_stream_info *_pstrct_streaminfo)
{
	if (audio_callback_time == 0){
		TRACE("audio_callback_time == 0");
		return;
	}
	int i, i_start, x, y1, y, ys, delay, n, nb_display_channels;
	int ch, channels, h, h2, bgcolor, fgcolor;
	int16_t time_diff;
	int rdft_bits, nb_freq;

	for (rdft_bits = 1; (1 << rdft_bits) < 2 * _pstrct_streaminfo->m_height; rdft_bits++)
		;
	nb_freq = 1 << (rdft_bits - 1);

	/* compute display index : center on currently output samples */
	channels = _pstrct_streaminfo->m_audio_tgt.channels;
	nb_display_channels = channels;
	if (!_pstrct_streaminfo->m_paused) {
		int data_used = _pstrct_streaminfo->m_show_mode == SHOW_MODE_WAVES ? _pstrct_streaminfo->m_width : (2 * nb_freq);
		n = 2 * channels;
		delay = _pstrct_streaminfo->m_audio_write_buf_size;
		delay /= n;

		/* to be more precise, we take into account the time spent since
		the last buffer computation */
		if (audio_callback_time) {
			time_diff = av_gettime() - audio_callback_time;
			delay -= (time_diff * _pstrct_streaminfo->m_audio_tgt.freq) / 1000000;
		}

		delay += 2 * data_used;
		if (delay < data_used)
			delay = data_used;

		i_start = x = compute_mod(_pstrct_streaminfo->m_sample_array_index - delay * channels, SAMPLE_ARRAY_SIZE);
		if (_pstrct_streaminfo->m_show_mode == SHOW_MODE_WAVES) {
			h = INT_MIN;
			for (i = 0; i < 1000; i += channels) {
				int idx = (SAMPLE_ARRAY_SIZE + x - i) % SAMPLE_ARRAY_SIZE;
				int a = _pstrct_streaminfo->m_sample_array[idx];
				int b = _pstrct_streaminfo->m_sample_array[(idx + 4 * channels) % SAMPLE_ARRAY_SIZE];
				int c = _pstrct_streaminfo->m_sample_array[(idx + 5 * channels) % SAMPLE_ARRAY_SIZE];
				int d = _pstrct_streaminfo->m_sample_array[(idx + 9 * channels) % SAMPLE_ARRAY_SIZE];
				int score = a - d;
				if (h < score && (b ^ c) < 0) {
					h = score;
					i_start = idx;
				}
			}
		}

		_pstrct_streaminfo->m_audio_last_i_start = i_start;
	}


	//背景色
	bgcolor = SDL_MapRGB(_pstrct_streaminfo->m_screen_surface->format, 0x00, 0x00, 0x00);
	if (_pstrct_streaminfo->m_show_mode == SHOW_MODE_WAVES) {
		SDL_Rect sdl_rect;
		_pstrct_streaminfo->m_xleft = 0;
		_pstrct_streaminfo->m_ytop = 0;
		sdl_rect.x = _pstrct_streaminfo->m_xleft;
		sdl_rect.y = _pstrct_streaminfo->m_ytop;
		sdl_rect.w = _pstrct_streaminfo->m_width;
		sdl_rect.h = _pstrct_streaminfo->m_height;

		fill_rec(_pstrct_streaminfo->m_screen_surface, _pstrct_streaminfo->m_xleft, _pstrct_streaminfo->m_ytop, _pstrct_streaminfo->m_width, _pstrct_streaminfo->m_height,bgcolor);

		fgcolor = SDL_MapRGB(_pstrct_streaminfo->m_screen_surface->format, 0xff, 0xff, 0xff);

		/* total height for one channel */
		h = _pstrct_streaminfo->m_height / nb_display_channels;
		/* graph height / 2 */
		h2 = (h * 9) / 20;
		for (ch = 0; ch < nb_display_channels; ch++) {
			i = i_start + ch;
			y1 = _pstrct_streaminfo->m_ytop + ch * h + (h / 2); /* position of center line */
			for (x = 0; x < _pstrct_streaminfo->m_width; x++) {
				y = (_pstrct_streaminfo->m_sample_array[i] * h2) >> 15;
				if (y < 0) {
					y = -y;
					ys = y1 - y;
				}
				else {
					ys = y1;
				}

				fill_rec(_pstrct_streaminfo->m_screen_surface, _pstrct_streaminfo->m_xleft + x, ys, 1, y, fgcolor);

				i += channels;
				if (i >= SAMPLE_ARRAY_SIZE)
					i -= SAMPLE_ARRAY_SIZE;
			}
		}

		fgcolor = SDL_MapRGB(_pstrct_streaminfo->m_screen_surface->format, 0x00, 0x00, 0xff);

		for (ch = 1; ch < nb_display_channels; ch++) {
			y = _pstrct_streaminfo->m_ytop + ch * h;

			fill_rec(_pstrct_streaminfo->m_screen_surface, _pstrct_streaminfo->m_xleft, y, _pstrct_streaminfo->m_width, 1, fgcolor);
		}
		if (SDL_UpdateWindowSurface(_pstrct_streaminfo->m_show_screen) != 0){
			TRACE("SDL_UpdateWindowSurface ERR");
		}
		//SDL_CreateRGBSurface();
		SDL_UpdateTexture(_pstrct_streaminfo->m_sdlTexture, NULL, _pstrct_streaminfo->m_screen_surface->pixels, _pstrct_streaminfo->m_screen_surface->pitch);
		SDL_RenderClear(_pstrct_streaminfo->m_sdlRenderer);
		SDL_RenderCopy(_pstrct_streaminfo->m_sdlRenderer, _pstrct_streaminfo->m_sdlTexture, &sdl_rect, &sdl_rect);
		SDL_RenderPresent(_pstrct_streaminfo->m_sdlRenderer);
	}
}

void CLS_DlgStreamPusher::video_display(struct_stream_info *_pstrct_streaminfo)
{

}

void CLS_DlgStreamPusher::fill_rec(SDL_Surface *screen,
	int x, int y, int w, int h, int color)
{
	if (NULL == screen){
		TRACE("NULL == screen");
		return;
	}

	//区域填充
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_FillRect(screen, &rect, color);
}