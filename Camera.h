#pragma once
#include "DirectX.h"

//Slot 1 of VS Const buffer reserved for Camera Offsets
class Camera
{
public:
	Camera(Graphics* pGFX,float OffsetX=0,float OffsetY=0);
	void BindConstBuffer();
	void UpdateOffsets(float OffX = 0, float OffY = 0);
public:
	float OffsetX;
	float OffsetY;
private:
	struct Offsets
	{
		float OffsetX;
		float OffsetY;
		float padd[2];
	};

	Offsets  CameraTransforms;
	Microsoft::WRL::ComPtr<ID3D11Buffer>  pCBuffTranform;
	Graphics* pGFX = nullptr;
};

