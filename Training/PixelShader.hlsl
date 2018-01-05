struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float4 color: COLOR;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    // return interpolated color
    // return mul(input.color, t1.Sample(s1, input.texCoord));
    return input.color;
}
