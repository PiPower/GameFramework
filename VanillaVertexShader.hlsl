struct VSOut
{
    float3 col: COLOR;
    float4 pos: SV_Position;
};

cbuffer cBuff
{
    matrix Transforms;
    float propotion;
};
cbuffer CameraTransform
{
    float OffsetX;
    float OffsetY;
};
VSOut main(float2 Pos:POSITION, float3 col : COLOR)
{
    VSOut vsout;
    vsout.pos = mul(float4(Pos, 0, 1), Transforms);
    //---------------------
   // vsout.pos.x = vsout.pos.x + OffsetX;
   // vsout.pos.y = vsout.pos.y + OffsetY;
    //---------------------
    vsout.pos.y = vsout.pos.y * propotion;


    vsout.col = col;
    return vsout;
}