//=============================================================================
// Structures
//=============================================================================

struct Basic3d_VSIn
{
    float3 pos  : POSITION0;         // position
    float3 norm : NORMAL;            // normal
    float2 tex  : TEXCOORD0;         // texture coordinate
};

struct Basic3d_VSOut
{
    float4 pos   : SV_POSITION;     // position
    float2 tex   : TEXCOORD0;       // texture coordinate
    float3 wPos  : POSITION0;       // world space pos
    float3 wNorm : NORMAL;			// world space normal
    float3 vPos  : POSITION1;		// world space view pos
    
    noperspective float3 heights : TEXCOORD1;  
};

struct Basic3d_GSIn
{
    float4 pos   : POSITION;
    float4 vPos	 : TEXCOORD0;
};

struct Basic3d_PSIn
{
    float4 pos   : SV_POSITION;     // position
    float2 tex   : TEXCOORD0;       // texture coordinate
    float3 wPos  : POSITION0;       // world space pos
    float3 wNorm : NORMAL;			// world space normal
    float3 vPos  : POSITION1;		// world space view pos
    
    noperspective float3 heights : TEXCOORD1;    
};

//=============================================================================
// Uniforms
//=============================================================================

cbuffer cbImmutable
{
	float3 lPos		= float3(100.0f, 500.0f, 60.0f);
	float3 lAmbient = float3(0.1f, 0.1f, 0.1f);
	float3 lDiffuse = float3(1.0f, 1.0f, 1.0f);
	float  lineWidth = 4.0f;
};

cbuffer cbPerFrame
{
	float4x4 g_View;
	float4x4 g_Proj;
};

cbuffer cbPerBatch
{
	float4x4 g_World;
	float4   g_color;
	int2	 g_viewport;
};

//=============================================================================
// Utility functions
//=============================================================================

float EvalMinDistanceToEdges(Basic3d_PSIn input)
{
    float dist;

	float3 ddxHeights = ddx( input.heights );
	float3 ddyHeights = ddy( input.heights );
	float3 ddHeights2 =  ddxHeights*ddxHeights + ddyHeights*ddyHeights;
	
    float3 pixHeights2 = input.heights *  input.heights / ddHeights2 ;
    
    dist = sqrt( min ( min (pixHeights2.x, pixHeights2.y), pixHeights2.z) );
    
    return dist;
}

//=============================================================================
// Vertex shaders
//=============================================================================

Basic3d_VSOut ScreenSpaceBasic3d_VS( Basic3d_VSIn input )
{
	Basic3d_VSOut output = (Basic3d_VSOut)0;

    output.pos.x =  ( input.pos.x / (g_viewport.x / 2.0f) ) - 1.0f;
    output.pos.y = -( input.pos.y / (g_viewport.y / 2.0f) ) + 1.0f;
    output.pos.z = input.pos.z;
    output.pos.w = 1.0f;
    
    output.tex = input.tex;

	return output;
}

Basic3d_GSIn ScreenSpaceBasic3d_SolidWire_VS( Basic3d_VSIn input )
{
	Basic3d_GSIn output = (Basic3d_GSIn)0;

    output.pos.x =  ( input.pos.x / (g_viewport.x / 2.0f) ) - 1.0f;
    output.pos.y = -( input.pos.y / (g_viewport.y / 2.0f) ) + 1.0f;
    output.pos.z = input.pos.z;
    output.pos.w = 1.0f;
    
    output.vPos = output.pos;

	return output;
}

Basic3d_VSOut Basic3d_VS( Basic3d_VSIn input )
{
	Basic3d_VSOut output = (Basic3d_VSOut)0;
	
	float4 worldPos = mul( float4( input.pos, 1 ), g_World );
    float4 cameraPos = mul( worldPos, g_View );
    
    output.pos   = mul( cameraPos, g_Proj );
    output.wNorm = normalize( mul( input.norm, (float3x3)g_World ) );
    output.wPos  = worldPos.xyz;
	output.vPos  = cameraPos.xyz;   
    
    return output;  
}

//=============================================================================
// Geometry shaders
//=============================================================================

[maxvertexcount(4)]
void Basic3d_SolidWire_GS( line Basic3d_GSIn input[2], inout TriangleStream<Basic3d_PSIn> outStream )
{
    Basic3d_PSIn output = (Basic3d_PSIn)0;

	float scale = lineWidth / (0.5f * g_viewport.x);
    float2 vec = input[0].pos.xy - input[1].pos.xy;
	float2 perp = scale * normalize(float2(vec.y, -vec.x));
    
    output.pos = float4(input[0].pos.xy - 0.5 * perp, input[0].pos.zw);
    output.heights = float3( 1, 0, 0 );
    outStream.Append( output );

    output.pos = float4(input[1].pos.xy - 0.5 * perp, input[1].pos.zw);
    output.heights = float3( 0, 1, 0 );
    outStream.Append( output );
    
    output.pos = float4(input[0].pos.xy + 0.5 * perp, input[0].pos.zw);
    output.heights = float3( 0, 0, 1 );
    outStream.Append( output );
    
    output.pos = float4(input[1].pos.xy + 0.5 * perp, input[1].pos.zw);
    output.heights = float3( 1, 0, 0 );
    outStream.Append( output );
    
    outStream.RestartStrip();
}

//=============================================================================
// Pixel shaders
//=============================================================================

float4 Basic3d_Lighting_PS( Basic3d_PSIn input ) : SV_Target
{
	float3 normal = normalize( input.wNorm );
    float3 viewVec = normalize( input.vPos - input.wPos );
    float3 lightVec = normalize( lPos - input.wPos );

	float diffuseAmount  = max( 0, dot( normal, lightVec ) );
	
	float3 lightContrib = g_color.rgb * lAmbient +
						  diffuseAmount * g_color.rgb * lDiffuse;
	
	return float4(lightContrib, g_color.a); 
}

float4 Basic3d_NoLighting_PS( Basic3d_PSIn input ) : SV_Target
{
	return g_color;  
}

float4 Basic3d_NoLightingSolidWire_PS( Basic3d_PSIn input ) : SV_Target
{
    // Compute the shortest distance between the fragment and the edges.
    float dist = EvalMinDistanceToEdges(input);

    // Cull fragments too far from the edge.
    //if (dist > 0.5*lineWidth+1) discard;

    // Map the computed distance to the [0,2] range on the border of the line.
    //dist = clamp((dist - (0.5*lineWidth-1)), 0, 6);
    dist = clamp((dist - lineWidth), 0, 6);

    // Alpha is computed from the function exp2(-2(x)^2).
    dist *= dist;
    float alpha = exp2(-2*dist);

    // Standard wire color
    float4 color = g_color;
    color.a *= alpha;
	
    return color;
}

//=============================================================================
// Techniques
//=============================================================================

technique11 RenderBasic3d
{
    pass Solid
    {
        SetVertexShader( CompileShader( vs_5_0, Basic3d_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, Basic3d_Lighting_PS() ) );
        SetBlendState( BSSrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
        SetRasterizerState( RSSolidFillCullBack );
    }
    
    pass Wireframe
    {
        SetVertexShader( CompileShader( vs_5_0, Basic3d_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, Basic3d_NoLighting_PS() ) );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DSEnableDepthLessEqual, 0x00000000 );
    }
    
        
    pass ScreenSpace
    {
        SetVertexShader( CompileShader( vs_5_0, ScreenSpaceBasic3d_SolidWire_VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, Basic3d_SolidWire_GS() ) );
        SetPixelShader( CompileShader( ps_5_0, Basic3d_NoLighting_PS() ) );
        SetRasterizerState( RSSolidFillCullNone );
        SetBlendState( BSSrcAlphaBlendingAddSrcDest, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

