#include "include/States.fx"
#include "include/Structures.fx"

//=============================================================================
// Uniforms
//=============================================================================

float4 g_color;
Texture2D texDiffuse;

//=============================================================================
// Vertexshaders
//=============================================================================

PostProcess_VSOut PostProcess_VS(PostProcess_VSIn input)
{
	PostProcess_VSOut output = (PostProcess_VSOut)0.0;

	output.tex = float2((input.vertexId << 1) & 2, input.vertexId & 2);
	output.pos = float4(output.tex * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);

	return output;
}

//=============================================================================
// Pixelshaders
//=============================================================================

float4 PostProcess_PS(PostProcess_PSIn input) : SV_Target
{
	return g_color;
}

float4 PostProcessTextured_PS(PostProcess_PSIn input) : SV_Target
{
	return texDiffuse.Sample(SampAnisotropicWrap, input.tex);
}


//=============================================================================
// Techniques
//=============================================================================

technique11 DisplayQuad
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PostProcess_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

technique11 DisplayTexturedQuad
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PostProcessTextured_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

