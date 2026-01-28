Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct VSInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput o;
    o.svpos = float4(input.pos, 1.0f);
    o.uv = input.uv;
    return o;
}

float4 PSMain(VSOutput input) : SV_Target
{
    return tex.Sample(samp, input.uv);
}

technique11 SplitScreen
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VSMain()));
        SetPixelShader(CompileShader(ps_5_0, PSMain()));
    }
}