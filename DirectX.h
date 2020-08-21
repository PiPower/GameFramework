#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <d2d1.h>
#include <dwrite.h>

#include <chrono>
struct Point
{
	Point(float x, float y,float u,float v) :X(x), Y(y),U(u),V(v) {}

	float X, Y;
	float U, V;
};


class Graphics
{
	friend class Camera;
	friend class BoardMenager;
	friend class GraphicalObject;
public:
	Graphics(HWND hwnd, int height, int width);
	~Graphics();
	void EndFrame();
	void BeginFrame();
	void Resize(HWND hwnd, int width, int height);
private:
	struct cBuff
	{
		float Proportion;
		float padd[3];
	};
	cBuff                                               ConstBuff;
	bool                                                resize;
	HWND                                                hwnd;
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Buffer>                pCBuff;
	D3D11_VIEWPORT   vp;
	int                                                 width;
	int                                                 height;

	Microsoft::WRL::ComPtr<ID2D1RenderTarget>          d2RenderTarget;
	Microsoft::WRL::ComPtr< ID2D1SolidColorBrush>      SolidColorBrush;
	Microsoft::WRL::ComPtr< IDWriteTextFormat>         TextFormat;

};

