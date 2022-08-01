struct VSIn
{
    float3 pos  : POSITION0;         // position
    float3 norm : NORMAL;           // normal
    float2 tex  : TEXCOORD0;         // texture coordinate
};

struct VSOut
{
    float4 pos   : SV_POSITION;     // position
    float2 tex   : TEXCOORD0;       // texture coordinate
    float3 wPos  : POSITION0;       // world space pos
    float3 wNorm : NORMAL;			// world space normal
    float depth  : depth;
};

struct PSIn
{
    float4 pos   : SV_POSITION;     // position
    float2 tex   : TEXCOORD0;       // texture coordinate
    float3 wPos  : POSITION0;       // world space pos
    float3 wNorm : NORMAL;			// world space normal
    float depth  : depth;
};

struct SMPSOut
{
	float target : SV_Target0;
	//float depth : SV_Depth;
};

struct PSOut2
{
    float4 target0 : SV_Target0;
    float4 target1 : SV_Target1;
};

struct PostProcess_VSIn
{
    uint vertexId : SV_VERTEXID;
};

struct PostProcess_VSOut
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct PostProcess_PSIn
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct Light
{
    float3 position;
    float4 diffuse;
    float4 specular;
    float4 ambient;
};
