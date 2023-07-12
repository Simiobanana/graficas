#pragma once
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include "CCanvas.h"
#define SAFE_RELEASE( x ) if ((x)){ (x)->Release() ; (x)=nullptr;}

class CDXManager
{
protected:
	ID3D11Device* m_pDev; //Resouce management
	ID3D11DeviceContext* m_pCtx; // Operations on resources
	IDXGISwapChain* m_pSwapChain;        // Swap Cahin
public:
	bool Initialize(HWND hWnd);
	CCanvas* GetCanvas();
	void Show(CCanvas* pCanvas);
	void Resize(int sx, int sy);
	void Uninitialize();
	boolean Ready() { return m_pDev && m_pCtx && m_pSwapChain; }
};



