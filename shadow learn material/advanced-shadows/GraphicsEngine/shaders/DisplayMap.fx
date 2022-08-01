
#include "include/Common.fx"

//=============================================================================
// Uniforms
//=============================================================================

Texture2D<float4> depthMap;
Texture2D<uint2> idepthMap;

Texture2D texMap;

Texture2DArray depthMapArray;
Texture2DArray texMapArray;

int g_arrayIndex;
int2 g_mapSize;
float4x4 g_mTex;
float4 g_texBounds;
float2 g_EVSMConst;
int	   g_filterWidth;

//=============================================================================
// Vertexshaders
//=============================================================================

PostProcess_VSOut PostProcess_VS(PostProcess_VSIn input)
{
	// Generate a full-screen triangle from vertex ID's

	PostProcess_VSOut output = (PostProcess_VSOut)0.0;

	output.tex = float2((input.vertexId << 1) & 2, input.vertexId & 2);
	//output.pos = float4(output.tex, 0.5, 1) * 2 - 1;
	output.pos = float4(output.tex * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);

	return output;
}

PostProcess_VSOut PSPostProcess_VS(PostProcess_VSIn input)
{
	// Generate a full-screen triangle from vertex ID's

	PostProcess_VSOut output = (PostProcess_VSOut)0.0;

	float2 uv  = float2((input.vertexId << 1) & 2, input.vertexId & 2);
	output.pos = float4(uv * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
	output.tex = mul(output.pos, g_mTex).xy;

	return output;
}

//=============================================================================
// Pixelshaders
//=============================================================================

float4 DisplayDepthMap_PS(PostProcess_PSIn input) : SV_Target
{
    float depth = depthMap.Sample(SampPointClamp, input.tex).r;

    return float4(depth, depth, depth, 1.0f);
}

float4 DisplayDepthMapArray_PS(PostProcess_PSIn input) : SV_Target
{
    float depth = depthMapArray.Sample(SampPointClamp, float3(input.tex, g_arrayIndex)).r;
    
    return float4(depth, depth, depth, 1.0f);
}

float4 DisplayESMDepthMap_PS(PostProcess_PSIn input) : SV_Target
{
	float depth = depthMap.Sample(SampPointClamp, input.tex).r;
	depth = log(depth) / g_EVSMConst.x;
	
	return float4(depth, depth, depth, 1.0f);
}

float4 DisplayLogESMDepthMap_PS(PostProcess_PSIn input) : SV_Target
{
	float depth = depthMap.Sample(SampPointClamp, input.tex).r;
	depth = depth / g_EVSMConst.x;
	
	return float4(depth, depth, depth, 1.0f);
}

float4 DisplaySAVSMDepthMap_PS(PostProcess_PSIn input) : SV_Target
{
	float2 halftexel = 0.5 / g_mapSize;

	halftexel *= float2(g_filterWidth,g_filterWidth);

	float2 br = input.tex + float2(halftexel.x,halftexel.y);
	float2 bl = input.tex + float2(-halftexel.x,halftexel.y);
	float2 tr = input.tex + float2(halftexel.x,-halftexel.y);
	float2 tl = input.tex + float2(-halftexel.x,-halftexel.y);
	
	br = clampPoint(br, g_texBounds);
	bl = clampPoint(bl, g_texBounds);
	tr = clampPoint(tr, g_texBounds);
	tl = clampPoint(tl, g_texBounds);
	
	float c0 = depthMap.Sample(SampPointBorder, br).r;
	float c1 = depthMap.Sample(SampPointBorder, bl).r;
	float c2 = depthMap.Sample(SampPointBorder, tr).r;
	float c3 = depthMap.Sample(SampPointBorder, tl).r;
	
	float w = (br.x - bl.x)*(g_mapSize.x);
	float h = (br.y - tr.y)*(g_mapSize.y);
	
	float div = w*h;
	div = div <= 0.0 ? 1.0 : div;

	float moments = (c3 - c2 - c1 + c0) / div;

	return float4(moments,moments,moments, 1.0f);
}

float4 DisplayDistrSAVSMDepthMap_PS(PostProcess_PSIn input) : SV_Target
{
	float2 halftexel = 0.5 / g_mapSize;

	halftexel *= float2(g_filterWidth,g_filterWidth);

	float2 br = input.tex + float2(halftexel.x,halftexel.y);
	float2 bl = input.tex + float2(-halftexel.x,halftexel.y);
	float2 tr = input.tex + float2(halftexel.x,-halftexel.y);
	float2 tl = input.tex + float2(-halftexel.x,-halftexel.y);

	br = clampPoint(br, g_texBounds);
	bl = clampPoint(bl, g_texBounds);
	tr = clampPoint(tr, g_texBounds);
	tl = clampPoint(tl, g_texBounds);

	float4 c0 = depthMap.Sample(SampPointBorder, br);
	float4 c1 = depthMap.Sample(SampPointBorder, bl);
	float4 c2 = depthMap.Sample(SampPointBorder, tr);
	float4 c3 = depthMap.Sample(SampPointBorder, tl);

	float w = (br.x - bl.x)*(g_mapSize.x);
	float h = (br.y - tr.y)*(g_mapSize.y);

	float div = w*h;
	div = div <= 0.0 ? 1.0 : div;

	float4 moments = (c3 - c2 - c1 + c0) / div;

	float depth = RecombinePrecision(moments).r;

	return float4(depth,depth,depth, 1.0f);
}

float4 DisplayUintSAVSMDepthMap_PS(PostProcess_PSIn input) : SV_Target
{	
	float2 halftexel = 0.5 / g_mapSize;

	halftexel *= float2(g_filterWidth,g_filterWidth);
	
	float2 br = input.tex + float2(halftexel.x,halftexel.y);
	float2 bl = input.tex + float2(-halftexel.x,halftexel.y);
	float2 tr = input.tex + float2(halftexel.x,-halftexel.y);
	float2 tl = input.tex + float2(-halftexel.x,-halftexel.y);
	
	br = clampPoint(br, g_texBounds);
	bl = clampPoint(bl, g_texBounds);
	tr = clampPoint(tr, g_texBounds);
	tl = clampPoint(tl, g_texBounds);
	
	uint c0 = idepthMap.Load(int3(br * g_mapSize.x,0)).r;
	uint c1 = idepthMap.Load(int3(bl * g_mapSize.x,0)).r;
	uint c2 = idepthMap.Load(int3(tr * g_mapSize.x,0)).r;
	uint c3 = idepthMap.Load(int3(tl * g_mapSize.x,0)).r;
	
	float w = (br.x - bl.x)*(g_mapSize.x);
	float h = (br.y - tr.y)*(g_mapSize.y);
	
	float div = w*h;
	div = div <= 0.0 ? 1.0 : div;

	float moments = (c3 - c2 - c1 + c0) / FloatToUintSAT / div;
	
	return float4(moments, moments, moments, 1.0f);
}

float4 DisplayMap_PS(PostProcess_PSIn input) : SV_Target
{
    return texMap.Sample(SampPointClamp, input.tex);
    //return (float4)texMap.Sample(SampPointClamp, input.tex).a / 1000.0;
}

float4 DisplayMapArray_PS(PostProcess_PSIn input) : SV_Target
{
    return texMapArray.Sample(SampPointClamp, float3(input.tex, g_arrayIndex));
}

float2 MinMaxMipMap_PS(PostProcess_PSIn input) : SV_Target
{
    float2 offset = 1.0f / g_mapSize;    
    
    float2 depth = depthMap.Sample(SampPointWrap, input.tex).rg, depth1;
    
    depth1 = depthMap.Sample(SampPointWrap, input.tex + float2(.0f, offset.y)).rg;
    depth = float2(min(depth.x, depth1.x), max(depth.y, depth1.y));
    
    depth1 = depthMap.Sample(SampPointWrap, input.tex + float2(offset.x, .0f)).rg;
    depth = float2(min(depth.x, depth1.x), max(depth.y, depth1.y));
    
    depth1 = depthMap.Sample(SampPointWrap, input.tex + float2(offset.x, offset.y)).rg;
    depth = float2(min(depth.x, depth1.x), max(depth.y, depth1.y));
    
    return depth;
}

float2 MinMipMap_PS(PostProcess_PSIn input) : SV_Target
{
    float2 offset = 1.0f / g_mapSize;    
    
    float depth = depthMap.Sample(SampPointWrap, input.tex).r, depth1;
    
    depth1 = depthMap.Sample(SampPointWrap, input.tex + float2(.0f, offset.y)).r;
    depth = min(depth, depth1);
    
    depth1 = depthMap.Sample(SampPointWrap, input.tex + float2(offset.x, .0f)).r;
    depth = min(depth, depth1);
    
    depth1 = depthMap.Sample(SampPointWrap, input.tex + float2(offset.x, offset.y)).r;
    depth = min(depth, depth1);
    
    return (float2)depth;
}

#include "include/Basic2d.fx"

//=============================================================================
// Techniques
//=============================================================================

technique11 DisplayDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        //SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

technique11 DisplayDepthMapArray
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayDepthMapArray_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

technique11 DisplayESMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayESMDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

technique11 DisplayLogESMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayLogESMDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

technique11 DisplaySAVSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplaySAVSMDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    } 

	pass p1
    {
        SetVertexShader( CompileShader( vs_5_0, PSPostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplaySAVSMDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

technique11 DisplayDistrSAVSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayDistrSAVSMDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }
	
    pass p1
    {
        SetVertexShader( CompileShader( vs_5_0, PSPostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayDistrSAVSMDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    } 
}

technique11 DisplayUintSAVSMDepthMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayUintSAVSMDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
	
	pass p1
    {
        SetVertexShader( CompileShader( vs_5_0, PSPostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayUintSAVSMDepthMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    } 
}

technique11 DisplayTexMap
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayMap_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

technique11 DisplayTexMapArray
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, DisplayMapArray_PS() ) );
        
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        //SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );
    }  
}

technique11 GenMipmap
{
	pass GenMinMipmap
	{
		SetVertexShader( CompileShader( vs_5_0, PostProcess_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, MinMipMap_PS() ) );
        
        SetDepthStencilState( DSNoDepthStencil, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSSolidFillCullFront );	
	}
}