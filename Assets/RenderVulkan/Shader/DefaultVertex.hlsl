

struct VertexInputType
{
    float3 position : POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 textureCoordinates : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 textureCoordinates : TEXCOORD;
};

PixelInputType Main(VertexInputType input)
{
    PixelInputType output;
    
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    output.normal = input.normal;
    output.textureCoordinates = input.textureCoordinates;
    
    return output;
}