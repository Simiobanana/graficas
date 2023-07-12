#include "CDXManager.h"

bool CDXManager::Initialize(HWND hWnd)
{
	RECT ClientRect;
	D3D_FEATURE_LEVEL lvlRequested = D3D_FEATURE_LEVEL_11_0, lvlCreated;
	DXGI_SWAP_CHAIN_DESC dscd;
	memset(&dscd, 0, sizeof(dscd));
	GetClientRect(hWnd, &ClientRect);
	dscd.BufferCount = 2;
	dscd.BufferDesc.Width = ClientRect.right;
	dscd.BufferDesc.Height = ClientRect.bottom;
	dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dscd.BufferDesc.RefreshRate.Numerator = 0;
	dscd.BufferDesc.RefreshRate.Denominator = 0;
	dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	dscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dscd.OutputWindow = hWnd;
	dscd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dscd.SampleDesc.Count = 1;
	dscd.SampleDesc.Quality = 0;
	dscd.Windowed = TRUE;
	HRESULT hr =
		D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
			&lvlRequested, 1, D3D11_SDK_VERSION, &dscd,
			&m_pSwapChain, &m_pDev, &lvlCreated, &m_pCtx);
	return SUCCEEDED(hr);
}
void CDXManager::Uninitialize()
{
	SAFE_RELEASE(m_pDev);
	SAFE_RELEASE(m_pCtx);
	SAFE_RELEASE(m_pSwapChain);
}

void CDXManager::Show(CCanvas* pCanvas)
{
	D3D11_TEXTURE2D_DESC dtd;
	ID3D11Texture2D* pStage = nullptr;
	ID3D11Texture2D* pBackBuffer = nullptr;

	m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
	pBackBuffer->GetDesc(&dtd);

	//Los tamaños deben ser compatibles
	if (dtd.Width != pCanvas->m_nSizeX ||
		dtd.Height != pCanvas->m_nSizeY)
	{
		SAFE_RELEASE(pBackBuffer);
		return;
	}
	//Crear recurso de adamiaje a partir de los atributos del Bakbuffer
	dtd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dtd.BindFlags = 0;
	dtd.Usage = D3D11_USAGE_STAGING;

	D3D11_SUBRESOURCE_DATA dsd;
	dsd.pSysMem = pCanvas->m_pBuffer;
	dsd.SysMemPitch = pCanvas->m_nPitch;
	dsd.SysMemSlicePitch = 0;
	m_pDev->CreateTexture2D(&dtd, &dsd, &pStage);

	//Transferir el recurso de memoria de andamio (en CPU) al GPU
	m_pCtx->CopyResource(pBackBuffer, pStage);
	SAFE_RELEASE(pStage);
	SAFE_RELEASE(pBackBuffer);

	//transferir el buffer de atras con el frontal
	m_pSwapChain->Present(1, 0);
}

CCanvas* CDXManager::GetCanvas()
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D,
		(void**)&pBackBuffer);
	D3D11_TEXTURE2D_DESC dtd;
	pBackBuffer->GetDesc(&dtd);
	CCanvas* pNewCanvas =
		CCanvas::Create((int)dtd.Width, (int)dtd.Height);
	SAFE_RELEASE(pBackBuffer);
	return pNewCanvas;
}

void CDXManager::Resize(int sx, int sy)
{
	//Liberar todos los recursos atados a este contexto de dispositivo.
	m_pCtx->ClearState();
	//Hacer el cambio de tamaño
	m_pSwapChain->ResizeBuffers(2, max((UINT)sx, 16), max((UINT)sy, 16),
		DXGI_FORMAT_R8G8B8A8_UNORM, 0);
}