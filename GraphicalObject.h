#pragma once
#include "DirectX.h"
#include "ImageFile.h"
#include <vector>
#include <DirectXMath.h>
class GraphicalObject
{
public:
	GraphicalObject(Graphics* gfx, float OffsetX, float OffsetY, float ScaleX,float ScaleY,
	float RotationAngle,ImageFile* imgFile);

	void SetUVcord(float LowerBoundX, float HigherBoundX, float LowerBoundY, float HigherBoundY);
	void Draw();
protected:
	struct UVTransformer
	{
		float LowerBoundX;
		float HigherBoundX;
		float LowerBoundY;
		float HigherBoundY;
	};
	struct PositionTransformer
	{
		DirectX::XMMATRIX transforms;
	};
protected:
	// Edge order: TopLeft,TopRigt,BottomLeft,BottomRight

	UVTransformer  UVTransform;
	PositionTransformer PosTranform;
	static std::vector<Point> Rectangle;
	float proportion;
	static bool init;
	float OffsetX;
	float OffsetY;
	float ScaleX;
	float ScaleY;
	float RotationAngle;
	Graphics* const pGFX = nullptr;
	ImageFile* Img = nullptr;
	std::vector<unsigned short> indicies;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11Buffer>  pCBuffUV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>  pCBuffTranform;
};

