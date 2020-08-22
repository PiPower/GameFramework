#include "BoardMenager.h"

using namespace Microsoft::WRL;
using namespace DirectX;

BoardMenager::BoardMenager(Graphics* pGFX,float BlockScale)
	:
pGFX(pGFX), OffsetX(0), OffsetY(0), ScaleX(1), ScaleY(1), RotationAngle(0),BlockScale(BlockScale)
	{
		this->proportion = (float)pGFX->width / (float)pGFX->height;


		for (float y = BlockScale; y < 1.0f; y += BlockScale * 2.0f)
		{
			Lines.emplace_back(Line{ -1, y, 0, 0, 0 });
			Lines.emplace_back(Line{ 1, y, 0, 0, 0 });
		}
		for (float y = -BlockScale; y > -1.0f; y -= BlockScale * 2.0f)
		{
			Lines.emplace_back(Line{ -1, y, 0, 0, 0 });
			Lines.emplace_back(Line{ 1, y, 0, 0, 0 });
		}

		for (float x = BlockScale; x < 1.0f; x += BlockScale * 2)
		{
			Lines.emplace_back(Line{ x, -1, 0, 0, 0 });
			Lines.emplace_back(Line{ x, 1, 0, 0, 0 });
		}
		for (float x = -BlockScale; x > -1.0f; x -= BlockScale * 2)
		{
			Lines.emplace_back(Line{ x, -1, 0, 0, 0 });
			Lines.emplace_back(Line{ x, 1, 0, 0, 0 });
		}

		HRESULT hr;
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
		bd.ByteWidth = Lines.size() * sizeof(Line);
		bd.StructureByteStride = sizeof(Line);
		D3D11_SUBRESOURCE_DATA sbd = {};
		sbd.pSysMem = Lines.data();
		hr = pGFX->pDevice->CreateBuffer(&bd, &sbd, &pVertexBuffer);

		// Creating Vertex shader-------------------------------------------------------------
		ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(L"VanillaVertexShader.cso", &pBlob);
		hr = pGFX->pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);

		// creating input layout-------------------------------------------------------------
		const D3D11_INPUT_ELEMENT_DESC ied[]
		{
			{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,8,D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		hr = pGFX->pDevice->CreateInputLayout(ied, std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);

		// Creating pixel shader-------------------------------------------------------------
		D3DReadFileToBlob(L"VanillaPixelShader.cso", &pBlob);
		hr = pGFX->pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

		PosTranform.transforms = XMMatrixTranspose(XMMatrixScaling(this->ScaleX, this->ScaleY, 0));
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

void BoardMenager::AddBlocks( Window* wnd,std::wstring path,Camera& cam)
{
	while (!wnd->IsMouseEventEmpty())
	{
		const auto e = wnd->ReadMouseEvent();
		if (e.Type == Window::MouseEvent::Event::LeftPress)
		{
			float PosX = wnd->GetMousePosXNormalized();
			float PosY = wnd->GetMousePosYNormalized()/proportion;

			int BlockNumbX = floor( (PosX+ BlockScale) / (BlockScale * 2));
			int BlockNumbY = floor((PosY+ BlockScale) /( BlockScale * 2));

			GraphicalObject* NewRect = new GraphicalObject(wnd->pDx11, path,
				BlockNumbX* BlockScale * 2-cam.OffsetX, BlockNumbY * BlockScale * 2 - cam.OffsetY, BlockScale, BlockScale, 0);
			

			NewRect->SetUVcord(193, 208, 1, 16);

			auto Rl = NewRect->GetVertecies();
			Blocks.push_back(NewRect);


		}
	}
}

void BoardMenager::DrawBlocks()
{
	for (auto& b : Blocks)
	{
		b->Draw();
	}
}

void BoardMenager::SaveBoard(std::string path)
{
	std::fstream stream(path, std::ios::out| std::ios::trunc);
	for (auto& p : Blocks)
	{
		p->Save(stream);
	}
	stream.close();
}

void BoardMenager::LoadBoard(std::string path,std::wstring TexPath)
{
	std::fstream stream(path, std::ios::in );
	std::string Line;
	while (std::getline(stream, Line))
	{
		std::string substring="";
		std::vector<float> elements;

		for (int i = 0; i < Line.size();i++)
		{
			char c = Line[i];
			if (c != ' ' && c != '\n')
			{
				substring += Line[i];
			}
			else
			{
				elements.push_back(std::stof(substring));
				substring = "";
			}
		}
		elements.push_back(std::stof(substring));
		substring = "";

		GraphicalObject* p = new GraphicalObject(pGFX, TexPath, elements[0], elements[1], elements[2], elements[3], elements[4]);
		p->SetUVcord(193, 208, 1, 16);
		Blocks.push_back(p);
	}

	stream.close();
}

BoardMenager::~BoardMenager()
{
	for (auto& p : Blocks)
	{
		delete p;
	}
}

void BoardMenager::Draw()
{
	const UINT stride = sizeof(Line);
	const UINT offset = 0u;
	HRESULT hr;

		
	this->proportion = (float)pGFX->width / (float)pGFX->height;
	PositionTransformer TransformMap;
	TransformMap.transforms = XMMatrixTranspose(XMMatrixRotationZ(this->RotationAngle)
		* XMMatrixScaling(this->ScaleX, this->ScaleY, 0) * XMMatrixTranslation(this->OffsetX, this->OffsetY, 0));
	TransformMap.proportion = this->proportion;

	D3D11_MAPPED_SUBRESOURCE	mappedData;
	hr = pGFX->pImmediateContext->Map(pCBuffTranform.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedData);
	memcpy(mappedData.pData, &TransformMap, sizeof(TransformMap));
	pGFX->pImmediateContext->Unmap(pCBuffTranform.Get(), 0u);

	pGFX->pImmediateContext->RSSetViewports(1, &(pGFX->vp));
	pGFX->pImmediateContext->VSSetConstantBuffers(0u, 1u, pCBuffTranform.GetAddressOf());
	pGFX->pImmediateContext->IASetInputLayout(pInputLayout.Get());
	pGFX->pImmediateContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
	pGFX->pImmediateContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	pGFX->pImmediateContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	pGFX->pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	pGFX->pImmediateContext->Draw(Lines.size(), 0u);
}
