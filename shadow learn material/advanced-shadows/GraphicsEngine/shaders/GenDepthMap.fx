
#include "include/Common.fx"

//=======================================================================================
// Macros
//=======================================================================================

#if NBROFSPLITS > 1
#define NUMSPLITS NBROFSPLITS
#else
#define NUMSPLITS 1
#endif

//=======================================================================================
// Structures
//=======================================================================================

struct VSDepthMapIn
{
    float3 pos  : POSITION;         // position
    float2 tex  : TEXCOORD;         // texture coordinate
};

struct VSDepthMapOut
{
    float4 pos   : SV_POSITION;     // position
    float2 tex   : TEXCOORD0;       // texture coordinate
    float depth  : Depth;			// depth
};

struct GSDepthMapOut
{
    float4 pos   : SV_POSITION;				// position
    float2 tex   : TEXCOORD0;				// texture coordinate
    float depth  : Depth;
    uint vpIndex : SV_ViewportArrayIndex;	// viewport index
};

struct PSDepthMapIn
{
    float4 pos   : SV_POSITION;     // position
    float2 tex   : TEXCOORD0;       // texture coordinate
    float depth  : Depth;
};

//=======================================================================================
// Uniforms
//=======================================================================================

cbuffer cbPerFrame
{
	float4x4 g_mView;
    float4x4 g_mProj;
	float2	 g_EVSMConst;
    float4x4 g_mLightViewProj;
    float4x4 g_mCrop[NUMSPLITS];
    float4x4 g_mLiSP[NUMSPLITS];
};

cbuffer cbPerBatch
{
	int g_firstSplit;
	int g_lastSplit;
};

//=======================================================================================
// Vertex shaders
//=======================================================================================

VSDepthMapOut VSSMmain(VSDepthMapIn input)
{
    VSDepthMapOut output = (VSDepthMapOut)0;

    float4 worldPos  = mul( float4( input.pos, 1 ), g_mWorld );
    
    output.pos = mul( worldPos, g_mLightViewProj );
    output.tex = mul( float4( input.tex, 1, 1 ), g_mDiffuseTex ).xy;
    
	return output;
}

VSDepthMapOut VSPreDepth(VSDepthMapIn input)
{
	VSDepthMapOut output = (VSDepthMapOut)0;
	
	float4 worldPos  = mul( float4( input.pos, 1 ), g_mWorld );
    float4 camPos = mul( worldPos, g_mView );
    
    output.pos = mul( camPos, g_mProj );	
	output.depth = -camPos.z;
	
	return output;
}

//=======================================================================================
// Pixel shaders
//=======================================================================================

float PSHWSMmain(PSDepthMapIn input) : SV_Target
{   
    float alpha = matDiffuse.a;
    
    if ( textured )
		alpha = texDiffuse.Sample(SampPointWrap, input.tex).a;
	
	if ( alpha < 0.5 )
		discard;
	
	return 1.0f;
}

float PSESMmain(PSDepthMapIn input) : SV_Target
{   
    float alpha = matDiffuse.a;
    
    if ( textured )
		alpha = texDiffuse.Sample(SampPointWrap, input.tex).a;
	
	if ( alpha < 0.5 )
		discard;
	
	return exp(g_EVSMConst.x * input.pos.z);
}

float PSLogESMmain(PSDepthMapIn input) : SV_Target
{   
    float alpha = matDiffuse.a;
    
    if ( textured )
		alpha = texDiffuse.Sample(SampPointWrap, input.tex).a;
	
	if ( alpha < 0.5 )
		discard;

	return g_EVSMConst.x * input.pos.z;
}

float2 PSVSMmain(PSDepthMapIn input) : SV_Target
{   
    float alpha = matDiffuse.a;
    
    if ( textured )
		alpha = texDiffuse.Sample(SampPointWrap, input.tex).a;
	
	if ( alpha < 0.5 )
		discard;

	float2 depth;
    depth.x = input.pos.z;
    depth.y = depth.x * depth.x;
	
    return depth;
}

float4 PSEVSMmain(PSDepthMapIn input) : SV_Target
{   
    float alpha = matDiffuse.a;
    
    if ( textured )
		alpha = texDiffuse.Sample(SampPointWrap, input.tex).a;
	
	if ( alpha < 0.5 )
		discard;

	float posDepth = exp(g_EVSMConst.x * input.pos.z);
	float negDepth = -exp(-g_EVSMConst.y * input.pos.z);

	return float4(posDepth, posDepth * posDepth, negDepth, negDepth * negDepth);
}

float4 PSDistrSAVSMmain(PSDepthMapIn input) : SV_Target
{   
    float alpha = matDiffuse.a;
    
    if ( textured )
		alpha = texDiffuse.Sample(SampPointWrap, input.tex).a;
	
	if ( alpha < 0.5 )
		discard;

	float2 depth;
    depth.x = input.pos.z;

    depth.y = depth.x * depth.x;
    
	return DistributePrecision(depth);
}

uint2 PSUintSAVSMmain(PSDepthMapIn input) : SV_Target
{   
    float alpha = matDiffuse.a;
    
    if ( textured )
		alpha = texDiffuse.Sample(SampPointWrap, input.tex).a;
	
	if ( alpha < 0.5 )
		discard;

	float depth = input.pos.z;
	float depthsquared = depth * depth;
	
	uint intdepth = round(depth*FloatToUintSAT);
	uint intdepthsquared = round(depthsquared*FloatToUintSAT);

	return uint2(intdepth,intdepthsquared);
}

float2 PSPreDepth(PSDepthMapIn input) : SV_Target
{
	return (float2)input.depth;
}

//=======================================================================================
// Geometry shaders
//=======================================================================================

[maxvertexcount(NUMSPLITS * 3)]
void GSSMmain(triangle VSDepthMapOut input[3], inout TriangleStream<GSDepthMapOut> stream)
{
	// for all splits which contain the current object
	for (int split = g_firstSplit; split <= g_lastSplit; split++)
	{
		GSDepthMapOut output = (GSDepthMapOut)0;
		
		// set the viewport index corresponding to the correct area in the texture atlas
		// according to the current split
		output.vpIndex = split;

		[unroll] for (int vertex = 0; vertex < 3; vertex++)
		{
			output.pos = mul( input[vertex].pos, g_mLiSP[split] );
			output.pos = mul( output.pos, g_mCrop[split] );
			output.tex = input[vertex].tex;
			stream.Append(output);
		}
		stream.RestartStrip();
	}
}

//=======================================================================================
// Techniques
//=======================================================================================

technique11 RenderHWSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSHWSMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSLightRenderDepth );
    }
    
    pass p1
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( CompileShader(gs_5_0, GSSMmain()) );
        SetPixelShader( CompileShader( ps_5_0, PSHWSMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSLightRenderDepth );
    }
}

technique11 RenderESMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSESMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass p1
	{
		SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
		SetGeometryShader( CompileShader(gs_5_0, GSSMmain()) );
		SetPixelShader( CompileShader( ps_5_0, PSESMmain() ) );
	    
		SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
		SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
	}
}

technique11 RenderLogESMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSLogESMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass p1
	{
		SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
		SetGeometryShader( CompileShader(gs_5_0, GSSMmain()) );
		SetPixelShader( CompileShader( ps_5_0, PSLogESMmain() ) );
	    
		SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
		SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
	}
}

technique11 RenderVSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSVSMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass p1
	{
		SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
		SetGeometryShader( CompileShader(gs_5_0, GSSMmain()) );
		SetPixelShader( CompileShader( ps_5_0, PSVSMmain() ) );
	    
		SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
		SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
	}
}

technique11 RenderEVSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSEVSMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass p1
	{
		SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
		SetGeometryShader( CompileShader(gs_5_0, GSSMmain()) );
		SetPixelShader( CompileShader( ps_5_0, PSEVSMmain() ) );
	    
		SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
		SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );
	}
}

technique11 RenderDistrSAVSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSDistrSAVSMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSLightRenderSATDepth );
    }
    
    pass p1
	{
		SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
		SetGeometryShader( CompileShader(gs_5_0, GSSMmain()) );
		SetPixelShader( CompileShader( ps_5_0, PSDistrSAVSMmain() ) );
	    
		SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
		SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSLightRenderSATDepth );
	}
}

technique11 RenderSAVSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSVSMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSLightRenderSATDepth );
    }
    
    pass p1
	{
		SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
		SetGeometryShader( CompileShader(gs_5_0, GSSMmain()) );
		SetPixelShader( CompileShader( ps_5_0, PSVSMmain() ) );
	    
		SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
		SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSLightRenderSATDepth );
	}
}

technique11 RenderUintSAVSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSUintSAVSMmain() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSLightRenderSATDepth );
    }
    
    pass p1
	{
		SetVertexShader( CompileShader( vs_5_0, VSSMmain() ) );
		SetGeometryShader( CompileShader(gs_5_0, GSSMmain()) );
		SetPixelShader( CompileShader( ps_5_0, PSUintSAVSMmain() ) );
	    
		SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
		SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSLightRenderSATDepth );
	}
}

technique11 RenderPreDepth
{
	pass p0
	{
		SetVertexShader( CompileShader( vs_5_0, VSPreDepth() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PSPreDepth() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullBack );		
	}
}



