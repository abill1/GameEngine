struct outData
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(outData input) : SV_TARGET
{
	return objTexture.Sample(objSamplerState, input.texcoord);
}