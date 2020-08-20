#include "GraphicalObject.h"

using namespace Microsoft::WRL;
using namespace DirectX;

std::vector<Point> GraphicalObject::Rectangle;
bool  GraphicalObject::init = false;

GraphicalObject::GraphicalObject(Graphics* gfx, float OffsetX, float OffsetY, float ScaleX, float ScaleY, float RotationAngle, ImageFile* imgFile)
	:
 pGFX(gfx), Img(imgFile),OffsetX(OffsetX), OffsetY(OffsetY),ScaleX(ScaleX), ScaleY(ScaleY),RotationAngle(RotationAngle)
{
	this->proportion = (float)pGFX->height / pGFX->width;

	indicies.push_back(0); indicies.push_back(1); indicies.push_back(3);
	indicies.push_back(0); indicies.push_back(3); indicies.push_back(2);
	assert(gfx != nullptr);
	//---------- Creating Rect-------------------------------------------------------------
	if (!GraphicalObject::init)
	{
		Rectangle.push_back(Point(-1, 1, 0, 0));
		Rectangle.push_back(Point(1, 1, 1, 0));
		Rectangle.push_back(Point(-1, -1, 0, 1));
		Rectangle.push_back(Point(1, -1, 1, 1));
	}

	GraphicalObject::init = true;

	HRESULT hr;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = Rectangle.size() * sizeof(Point);
	bd.StructureByteStride = sizeof(Point);
	D3D11_SUBRESOURCE_DATA sbd = {};
	sbd.pSysMem = Rectangle.data();
	pGFX->pDevice->CreateBuffer(&bd, &sbd, &pVertexBuffer);

	// Creating Vertex shader-------------------------------------------------------------
	ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
	hr = pGFX->pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
    
	// creating input layout-------------------------------------------------------------
	const D3D11_INPUT_ELEMENT_DESC ied[]
	{
		{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,8,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	hr = pGFX->pDevice->CreateInputLayout(ied, std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);

	// Creating pixel shader-------------------------------------------------------------
	D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	hr = pGFX->pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);


	// Creating index buffer-------------------------------------------------------------
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = sizeof(indicies)*indicies.size();
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA sbd2;
	sbd2.pSysMem = indicies.data();
	pGFX->pDevice->CreateBuffer(&ibd, &sbd2, &pIndexBuffer);

	// Creating Texture-------------------------------------------------------------
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = imgFile->GetWidth();
	textureDesc.Height = imgFile->GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = imgFile->GetFilePtr();

	sd.SysMemPitch = imgFile->GetWidth() * sizeof(unsigned int);
    pGFX->pDevice->CreateTexture2D(&textureDesc, &sd, &pTexture );

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	pGFX->pDevice->CreateShaderResourceView( pTexture.Get(), &srvDesc, &pTextureView);

	// Creating sampler -------------------------------------------------------------
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT ;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	pGFX->pDevice->CreateSamplerState(&samplerDesc, &pSampler);
	// Creating CBuff for UV transform-----------------------------------------------

	UVTransform.HigherBoundX = 1;
	UVTransform.LowerBoundX = 0;
	UVTransform.HigherBoundY =1;
	UVTransform.LowerBoundY= 0;

	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.ByteWidth = sizeof(UVTransform);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA sbd3;
	sbd3.pSysMem = &UVTransform;
	pGFX->pDevice->CreateBuffer(&cbd, &sbd3, &pCBuffUV);
	pGFX->pImmediateContext->PSSetConstantBuffers(0u, 1u, pCBuffUV.GetAddressOf());

	PosTranform.transforms = XMMatrixTranspose( XMMatrixScaling(proportion * ScaleX, ScaleY, 0) * XMMatrixRotationZ(RotationAngle) + XMMatrixTranslation(OffsetX, OffsetY, 0));


	D3D11_BUFFER_DESC cbd2 = {};
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DYNAMIC;
	cbd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd2.ByteWidth = sizeof(PosTranform);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA sbd4;
	sbd4.pSysMem = &PosTranform;
	pGFX->pDevice->CreateBuffer(&cbd, &sbd3, &pCBuffTranform);
	pGFX->pImmediateContext->VSSetConstantBuffers(1u, 1u, pCBuffTranform.GetAddressOf());
}

void GraphicalObject::SetUVcord( float LowerBoundX, float HigherBoundX, float LowerBoundY,float HigherBoundY)
{
	HRESULT hr;

	UVTransformer UVTransformer2;
	UVTransformer2.HigherBoundX = HigherBoundX;
	UVTransformer2.LowerBoundX = LowerBoundX;
	UVTransformer2.HigherBoundY = HigherBoundY;
	UVTransformer2.LowerBoundY = LowerBoundY;

	D3D11_MAPPED_SUBRESOURCE	mappedData;
	hr=pGFX->pImmediateContext->Map(pCBuffUV.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedData);
	memcpy(mappedData.pData, &UVTransformer2, sizeof(UVTransformer2));
	pGFX->pImmediateContext->Unmap(pCBuffUV.Get(), 0u);
}

void GraphicalObject::Draw()
{
	const UINT stride = sizeof(Point);
	const UINT offset = 0u;
	HRESULT hr;
	/*struct ConstantBuffer
	{
		double dx, dy;
		double scale;
		int iterations;
	};

	const ConstantBuffer cbuff{ OffsetX,OffsetY,Scale,Iterations };
	ComPtr<ID3D11Buffer>cbuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.ByteWidth = sizeof(cbuff);
	bd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA sbd = {};
	sbd.pSysMem = &cbuff;
	hr = pGFX->pDevice->CreateBuffer(&bd, &sbd, &cbuffer);*/


	pGFX->pImmediateContext->IASetInputLayout(pInputLayout.Get());
	pGFX->pImmediateContext->IASetIndexBuffer(pIndexBuffer.Get(),DXGI_FORMAT_R16_UINT,0u);
	pGFX->pImmediateContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
	pGFX->pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pGFX->pImmediateContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	pGFX->pImmediateContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	pGFX->pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pGFX->pImmediateContext->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
	pGFX->pImmediateContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());
	pGFX->pImmediateContext->DrawIndexed(indicies.size(),0u,0u);
}
