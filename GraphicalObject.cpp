#include "GraphicalObject.h"

using namespace Microsoft::WRL;
using namespace DirectX;

std::vector<Point> GraphicalObject::Rectangle;
GraphicalObject::CollRect GraphicalObject::CollisionRectangle;
std::vector<unsigned short> GraphicalObject::indicies;
float GraphicalObject::proportion;
bool  GraphicalObject::init = false;
ComPtr<ID3D11Buffer> GraphicalObject::pVertexBuffer;

GraphicalObject::GraphicalObject(Graphics* gfx, std::wstring& path, float OffsetX, float OffsetY, float ScaleX, float ScaleY, float RotationAngle)
	:
	pGFX(gfx), OffsetX(OffsetX), OffsetY(OffsetY), ScaleX(ScaleX), ScaleY(ScaleY), RotationAngle(RotationAngle)
{
	Img = new ImageFile(path.c_str());
	assert(gfx != nullptr);
	HRESULT hr;

	Topology = Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (!GraphicalObject::init)
	{
	this->proportion = (float)pGFX->width / (float)pGFX->height;
	// ---------Creating Collision Vecotr-------------------------------------------------
	CollisionRectangle.TopLeft.x = -1.0f;
	CollisionRectangle.TopLeft.y = 1.0f;

	CollisionRectangle.BottomRight.x = 1.0f;
	CollisionRectangle.BottomRight.y = -1.0f;

	// ---------Creating indicies-------------------------------------------------
	indicies.push_back(0); indicies.push_back(1); indicies.push_back(3);
	indicies.push_back(0); indicies.push_back(3); indicies.push_back(2);
	//---------- Creating Rect-------------------------------------------------------------

	     Rectangle.push_back(Point(-1, 1, 0, 0));
	     Rectangle.push_back(Point(1, 1, 1, 0));
	     Rectangle.push_back(Point(-1, -1, 0, 1));
	     Rectangle.push_back(Point(1, -1, 1, 1));


		 D3D11_BUFFER_DESC bd = {};
		 bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		 bd.Usage = D3D11_USAGE_DEFAULT;
		 bd.CPUAccessFlags = 0;
		 bd.ByteWidth = Rectangle.size() * sizeof(Point);
		 bd.StructureByteStride = sizeof(Point);
		 D3D11_SUBRESOURCE_DATA sbd = {};
		 sbd.pSysMem = Rectangle.data();
		 pGFX->pDevice->CreateBuffer(&bd, &sbd, &pVertexBuffer);
	}

	GraphicalObject::init = true;


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
	textureDesc.Width = Img->GetWidth();
	textureDesc.Height = Img->GetHeight();
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
	sd.pSysMem = Img->GetFilePtr();

	sd.SysMemPitch = Img->GetWidth() * sizeof(unsigned int);
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

	PosTranform.transforms = XMMatrixTranspose(XMMatrixRotationZ(this->RotationAngle)
	*XMMatrixScaling(this->ScaleX, this->ScaleY, 0)  *XMMatrixTranslation(this->OffsetX, this->OffsetY, 0));
	PosTranform.proportion = this->proportion;

	D3D11_BUFFER_DESC cbd2 = {};
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DYNAMIC;
	cbd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd2.ByteWidth = sizeof(PosTranform);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA sbd4;
	sbd4.pSysMem = &PosTranform;
	hr = pGFX->pDevice->CreateBuffer(&cbd2, &sbd4, &pCBuffTranform);

}

//Give number area in pixel cord u want to draw 
void GraphicalObject::SetUVcord( int LowerBoundX, int HigherBoundX, int LowerBoundY, int HigherBoundY)
{
	HRESULT hr;

	UVTransformer UVTransformer2;
	UVTransformer2.HigherBoundX = (float)HigherBoundX/Img->GetWidth();
	UVTransformer2.LowerBoundX = (float)LowerBoundX/ Img->GetWidth();
	UVTransformer2.HigherBoundY = (float)HigherBoundY/ Img->GetHeight();
	UVTransformer2.LowerBoundY = (float)LowerBoundY/Img->GetHeight();

	D3D11_MAPPED_SUBRESOURCE	mappedData;
	hr=pGFX->pImmediateContext->Map(pCBuffUV.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedData);
	memcpy(mappedData.pData, &UVTransformer2, sizeof(UVTransformer2));

	//UVTransformer* lol =  reinterpret_cast<UVTransformer*>(mappedData.pData);
	//UVTransformer heh = *lol;
	pGFX->pImmediateContext->Unmap(pCBuffUV.Get(), 0u);
}

void GraphicalObject::Move(float OffsetX, float OffsetY)
{
	this->OffsetX += OffsetX;
	this->OffsetY += OffsetY;
}

void GraphicalObject::Save(std::fstream& stream)
{
	stream << OffsetX;
	stream << ' ';
	stream << OffsetY;
	stream << ' ';
	stream << ScaleX;
	stream << ' ';
	stream << ScaleY;
	stream << ' ';
	stream << RotationAngle;
	stream << '\n';
}

void GraphicalObject::Scale(float ScaleX, float ScaleY)
{
	this->ScaleX *= ScaleX;
	this->ScaleY *= ScaleY;
}

void GraphicalObject::Rotate(float RotationAngle)
{
	this->RotationAngle += RotationAngle;
}

GraphicalObject::CollRect GraphicalObject::GetVertecies()
{
	CollRect ret;

	PositionTransformer Transform;
	Transform.transforms = XMMatrixRotationZ(this->RotationAngle)
		* XMMatrixScaling(this->ScaleX, this->ScaleY, 0) * XMMatrixTranslation(this->OffsetX, this->OffsetY, 0);

	//Top Left--------------------------------------------------
	XMVECTOR vec = XMLoadFloat2(&CollisionRectangle.TopLeft);
	XMVECTOR res = XMVector2Transform(vec, Transform.transforms);
	XMFLOAT2 FinalVec;
	XMStoreFloat2(&FinalVec, res);
	ret.TopLeft = FinalVec;
	//Bottom Right   --------------------------------------------------
	XMVECTOR vec2 = XMLoadFloat2(&CollisionRectangle.BottomRight);
	XMVECTOR res2 = XMVector2Transform(vec2, Transform.transforms);
	XMFLOAT2 FinalVec2;
	XMStoreFloat2(&FinalVec2, res2);
	ret.BottomRight = FinalVec2;

	return ret;
}

GraphicalObject::~GraphicalObject()
{
	if (Img != nullptr)
	{
		delete Img;
		Img = nullptr;
	}

}

void GraphicalObject::Draw()
{
	const UINT stride = sizeof(Point);
	const UINT offset = 0u;
	HRESULT hr;
	this->proportion = (float)pGFX->width / (float)pGFX->height;
	

	// Update Transform Matrix
	//multiplicating transY by proportion to keep the relation of offsetY the rest of graphical object the way it was before multiplication 
		PositionTransformer TransformMap;
		TransformMap.transforms = XMMatrixTranspose(XMMatrixRotationZ(this->RotationAngle)
			* XMMatrixScaling(this->ScaleX, this->ScaleY , 0) * XMMatrixTranslation(this->OffsetX, this->OffsetY, 0));
		TransformMap.proportion = this->proportion;

		D3D11_MAPPED_SUBRESOURCE	mappedData;
		hr = pGFX->pImmediateContext->Map(pCBuffTranform.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedData);
		memcpy(mappedData.pData, &TransformMap, sizeof(TransformMap));
		pGFX->pImmediateContext->Unmap(pCBuffTranform.Get(), 0u);
	//---------------------------------------------------

	pGFX->pImmediateContext->RSSetViewports(1, &(pGFX->vp));
	pGFX->pImmediateContext->VSSetConstantBuffers(0u, 1u, pCBuffTranform.GetAddressOf());
	pGFX->pImmediateContext->PSSetConstantBuffers(0u, 1u, pCBuffUV.GetAddressOf());
	pGFX->pImmediateContext->IASetInputLayout(pInputLayout.Get());
	pGFX->pImmediateContext->IASetIndexBuffer(pIndexBuffer.Get(),DXGI_FORMAT_R16_UINT,0u);
	pGFX->pImmediateContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
	pGFX->pImmediateContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	pGFX->pImmediateContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	pGFX->pImmediateContext->IASetPrimitiveTopology(Topology);
	pGFX->pImmediateContext->PSSetShaderResources(0, 1u, pTextureView.GetAddressOf());
	pGFX->pImmediateContext->PSSetSamplers(0, 1, pSampler.GetAddressOf());
	pGFX->pImmediateContext->DrawIndexed(indicies.size(),0u,0u);
}
