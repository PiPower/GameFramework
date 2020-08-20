Texture2D tex;
SamplerState sam;

cbuffer UVTransform
{
	float LowerBoundX;
	float HigherBoundX;
	float LowerBoundY;
	float HigherBoundY;
};

float4 main(float2 texCoord : TEXCOORD0) : SV_TARGET
{
   texCoord.x = texCoord.x *(HigherBoundX - LowerBoundX) + LowerBoundX;
   texCoord.y = texCoord.y * (HigherBoundY - LowerBoundY) + LowerBoundY;

	float4 col= tex.Sample(sam,texCoord);
	return col;
}