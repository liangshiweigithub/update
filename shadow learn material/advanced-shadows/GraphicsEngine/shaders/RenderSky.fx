#include "include/States.fx"
#include "include/Structures.fx"

cbuffer cbPerFrame
{
    float4x4 g_mView;
    float4x4 g_mProj;
};

float4x4 g_mTex;
float4x4 g_mWorld;
Texture2D g_txDiffuse;

VSOut VSRenderSky(VSIn input)
{
    VSOut output = (VSOut)0.0;

    //output our final position in clipspace
    float4 worldPos = mul( float4( input.pos, 1 ), g_mWorld );
    float4 cameraPos = mul( worldPos, g_mView );
    output.pos = mul( cameraPos, g_mProj );
    
    //propogate texture coordinate
    output.tex = (float2)mul(float4(input.tex, 0, 0), g_mTex);
    
    return output;
}

float4 PSRenderSky(PSIn input) : SV_Target
{   
    return g_txDiffuse.Sample( SampAnisotropicWrap, input.tex );
}

#include "include/Basic3d.fx"

technique11 RenderSky
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSRenderSky() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSky() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );     
    }  
}
