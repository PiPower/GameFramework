#pragma once
#include "DirectX.h"
#include "ImageFile.h"
#include <vector>
#include <DirectXMath.h>
class GraphicalObject
{
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
		float proportion;
	};
	struct CollRect
	{
		DirectX::XMFLOAT2  TopLeft;
		DirectX::XMFLOAT2  BottomRight;
	};
public:
	GraphicalObject(Graphics* gfx, std::wstring& path, float OffsetX=0, float OffsetY=0, float ScaleX=1,float ScaleY=1,float RotationAngle=0);
	void SetUVcord(int LowerBoundX, int HigherBoundX, int LowerBoundY, int HigherBoundY);
	void Move(float OffsetX = 0, float OffsetY = 0);
	void Scale(float ScaleX = 1, float ScaleY = 1);
	void Rotate(float RotationAngle);
	CollRect GetVertecies();
	~GraphicalObject();
	void Draw();
protected:
	UVTransformer  UVTransform;
	PositionTransformer PosTranform;
	float OffsetX;
	float OffsetY;
	float ScaleX;
	float ScaleY;
	float RotationAngle;
	Graphics* const pGFX = nullptr;
	ImageFile* Img = nullptr;
	static std::vector<Point> Rectangle;
	static CollRect  CollisionRectangle;
	static float proportion;
	static bool init;
	static std::vector<unsigned short> indicies;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11Buffer>  pCBuffUV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>  pCBuffTranform;
	D3D_PRIMITIVE_TOPOLOGY    Topology;
};

