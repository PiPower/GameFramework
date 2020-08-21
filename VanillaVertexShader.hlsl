struct VSOut
{
    float3 col: COLOR;
    float4 pos: SV_Position;
};

cbuffer cBuff
{
    matrix Transforms;
};
VSOut main(float2 Pos:POSITION, float3 col : COLOR)
{
    VSOut vsout;
    vsout.pos = mul(float4(Pos, 0, 1), Transforms);
    vsout.col = col;
    return vsout;
}