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
	output.pos = float4(position,0,1);
	output.pos = mul(output.pos, Transforms);
	output.tex = texCoord;
	
	return output;
}