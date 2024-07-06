



struct VertexInputType
{
    uint vertexID : SV_VertexID;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
	float2 positions[3] =
	{
		float2( 0.0f, -0.5f),
        float2( 0.5f,  0.5f),
        float2(-0.5f,  0.5f)
	};
    
    output.position = float4(positions[input.vertexID], 0.0f, 1.0f);
    
    return output;
}