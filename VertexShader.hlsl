struct VOut
{
	float2 tex : TEXCOORD0;
	float4 pos : SV_Position;
};

cbuffer cBuff
{
	matrix Transforms;
	float proportion;
};

cbuffer CameraTransform
{
	float OffsetX;
	float OffsetY;
};
VOut main(float2 position : POSITION, float2 texCoord : TEXCOORD0) 
{
	VOut output;
	output.pos = mul(float4(position.x,position.y,0,1),Transforms);
	//------------
	output.pos.x = output.pos.x + OffsetX;
	output.pos.y = output.pos.y + OffsetY;
	//------------------
	output.pos.y = output.pos.y * proportion;


	output.tex = texCoord;
	
	return output;
}