#ifndef _SCREENCAPTURE9_H_
#define _SCREENCAPTURE9_H_

#include <d3d9.h>

class ScreenCapture9
{
private:
	//vars
	IDirect3D9*			g_pD3D = NULL;
	IDirect3DDevice9*	g_pd3dDevice = NULL;
	IDirect3DSurface9*	g_pSurface = NULL;
	HWND				ghWnd = NULL;
	RECT				gScreenRect = { 0,0,0,0 };
	UINT				gPitch = 0;

public:
	//methods
	ScreenCapture9();
	~ScreenCapture9();
	HRESULT	InitD3D(HWND hWnd = NULL);
	UINT GetPitch();
	UINT GetHeight();
	UINT GetWidth();
	HRESULT GrabImage(void* bitmap);
	HRESULT Reset();
	void Cleanup();
};


#endif // !_SCREENCAPTURE9_H_