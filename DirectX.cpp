#include "DirectX.h"
#include <dxgi.h>
#include <algorithm>
#include <Windowsx.h>
#include <string>
#include <assert.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"D3DCompiler.lib")
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"dwrite.lib")

#pragma warning(disable : 4996).

using namespace Microsoft::WRL;
using namespace std;


Graphics::Graphics(HWND hwnd, int height, int width)
	:
	height(height), width(width),hwnd(hwnd), resize(false)
{
	assert(hwnd != nullptr);
	HRESULT hr;
	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, flags, nullptr, 0, D3D11_SDK_VERSION,
		&sd, &pSwapChain, &pDevice, nullptr, &pImmediateContext);

	ComPtr<ID3D11Resource> pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTargetView);

	pImmediateContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);

	vp.Width = this->width;
	vp.Height = this->height;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	pImmediateContext->RSSetViewports(1, &vp);

	ConstBuff.Proportion = (float)width / height;
	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.ByteWidth = sizeof(ConstBuff);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA sbd3;
	sbd3.pSysMem = &ConstBuff;
	pDevice->CreateBuffer(&cbd, &sbd3, &pCBuff);
	//pImmediateContext->VSSetConstantBuffers(0u, 1u, pCBuff.GetAddressOf());
	/*FLOAT dpiX;
	FLOAT dpiY;
	ComPtr<ID2D1Factory> m_pD2DFactory;
	Microsoft::WRL::ComPtr<IDXGISurface>         pBackBuffer2;

	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &m_pD2DFactory);

	m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

	// Create a Direct2D render target which can draw into the surface in the swap chain

	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer2));

	m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX,
			dpiY
		);

	hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(pBackBuffer2.Get(), &props, &d2RenderTarget);

	ComPtr<IDWriteFactory> pDWriteFactory;
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED,
		__uuidof(IDWriteFactory), &pDWriteFactory);

	hr = pDWriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 50.0f, L"en-us", &TextFormat);

	hr = d2RenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &SolidColorBrush);*/
}

Graphics::~Graphics()
{
	if (pImmediateContext) pImmediateContext->ClearState();
}


void Graphics::EndFrame()
{
	HRESULT hr;


	/*wstring FPScounter = L"FPS: " + to_wstring(FPS);
	D2D1_RECT_F layoutRect;
	layoutRect.left = 40;
	layoutRect.top = 0;
	layoutRect.right = 400;
	layoutRect.bottom = 40;
	d2RenderTarget->DrawText(FPScounter.c_str(), FPScounter.size(), TextFormat.Get(), &layoutRect, SolidColorBrush.Get());

	d2RenderTarget->EndDraw();*/
	// flip back/front buffers
	if (FAILED(hr = pSwapChain->Present(1u, 0u)))
	{
		MessageBox(hwnd, L"Swap chain failed", 0, 0);
	}

}



void Graphics::BeginFrame()
{
	const float color[] = { 1,1,1,1 };
	pImmediateContext->ClearRenderTargetView(pRenderTargetView.Get(), color);
	//d2RenderTarget->BeginDraw();
}

void Graphics::Resize(HWND hwnd, int width, int height)
{
	HRESULT hr;

	this->width = width;
	this->height = height;

	pRenderTargetView->Release();

	hr = pSwapChain->ResizeBuffers(0, this->width, this->height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	ComPtr<ID3D11Resource> pBackBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	hr = pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTargetView.GetAddressOf());

	pImmediateContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);

	vp.Width = this->width;
	vp.Height = this->height;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	pImmediateContext->RSSetViewports(1, &vp);
	resize = true;
}

