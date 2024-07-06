

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
	float3 normal : NORMAL;
	float2 textureCoordinates : TEXTURECOORDINATES;
};

float4 Main(PixelInputType input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}