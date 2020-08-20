struct VOut
{
	float2 tex : TEXCOORD0;
	float4 pos : SV_Position;
};

cbuffer cBuff
{
	matrix Transforms;
};
VOut main(float2 position : POSITION, float2 texCoord : TEXCOORD0) 
{
	VOut output;
	output.pos = mul(float4(position.x,position.y,0,1),Transforms);
	output.tex = texCoord;
	
	return output;
}