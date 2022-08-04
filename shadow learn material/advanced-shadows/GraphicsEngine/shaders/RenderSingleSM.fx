
#include "include/Shadows.fx"

// disable warning 4121: gradient based operation inside of flow control
#pragma warning( disable : 4121 )

//=======================================================================================
// Uniforms
//=======================================================================================

cbuffer cbPerFrame
{
	float4x4 g_mView;
    float4x4 g_mProj;
    float4x4 g_mCamView;
    float4x4 g_mNormLightViewProj;
    float4x4 g_mLightView;
    bool	 g_visTexels;
    int		 g_shadowMapSz;
	float2	 g_LightRadius;
	int		 g_texelSz;
	float3	 g_CamPos; 
	bool	 g_visSampling;
	bool	 g_renderShadows;
	int		 g_errorDir;
	float3	 g_viewVec;
	float3	 g_lightVec;
	bool	 g_hwPCF;
	bool	 g_blockerSearch;
	bool	 g_shading;
	bool	 g_distributePrecision;
	float2	 g_EVSMConst;
	float	 g_LBRAmount;
	bool	 g_LogBlur;
	int		 g_FilterWidth;
	bool	 g_VisPCFRegions;
	bool	 g_MipMapping;
	bool	 g_VisMipMap;
	int		 g_MaxSamples;
};

Texture2D<uint2> itexShadow;
Texture2D<float4> texShadow;

//=======================================================================================
// Vertexshaders
//=======================================================================================

VSOut VSRenderSingleSMmain(VSIn input)
{
    VSOut output = (VSOut)0.0;

    float4 worldPos = mul( float4( input.pos, 1 ), g_mWorld );
    float4 eyePos = mul( worldPos, g_mView );
    
    output.pos   = mul( eyePos, g_mProj );
    output.tex   = mul( float4( input.tex, 1, 1 ), g_mDiffuseTex ).xy;
	output.wPos  = worldPos.xyz;
    output.wNorm = normalize( mul( input.norm, (float3x3)g_mWorld ) );
    output.depth = -mul( worldPos, g_mCamView ).z;
       
	return output;
}

//=======================================================================================
// Pixelshaders
//=======================================================================================

float4 PSRenderSingleSMmain(PSIn input, uniform int method) : SV_Target
{   
	if(g_shading)
	{
		float4 lPos = mul( float4(input.wPos, 1), g_mNormLightViewProj );
		float2 uv = lPos.xy / lPos.w;
		float zReceiver = lPos.z / lPos.w;
	
		float3 normal = normalize( input.wNorm );
		float3 viewVec = normalize( g_CamPos - input.wPos );
		float3 lightVec = normalize( g_light.position.xyz - input.wPos );
     
		float3 shadowContrib = (float3)1.0;
		bool pcfused = false;
		float miplevel = 0;

		if (g_renderShadows)
		{
			float zEye = .0f;

			switch (method)
			{
				case 0:
					shadowContrib = ShadowLookUp(texShadow, uv, zReceiver);
					break;
				case 1:
					shadowContrib = HWPCFShadowLookUp(texShadow, uv, zReceiver);
					break;
				case 2:
					//zEye = mul(float4(input.wPos, 1), g_mLightView).z;
					//shadowContrib = BoxPCFLookUp(texShadow, uv, zReceiver, zEye, g_LightRadius, g_hwPCF, g_blockerSearch);
					shadowContrib = BoxPCF(texShadow, uv, zReceiver, g_shadowMapSz, g_hwPCF);
					break;
				case 3:
					//zEye = mul(float4(input.wPos, 1), g_mLightView).z;
					//shadowContrib = PoissonPCFLookUp(texShadow, uv, zReceiver, zEye, g_LightRadius, g_hwPCF, g_blockerSearch);
					shadowContrib = PoissonPCF(texShadow, uv, zReceiver, g_shadowMapSz, g_hwPCF);
					break;
				case 4:
					zEye = mul(float4(input.wPos, 1), g_mLightView).z;
					shadowContrib = PoissonPCFPCSSLookUp(texShadow, uv, zReceiver, zEye, g_LightRadius, g_hwPCF);
					break;
				case 5:
					if(g_MipMapping)
						shadowContrib = ESMLookUp(texShadow, uv, zReceiver, g_EVSMConst.x, g_LogBlur, g_shadowMapSz, g_FilterWidth, miplevel, pcfused);
					else
						shadowContrib = ESMLookUp(texShadow, uv, zReceiver, g_EVSMConst.x, g_LogBlur, g_shadowMapSz, pcfused);
					break;
				case 6:
					if(g_MipMapping)
						shadowContrib = VSMLookUp(texShadow, uv, zReceiver, g_LBRAmount, g_FilterWidth, g_shadowMapSz, miplevel);
					else
						shadowContrib = VSMLookUp(texShadow, uv, zReceiver, g_LBRAmount);
					break;
				case 7:
					if(g_MipMapping)
						shadowContrib = EVSMLookUp(texShadow, uv, zReceiver, g_EVSMConst, g_FilterWidth, g_shadowMapSz, miplevel);
					else
						shadowContrib = EVSMLookUp(texShadow, uv, zReceiver, g_EVSMConst);
					break;
				case 8:
					if(g_MipMapping)
						shadowContrib = BoxPCF(texShadow, uv, zReceiver, g_shadowMapSz, g_hwPCF, g_FilterWidth, g_MaxSamples, miplevel);
					else
						shadowContrib = BoxPCF(texShadow, uv, zReceiver, g_shadowMapSz, g_hwPCF, g_FilterWidth, g_MaxSamples);
					break;
				case 9:
					if(g_MipMapping)
						shadowContrib = SAVSMLookUp(texShadow, uv, zReceiver, g_shadowMapSz, g_distributePrecision, g_LBRAmount, g_FilterWidth, float4(0.0f,0.0f,1.0f,1.0f), miplevel);
					else
						shadowContrib = SAVSMLookUp(texShadow, uv, zReceiver, g_shadowMapSz, g_distributePrecision, g_LBRAmount, g_FilterWidth, float4(0.0f,0.0f,1.0f,1.0f));
					break;
				case 10:
					if(g_MipMapping)
						shadowContrib = UintSAVSMLookUp(itexShadow, uv, zReceiver, g_shadowMapSz, g_LBRAmount, g_FilterWidth, float4(0.0f,0.0f,1.0f,1.0f), miplevel);
					else
						shadowContrib = UintSAVSMLookUp(itexShadow, uv, zReceiver, g_shadowMapSz, g_LBRAmount, g_FilterWidth, float4(0.0f,0.0f,1.0f,1.0f));
					break;
			}
		}				

		float4 color = Lighting(viewVec, normal, lightVec, input.tex, shadowContrib);
		
		if(g_MipMapping && g_VisMipMap && method > 4)
			color.rgb = lerp(color.rgb, MipMapColors[miplevel%4], 0.2f);

		if(pcfused && g_VisPCFRegions)
			color.rgb = lerp(color.rgb, float3(1.0f,0.0f,0.0f), 0.2f);

		if (uv.x >= .0f && uv.y >= .0f && uv.x <= 1.0f && uv.y <= 1.0f)
		{
			[flatten] if (g_visTexels) 
				color *= VisualizeTexels(uv, g_shadowMapSz, g_texelSz, 1.0);
	
			[flatten] if (g_visSampling)
				color *= VisualizeSampling(uv, g_shadowMapSz, 1.0f, g_errorDir);
		}
	
		return color;
	}
	
	return WithoutShading(input.tex);
}

#include "include/Basic3d.fx"

//=======================================================================================
// Techniques
//=======================================================================================

technique11 RenderSingleSM
{
    pass Common
    {
        SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(0) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass HWPCF
    {
        SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(1) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass BoxPCF
    {
        SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(2) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass PoissonPCF
    {
        SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(3) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass PoissonPCSSPCF
    {
        SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(4) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }

	pass ESM
    {
        SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(5) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }

	pass VSM
	{
		SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(6) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
	}

	pass EVSM
	{
		SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(7) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
	}

	pass VarBoxPCF
    {
        SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(8) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }

	pass SAVSM
	{
		SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(9) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
	}

	pass UINTSAVSM
	{
		SetVertexShader( CompileShader( vs_5_0, VSRenderSingleSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSRenderSingleSMmain(10) ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSAlphaToCoverage, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
	}
}


