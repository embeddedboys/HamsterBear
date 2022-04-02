/*===================================================================*/
/*                                                                   */
/*  InfoNES_System_Wce.cpp : WindowsCE specific File                 */
/*                                                                   */
/*  2001/12/08  Mopeo                                                */
/*                                                                   */
/*===================================================================*/

#define VK_CASSIOPEIA_SLIDER	0x86
#define VK_CASSIOPEIA_ESC		0xC1
#define VK_CASSIOPEIA_REC		0xC2
#define VK_CASSIOPEIA_B1		0xC3
#define VK_CASSIOPEIA_B2		0xC4
#define VK_CASSIOPEIA_B3		0xC5
#define VK_CASSIOPEIA_LEFT		37
#define VK_CASSIOPEIA_UP		38
#define VK_CASSIOPEIA_RIGHT		39
#define VK_CASSIOPEIA_DOWN		40

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include <windows.h>
#include <aygshell.h>
#include <Commctrl.h>
#include <Commdlg.h>
#include <stdlib.h>
#include <stdio.h>

#include "gx.h"
#pragma comment(lib, "gx.lib")

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"
#include "resource.h"

/*-------------------------------------------------------------------*/
/*  ROM image file information                                       */
/*-------------------------------------------------------------------*/

char szRomName[ 256 ];
char szSaveName[ 256 ];
int nSRAM_SaveFlag;

/*-------------------------------------------------------------------*/
/*  Global Variables                                                 */
/*-------------------------------------------------------------------*/

TCHAR szAppName[] = TEXT("InfoNES Pocket PC Application");
TCHAR szTitle[]   = TEXT("InfoNES");

HWND m_hwnd;
HINSTANCE hinst;
GXDisplayProperties g_gxdp;
HANDLE hthread = NULL;
DWORD threadid;
BOOL threadrun;

static SHACTIVATEINFO s_sai;

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

LRESULT CALLBACK MainWndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
int LoadSRAM();
int SaveSRAM();

/* Palette data */
WORD NesPalette[ 64 ];

DWORD NesPalette24[64] = { /* Matthew Conte's Palette */
	0x808080, 0x0000bb, 0x3700bf, 0x8400a6,
	0xbb006a, 0xb7001e, 0xb30000, 0x912600,
	0x7b2b00, 0x003e00, 0x00480d, 0x003c22,
	0x002f66, 0x000000, 0x050505, 0x050505,

	0xc8c8c8, 0x0059ff, 0x443cff, 0xb733cc,
	0xff33aa, 0xff375e, 0xff371a, 0xd54b00,
	0xc46200, 0x3c7b00, 0x1e8415, 0x009566,
	0x0084c4, 0x111111, 0x090909, 0x090909,

	0xffffff, 0x0095ff, 0x6f84ff, 0xd56fff,
	0xff77cc, 0xff6f99, 0xff7b59, 0xff915f,
	0xffa233, 0xa6bf00, 0x51d96a, 0x4dd5ae,
	0x00d9ff, 0x666666, 0x0d0d0d, 0x0d0d0d,

	0xffffff, 0x84bfff, 0xbbbbff, 0xd0bbff,
	0xffbfea, 0xffbfcc, 0xffc4b7, 0xffccae,
	0xffd9a2, 0xcce199, 0xaeeeb7, 0xaaf7ee,
	0xb3eeff, 0xdddddd, 0x111111, 0x111111
};

/*===================================================================*/
/*                                                                   */
/*                WinMain() : Application main                       */
/*                                                                   */
/*===================================================================*/

/* Application main */
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPWSTR lpCmdLine,
				   int CmdShow)
{
	MSG msg;
	WNDCLASS wc;
	OPENFILENAME ofn;
	TCHAR szFileName[256];
	HWND hwnd_infones = NULL;
	int i, y, x;
	BYTE r, g, b;
	DWORD temp;
	unsigned short *pusLine, *pusDest;

	/* すでに実行されているかチェック */
	hwnd_infones = FindWindow(szAppName, szTitle);
	if(hwnd_infones){
		SetForegroundWindow(hwnd_infones);
		return 0;
	}

	hinst = hInstance;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)MainWndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = 0;
	wc.hInstance = hInstance;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;
	if(!RegisterClass(&wc)){
		return FALSE;
	}

	m_hwnd = CreateWindow(szAppName,
					szTitle,
					WS_VISIBLE,
					0,
					0,
					GetSystemMetrics(SM_CXSCREEN),
					GetSystemMetrics(SM_CYSCREEN),
					NULL, NULL, hInstance, NULL);
	if(!m_hwnd){
		return FALSE;
	}

	/* ファイルダイアログを開く */
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("NES files (*.nes)\0*.nes\0All files (*.*)\0*.*\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = 256;
	ofn.Flags = OFN_EXPLORER;
	if(!GetOpenFileName(&ofn)){
		MessageBox(m_hwnd, TEXT("ファイルを選択してください"), szTitle, MB_ICONEXCLAMATION);
		return FALSE;
	}
	wcstombs(szRomName, szFileName, 256); /* UnicodeからANSIへ */

	/* NESファイルを開く */
	if(InfoNES_Load(szRomName) != 0){
		MessageBox(m_hwnd, TEXT("未対応のファイルです"), szTitle, MB_ICONEXCLAMATION);
		return FALSE;
	}
	LoadSRAM();

	ShowWindow(m_hwnd, CmdShow);
	UpdateWindow(m_hwnd);

	/* GAPI初期化 */
	if(!GXOpenDisplay(m_hwnd, GX_FULLSCREEN)){
		MessageBox(m_hwnd, TEXT("GXOpenDisplayエラー"), szTitle, MB_ICONEXCLAMATION);
		return FALSE;
	}
	g_gxdp = GXGetDisplayProperties();
	if(g_gxdp.cxWidth != 240 || g_gxdp.cyHeight != 320){
		MessageBox(m_hwnd, TEXT("使用できない画面モードです"), szTitle, MB_ICONEXCLAMATION);
		return FALSE;
	}
	if(g_gxdp.cBPP != 16 || !(g_gxdp.ffFormat & kfDirect565)){
		MessageBox(m_hwnd, TEXT("使用できない画面モードです"), szTitle, MB_ICONEXCLAMATION);
		return FALSE;
	}
	if (!GXOpenInput()){ /* ハードウェアキーを独占するためにロードだけしておく */
		MessageBox(m_hwnd, TEXT("GXOpenInputエラー"), szTitle, MB_ICONEXCLAMATION);
		return FALSE;
	}

	/* 画面のクリア */
	pusLine = (unsigned short *)GXBeginDraw();
	for(y = 0; y < 320; y++){
		pusDest = pusLine;
		for(x = 0; x < 240; x++){
			*pusDest = 0xffff;
			pusDest += g_gxdp.cbxPitch >> 1;
		}
		pusLine += g_gxdp.cbyPitch >> 1;
	}
	GXEndDraw();

	/* パレットのセット */
	for(i = 0; i < 64; i++){
		temp = NesPalette24[i];
		r = (BYTE)(temp >> 16);
		g = (BYTE)(((WORD)(temp)) >> 8);
		b = (BYTE)(temp);
		NesPalette[i] = (((WORD)r << 8) & 0xf800) | 
		(((WORD)g << 3) & 0x0fe0) | 
		((WORD)b >> 3); /* 5-6-5へ */
	}

	/* エミュレーションの開始 */
	threadrun = TRUE;
	hthread = CreateThread((LPSECURITY_ATTRIBUTES)NULL,
		(DWORD)0, (LPTHREAD_START_ROUTINE)InfoNES_Main, (LPVOID)NULL,
		(DWORD)0, &threadid);

	/* メッセージループ */
	while(GetMessage(&msg, NULL, 0, 0) == TRUE){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GXCloseDisplay();
	GXCloseInput();

	return msg.wParam;
}

/*===================================================================*/
/*                                                                   */
/*            MainWndproc() : ウィンドウプロシージャ関数             */
/*                                                                   */
/*===================================================================*/

LRESULT CALLBACK MainWndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	POINT  pt;
	HMENU  hMenu;
	HMENU  hSubMenu;

	switch(msg){
		case WM_COMMAND:
			switch(LOWORD(wp)){
				case IDM_MAIN_END:
					TerminateThread(hthread, (DWORD)0);
					CloseHandle(hthread);
					hthread = NULL;
					SaveSRAM();
					PostQuitMessage(0);
					break;
				case IDM_MAIN_RESUME:
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
				case IDM_MAIN_RESET:
					TerminateThread(hthread, (DWORD)0);
					CloseHandle(hthread);
					hthread = NULL;
					SaveSRAM();
					if(InfoNES_Load(szRomName) == 0){
						LoadSRAM();
						threadrun = TRUE;
						hthread = CreateThread((LPSECURITY_ATTRIBUTES)NULL,
							(DWORD)0, (LPTHREAD_START_ROUTINE)InfoNES_Main, (LPVOID)NULL,
							(DWORD)0, &threadid);
					}
					break;
				case IDM_MAIN_FRAME_0:
					FrameCnt = 0;
					FrameSkip = 0;
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
				case IDM_MAIN_FRAME_1:
					FrameCnt = 0;
					FrameSkip = 1;
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
				case IDM_MAIN_FRAME_2:
					FrameCnt = 0;
					FrameSkip = 2;
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
				case IDM_MAIN_FRAME_3:
					FrameCnt = 0;
					FrameSkip = 3;
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
				case IDM_MAIN_FRAME_4:
					FrameCnt = 0;
					FrameSkip = 4;
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
				case IDM_MAIN_FRAME_5:
					FrameCnt = 0;
					FrameSkip = 5;
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
				case IDM_MAIN_FRAME_8:
					FrameCnt = 0;
					FrameSkip = 8;
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
				case IDM_MAIN_FRAME_10:
					FrameCnt = 0;
					FrameSkip = 10;
					threadrun = TRUE;
					ResumeThread(hthread);
					break;
			}
			break;
		case WM_INITMENUPOPUP:
			CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_0, MF_UNCHECKED);
			CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_1, MF_UNCHECKED);
			CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_2, MF_UNCHECKED);
			CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_3, MF_UNCHECKED);
			CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_4, MF_UNCHECKED);
			CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_5, MF_UNCHECKED);
			CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_8, MF_UNCHECKED);
			CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_10, MF_UNCHECKED);
			switch(FrameSkip){
				case 0:
					CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_0, MF_CHECKED);
					break;
				case 1:
					CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_1, MF_CHECKED);
					break;
				case 2:
					CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_2, MF_CHECKED);
					break;
				case 3:
					CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_3, MF_CHECKED);
					break;
				case 4:
					CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_4, MF_CHECKED);
					break;
				case 5:
					CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_5, MF_CHECKED);
					break;
				case 8:
					CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_8, MF_CHECKED);
					break;
				case 10:
					CheckMenuItem(HMENU(wp), IDM_MAIN_FRAME_10, MF_CHECKED);
					break;
			}
			break;
		case WM_LBUTTONDOWN:
			if(threadrun == TRUE){
				threadrun = FALSE;
				SuspendThread(hthread);
			}
			pt.x = LOWORD(lp);
			pt.y = HIWORD(lp);
			ClientToScreen(hwnd, &pt);
			hMenu = LoadMenu(hinst, MAKEINTRESOURCE(IDR_MENU));
			hSubMenu = GetSubMenu (hMenu, 0);
			TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, 
				pt.x, pt.y, 0, hwnd, NULL);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SETTINGCHANGE:
			SHHandleWMSettingChange(hwnd, wp, lp, &s_sai);
     		break;
		case WM_KILLFOCUS:
			GXSuspend();
			break;
		case WM_SETFOCUS:
			GXResume();
			break;
		default:
			return DefWindowProc(hwnd, msg, wp, lp);
	}

	return 0;
}

/*===================================================================*/
/*                                                                   */
/*           LoadSRAM() : Load a SRAM                                */
/*                                                                   */
/*===================================================================*/
int LoadSRAM()
{
/*
 *  Load a SRAM
 *
 *  Return values
 *     0 : Normally
 *    -1 : SRAM data couldn't be read
 */

	FILE *fp;
	unsigned char pSrcBuf[ SRAM_SIZE ];
	unsigned char chData;
	unsigned char chTag;
	int nRunLen;
	int nDecoded;
	int nDecLen;
	int nIdx;

	// It doesn't need to save it
	nSRAM_SaveFlag = 0;

	// It is finished if the ROM doesn't have SRAM
	if ( !ROM_SRAM )
		return 0;

	// There is necessity to save it
	nSRAM_SaveFlag = 1;

	// The preparation of the SRAM file name
	strcpy( szSaveName, szRomName );
	strcpy( strrchr( szSaveName, '.' ) + 1, "srm" );

	/*-------------------------------------------------------------------*/
	/*  Read a SRAM data                                                 */
	/*-------------------------------------------------------------------*/

	// Open SRAM file
	fp = fopen( szSaveName, "rb" );
	if ( fp == NULL )
		return -1;

	// Read SRAM data
	fread( pSrcBuf, SRAM_SIZE, 1, fp );

	// Close SRAM file
	fclose( fp );

	/*-------------------------------------------------------------------*/
	/*  Extract a SRAM data                                              */
	/*-------------------------------------------------------------------*/

	nDecoded = 0;
	nDecLen = 0;

	chTag = pSrcBuf[ nDecoded++ ];

	while ( nDecLen < 8192 )
	{
		chData = pSrcBuf[ nDecoded++ ];

		if ( chData == chTag )
		{
			chData = pSrcBuf[ nDecoded++ ];
			nRunLen = pSrcBuf[ nDecoded++ ];
			for ( nIdx = 0; nIdx < nRunLen + 1; ++nIdx )
			{
				SRAM[ nDecLen++ ] = chData;
			}
		}
		else
		{
			SRAM[ nDecLen++ ] = chData;
		}
	}

	// Successful
	return 0;
}

/*===================================================================*/
/*                                                                   */
/*           SaveSRAM() : Save a SRAM                                */
/*                                                                   */
/*===================================================================*/
int SaveSRAM()
{
/*
 *  Save a SRAM
 *
 *  Return values
 *     0 : Normally
 *    -1 : SRAM data couldn't be written
 */
	FILE *fp;
	int nUsedTable[ 256 ];
	unsigned char chData;
	unsigned char chPrevData;
	unsigned char chTag;
	int nIdx;
	int nEncoded;
	int nEncLen;
	int nRunLen;
	unsigned char pDstBuf[ SRAM_SIZE ];

	if ( !nSRAM_SaveFlag )
		return 0;  // It doesn't need to save it

	/*-------------------------------------------------------------------*/
	/*  Compress a SRAM data                                             */
	/*-------------------------------------------------------------------*/

	memset( nUsedTable, 0, sizeof nUsedTable );

	for ( nIdx = 0; nIdx < SRAM_SIZE; ++nIdx )
	{
		++nUsedTable[ SRAM[ nIdx++ ] ];
	}
	for ( nIdx = 1, chTag = 0; nIdx < 256; ++nIdx )
	{
		if ( nUsedTable[ nIdx ] < nUsedTable[ chTag ] )
			chTag = nIdx;
	}

	nEncoded = 0;
	nEncLen = 0;
	nRunLen = 1;

	pDstBuf[ nEncLen++ ] = chTag;

	chPrevData = SRAM[ nEncoded++ ];

	while ( nEncoded < SRAM_SIZE && nEncLen < SRAM_SIZE - 133 )
	{
		chData = SRAM[ nEncoded++ ];

		if ( chPrevData == chData && nRunLen < 256 )
			++nRunLen;
		else
		{
			if ( nRunLen >= 4 || chPrevData == chTag )
			{
				pDstBuf[ nEncLen++ ] = chTag;
				pDstBuf[ nEncLen++ ] = chPrevData;
				pDstBuf[ nEncLen++ ] = nRunLen - 1;
			}
			else
			{
			for ( nIdx = 0; nIdx < nRunLen; ++nIdx )
				pDstBuf[ nEncLen++ ] = chPrevData;
			}

			chPrevData = chData;
			nRunLen = 1;
		}

	}
	if ( nRunLen >= 4 || chPrevData == chTag )
	{
		pDstBuf[ nEncLen++ ] = chTag;
		pDstBuf[ nEncLen++ ] = chPrevData;
		pDstBuf[ nEncLen++ ] = nRunLen - 1;
	}
	else
	{
		for ( nIdx = 0; nIdx < nRunLen; ++nIdx )
			pDstBuf[ nEncLen++ ] = chPrevData;
		}

	/*-------------------------------------------------------------------*/
	/*  Write a SRAM data                                                */
	/*-------------------------------------------------------------------*/

	// Open SRAM file
	fp = fopen( szSaveName, "wb" );
	if ( fp == NULL )
		return -1;

	// Write SRAM data
	fwrite( pDstBuf, nEncLen, 1, fp );

	// Close SRAM file
	fclose( fp );

	// Successful
	return 0;
}


/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Menu() : Menu screen                     */
/*                                                                   */
/*===================================================================*/
int InfoNES_Menu()
{
/*
 *  Menu screen
 *
 *  Return values
 *     0 : Normally
 *    -1 : Exit InfoNES
 */

  /* Nothing to do here */
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*               InfoNES_ReadRom() : Read ROM image file             */
/*                                                                   */
/*===================================================================*/
int InfoNES_ReadRom( const char *pszFileName )
{
/*
 *  Read ROM image file
 *
 *  Parameters
 *    const char *pszFileName          (Read)
 *
 *  Return values
 *     0 : Normally
 *    -1 : Error
 */

	FILE *fp;

	/* Open ROM file */
	fp = fopen( pszFileName, "rb" );
	if ( fp == NULL )
	return -1;

	/* Read ROM Header */
	fread( &NesHeader, sizeof NesHeader, 1, fp );
	if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
	{
		/* not .nes file */
		fclose( fp );
		return -1;
	}

	/* Clear SRAM */
	memset( SRAM, 0, SRAM_SIZE );

	/* If trainer presents Read Triner at 0x7000-0x71ff */
	if ( NesHeader.byInfo1 & 4 )
	{
		fread( &SRAM[ 0x1000 ], 512, 1, fp );
	}

	/* Allocate Memory for ROM Image */
	ROM = (BYTE *)malloc( NesHeader.byRomSize * 0x4000 );

	/* Read ROM Image */
	fread( ROM, 0x4000, NesHeader.byRomSize, fp );

	if ( NesHeader.byVRomSize > 0 )
	{
		/* Allocate Memory for VROM Image */
		VROM = (BYTE *)malloc( NesHeader.byVRomSize * 0x2000 );

		/* Read VROM Image */
		fread( VROM, 0x2000, NesHeader.byVRomSize, fp );
	}

	/* File close */
	fclose( fp );

	/* Successful */
	return 0;
}

/*===================================================================*/
/*                                                                   */
/*           InfoNES_ReleaseRom() : Release a memory for ROM         */
/*                                                                   */
/*===================================================================*/
void InfoNES_ReleaseRom()
{
/*
 *  Release a memory for ROM
 *
 */

	if ( ROM )
	{
		free( ROM );
		ROM = NULL;
	}

	if ( VROM )
	{
		free( VROM );
		VROM = NULL;
	}
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemoryCopy() : memcpy                         */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemoryCopy( void *dest, const void *src, int count )
{
/*
 *  memcpy
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the copied block's destination
 *
 *    const void *src                  (Read)
 *      Points to the starting address of the block of memory to copy
 *
 *    int count                        (Read)
 *      Specifies the size, in bytes, of the block of memory to copy
 *
 *  Return values
 *    Pointer of destination
 */

	memcpy( dest, src, count );
	return dest;
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemorySet() : memset                          */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemorySet( void *dest, int c, int count )
{
/*
 *  memset
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the block of memory to fill
 *
 *    int c                            (Read)
 *      Specifies the byte value with which to fill the memory block
 *
 *    int count                        (Read)
 *      Specifies the size, in bytes, of the block of memory to fill
 *
 *  Return values
 *    Pointer of destination
 */

	memset( dest, c, count);  
	return dest;
}

/*===================================================================*/
/*                                                                   */
/*      InfoNES_LoadFrame() :                                        */
/*           Transfer the contents of work frame on the screen       */
/*                                                                   */
/*===================================================================*/
void InfoNES_LoadFrame()
{
/*
 *  Transfer the contents of work frame on the screen
 *
 */
	unsigned short *pusLine, *pusDest, *pusSrc;
	int x, y, xp, yp;

	pusSrc = WorkFrame;
	xp = g_gxdp.cbxPitch >> 1;
	yp = g_gxdp.cbyPitch >> 1;
	pusLine = (unsigned short *)GXBeginDraw();

	for(y = 0; y < NES_DISP_HEIGHT; y++){
		pusDest = pusLine;
		for(x = 0; x < 240; x++){
			*pusDest = *pusSrc;
			pusDest += xp;
			pusSrc++;
		}
		pusSrc += 16;
		pusLine += yp;
	}
	
	GXEndDraw();
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_PadState() : Get a joypad state               */
/*                                                                   */
/*===================================================================*/
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
/*
 *  Get a joypad state
 *
 *  Parameters
 *    DWORD *pdwPad1                   (Write)
 *      Joypad 1 State
 *
 *    DWORD *pdwPad2                   (Write)
 *      Joypad 2 State
 *
 *    DWORD *pdwSystem                 (Write)
 *      Input for InfoNES
 *
 */

	/* Transfer joypad state */
	*pdwPad1 = (GetAsyncKeyState(VK_CASSIOPEIA_B3) < 0 ) | /* A */
			((GetAsyncKeyState(VK_CASSIOPEIA_B2) < 0 ) << 1 ) | /* B */
			((GetAsyncKeyState(VK_CASSIOPEIA_B1) < 0 ) << 2 ) | /* Select */
			((GetAsyncKeyState(VK_CASSIOPEIA_REC)  < 0 ) << 3 ) | /* Start */
			((GetAsyncKeyState(VK_CASSIOPEIA_UP) < 0 ) << 4 ) | /* Up */
			((GetAsyncKeyState(VK_CASSIOPEIA_DOWN) < 0 ) << 5 ) | /* Down */
			((GetAsyncKeyState(VK_CASSIOPEIA_LEFT) < 0 ) << 6 ) | /* Left */
			((GetAsyncKeyState(VK_CASSIOPEIA_RIGHT) < 0 ) << 7 ); /* Right */

	*pdwPad2   = 0;
	*pdwSystem = 0;
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundInit() : Sound Emulation Initialize           */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundInit( void ) 
{
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundOpen() : Sound Open                           */
/*                                                                   */
/*===================================================================*/
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate ) 
{
  /* Successful */
  return 1;
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundClose() : Sound Close                         */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundClose( void ) 
{
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_SoundOutput4() : Sound Output 4 Waves          */           
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundOutput4( int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4 ) 
{
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_Wait() : Wait Emulation if required            */
/*                                                                   */
/*===================================================================*/
void InfoNES_Wait()
{
}

