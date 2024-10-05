struct inData 
{
	float4 pos : POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

struct outData 
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

cbuffer imageBuffer : register(b0)
{
	float4x4 imgCoord;
};

outData main( inData input )
{
	outData output;
	output.pos = input.pos;
	output.color = input.color;
	float4 tc = float4(input.texcoord[0], input.texcoord[1], 0.0f, 1.0f);
	float4 rtc = mul(imgCoord, tc);
	output.texcoord = float2(rtc[0], rtc[1]);
	return output;
}