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

outData main( inData input )
{
	outData output;
	output.pos = input.pos;
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}