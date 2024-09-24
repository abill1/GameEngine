struct inData 
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct outData 
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

outData main( inData input )
{
	outData output;
	output.pos = input.pos;
	output.color = input.color;
	return output;
}