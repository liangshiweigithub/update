//=============================================================================
// Structures
//=============================================================================

struct Basic2d_VSIn
{
    uint vertexId : SV_VERTEXID;	 // vertexID
};

struct Basic2d_VSOut
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct Basic2d_PSIn
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};


//=============================================================================
// Uniforms
//=============================================================================

cbuffer cbPerFrame
{
	float4 g_color;
};

//=============================================================================
// Vertex shaders
//=============================================================================

Basic2d_VSOut FullscreenBasic2d_VS( Basic2d_VSIn input )
{
	Basic2d_VSOut output = (Basic2d_VSOut)0;

	output.tex = float2((input.vertexId << 1) & 2, input.vertexId & 2);
	output.pos = float4(output.tex * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);

	return output;
}

//=============================================================================
// Pixel shaders
//=============================================================================

float4 Basic2d_PS( Basic2d_PSIn input ) : SV_Target
{
    return g_color;
}

//=============================================================================
// Techniques
//=============================================================================

technique11 RenderBasic2d
{
    pass Fullscreen
    {
        SetVertexShader( CompileShader( vs_5_0, FullscreenBasic2d_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, Basic2d_PS() ) );
        SetBlendState( BSSrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        //SetRasterizerState( RSSolidFillCullFront );
    }
}
