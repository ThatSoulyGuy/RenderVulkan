

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
    
    output.position = float4(positions[input.vertexID], 0.0f, 1.0f);
    
    return output;
}