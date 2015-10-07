
// FFMPEG_SDL_MFC_PLAYER_1.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFFMPEG_SDL_MFC_PLAYER_1App:
// See FFMPEG_SDL_MFC_PLAYER_1.cpp for the implementation of this class
//

class CFFMPEG_SDL_MFC_PLAYER_1App : public CWinAppEx
{
public:
	CFFMPEG_SDL_MFC_PLAYER_1App();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFFMPEG_SDL_MFC_PLAYER_1App theApp;