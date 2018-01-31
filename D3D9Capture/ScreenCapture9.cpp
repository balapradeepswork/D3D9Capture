#include "stdafx.h"
#include "ScreenCapture9.h"


ScreenCapture9::ScreenCapture9()
{
}


ScreenCapture9::~ScreenCapture9()
{
}

HRESULT	ScreenCapture9::InitD3D(HWND hWnd)
{
	D3DDISPLAYMODE	ddm;
	D3DPRESENT_PARAMETERS	d3dpp;

	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		printf_s("Unable to Create Direct3D\n");
		return E_FAIL;
	}

	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &ddm)))
	{
		printf_s("Unable to Get Adapter Display Mode\n");
		return E_FAIL;
	}

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.Windowed = true;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat = ddm.Format;
	d3dpp.BackBufferHeight = gScreenRect.bottom = ddm.Height;
	d3dpp.BackBufferWidth = gScreenRect.right = ddm.Width;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		printf_s("Unable to Create Device\n");
		return E_FAIL;
	}

	ghWnd = hWnd;

	if (FAILED(g_pd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &g_pSurface, NULL)))
	{
		printf_s("Unable to Create Surface\n");
		return E_FAIL;
	}

	D3DLOCKED_RECT	lockedRect;

	if (FAILED(g_pSurface->LockRect(&lockedRect, NULL, 0)))					// compute the required buffer size
	{
		printf_s("Unable to Lock Surface\n");
		return E_FAIL;
	}
	gPitch = lockedRect.Pitch;
	if (FAILED(g_pSurface->UnlockRect()))
	{
		printf_s("Unable to Unlock Surface\n");
		return E_FAIL;
	}

	return S_OK;
}

UINT ScreenCapture9::GetPitch()
{
	return gPitch;
}

UINT ScreenCapture9::GetHeight()
{
	return gScreenRect.bottom;
}

UINT ScreenCapture9::GetWidth()
{
	return gScreenRect.right;
}

HRESULT ScreenCapture9::GrabImage(void* pBits)
{
	if (FAILED(g_pd3dDevice->GetFrontBufferData(0, g_pSurface)))
	{
		printf_s("Unable to get Buffer Surface Data\n");
		return E_FAIL;
	}

	D3DLOCKED_RECT	lockedRect;
	if (FAILED(g_pSurface->LockRect(&lockedRect, NULL, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY)))
	{
		printf_s("Unable to Lock Front Buffer Surface\n");
		return E_FAIL;
	}

	errno_t err;
	err = memcpy_s((BYTE*)pBits, lockedRect.Pitch*gScreenRect.bottom, (BYTE*)lockedRect.pBits, lockedRect.Pitch*gScreenRect.bottom);
	if (err)
	{
		printf_s("Error executing memcpy_s.\n");
	}
	
	if (FAILED(g_pSurface->UnlockRect()))
	{
		printf_s("Unable to Unlock Front Buffer Surface\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT ScreenCapture9::Reset()
{
	D3DDISPLAYMODE	ddm;
	D3DPRESENT_PARAMETERS	d3dpp;

	if (g_pSurface)															//Release the Surface - we need to get the latest surface
	{
		g_pSurface->Release();
		g_pSurface = NULL;
	}

	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &ddm)))	//User might have changed the mode - Get it afresh
	{
		printf_s("Unable to Get Adapter Display Mode");
		return E_FAIL;
	}

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.Windowed = true;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat = ddm.Format;
	d3dpp.BackBufferHeight = gScreenRect.bottom = ddm.Height;
	d3dpp.BackBufferWidth = gScreenRect.right = ddm.Width;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = ghWnd;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(g_pd3dDevice->Reset(&d3dpp)))
	{
		printf_s("Unable to Reset Device");
		return E_FAIL;
	}

	if (FAILED(g_pd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &g_pSurface, NULL)))
	{
		printf_s("Unable to Create Surface");
		return E_FAIL;
	}

	return S_OK;
}

void ScreenCapture9::Cleanup()
{
	if (g_pSurface)
	{
		g_pSurface->Release();
		g_pSurface = NULL;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}
	if (g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}