#include "Camera.h"

Camera::Camera(Graphics* pGFX, float OffsetX, float OffsetY)
	:
pGFX(pGFX),OffsetX(OffsetX), OffsetY(OffsetY)
{

	HRESULT hr;
	CameraTransforms.OffsetX = OffsetX;
	CameraTransforms.OffsetY = OffsetY;

	D3D11_BUFFER_DESC cbd2 = {};
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DYNAMIC;
	cbd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd2.ByteWidth = sizeof(CameraTransforms);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA sbd4;
	sbd4.pSysMem = &CameraTransforms;
	hr = pGFX->pDevice->CreateBuffer(&cbd2, &sbd4, &pCBuffTranform);

}

void Camera::BindConstBuffer()
{
	Offsets TransformMap{ OffsetX,OffsetY };
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE	mappedData;
	hr = pGFX->pImmediateContext->Map(pCBuffTranform.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedData);
	memcpy(mappedData.pData, &TransformMap, sizeof(TransformMap));
	pGFX->pImmediateContext->Unmap(pCBuffTranform.Get(), 0u);

	pGFX->pImmediateContext->VSSetConstantBuffers(1u, 1u, pCBuffTranform.GetAddressOf());
}

void Camera::UpdateOffsets(float OffX, float OffY)
{
	OffsetX += OffX;
	OffsetY += OffY;
}
