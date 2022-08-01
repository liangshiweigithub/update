
#include "include/Common.fx"

//=============================================================================
// Structures
//=============================================================================

struct GenSAT_VSIn
{
    uint vertexId : SV_VERTEXID;
};

struct GenSAT_VSOut
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

struct GenSAT_PSIn
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

//=============================================================================
// States
//=============================================================================

RasterizerState RSGenSAT
{
    CullMode = None;
    FillMode = Solid;
};


//=============================================================================
// Uniforms
//=============================================================================

cbuffer cbPerFrame
{
	int		g_texWidth;
	int		g_texHeight;
	int		g_offset;
	int		g_samples;
	float4x4 g_mTex;
	float4	g_texBounds;
};

Texture2D<float4> map;
Texture2D<uint2> imap;

//=============================================================================
// Vertexshaders
//=============================================================================

GenSAT_VSOut GenSAT_VS(GenSAT_VSIn input)
{
	GenSAT_VSOut output = (GenSAT_VSOut)0.0;

	output.tex = float2((input.vertexId << 1) & 2, input.vertexId & 2);
	output.pos = float4(output.tex * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);

	return output;
}

GenSAT_VSOut PSGenSAT_VS(GenSAT_VSIn input)
{
	GenSAT_VSOut output = (GenSAT_VSOut)0.0;

	float2 uv  = float2((input.vertexId << 1) & 2, input.vertexId & 2);
	output.pos = float4(uv * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
	output.tex = mul(output.pos, g_mTex).xy;

	return output;
}

//=============================================================================
// Pixelshaders
//=============================================================================

float2 GenerateSATX_PS(GenSAT_PSIn input) : SV_Target
{
	// Horizontal Pass
	float2 s = input.tex;
	float2 offset = float2(1.0/ float(g_texWidth) * float(g_offset), 0.0f);

	float2 sum = 0;
	for(int i=0;i<g_samples;i++)
	{
		float2 off = s + i * offset;
		float2 sample = map.Sample(SampPointBorder, off).rg;
		if(off.x >= g_texBounds.x && off.y >= g_texBounds.y && off.x < g_texBounds.z && off.y < g_texBounds.w)	
			sum += sample;
	}

	return sum;
}

float2 GenerateSATY_PS(GenSAT_PSIn input) : SV_Target
{
	// Vertical Pass
	float2 s = input.tex;
	float2 offset = float2(0.0f, 1.0/ float(g_texHeight) * float(g_offset));

	float2 sum = 0;
	for(int i=0;i<g_samples;i++)
	{
		float2 off = s + i * offset;
		float2 sample = map.Sample(SampPointBorder, off).rg;
		if(off.x >= g_texBounds.x && off.y >= g_texBounds.y && off.x < g_texBounds.z && off.y < g_texBounds.w)	
			sum += sample;
	}

	return sum;
}

uint2 GenerateUintSATX_PS(GenSAT_PSIn input) : SV_Target
{
	// Horizontal Pass
	int3 s = int3(input.tex.x * g_texWidth, input.tex.y * g_texHeight, 0);
	int3 offset = int3(g_offset,0,0);
	int4 bounds = int4(g_texBounds.x * g_texWidth, g_texBounds.y * g_texHeight, g_texBounds.z * g_texWidth, g_texBounds.w * g_texHeight);

	uint2 sum = 0;
	for(int i=0;i<g_samples;i++)
	{
		int3 off = s + i * offset;
		uint2 sample = imap.Load(off);
		if(off.x >=  bounds.x && off.y >=  bounds.y && off.x < bounds.z && off.y < bounds.w)	
			sum += sample;
	}

	return sum;
}

uint2 GenerateUintSATY_PS(GenSAT_PSIn input) : SV_Target
{
	// Vertical Pass
	int3 s = int3(input.tex.x * g_texWidth, input.tex.y * g_texHeight, 0);
	int3 offset = int3(0,g_offset,0);
	int4 bounds = int4(g_texBounds.x * g_texWidth, g_texBounds.y * g_texHeight, g_texBounds.z * g_texWidth, g_texBounds.w * g_texHeight);

	uint2 sum = 0;
	for(int i=0;i<g_samples;i++)
	{
		int3 off = s + i * offset;
		uint2 sample = imap.Load(off);
		if(off.x >=  bounds.x && off.y >=  bounds.y && off.x < bounds.z && off.y < bounds.w)	
			sum += sample;
	}

	return sum;
}

float4 GenerateDistrSATX_PS(GenSAT_PSIn input) : SV_Target
{
	// Horizontal Pass
	float2 s = input.tex;
	float2 offset = float2(1.0/ float(g_texWidth) * float(g_offset), 0.0f);

	float4 sum = 0;
	for(int i=0;i<g_samples;i++)
	{
		float2 off = s + i * offset;
		float4 sample = map.Sample(SampPointBorder, off);
		if(off.x >= g_texBounds.x && off.y >= g_texBounds.y && off.x < g_texBounds.z && off.y < g_texBounds.w)	
			sum += sample;
	}

	return sum;
}

float4 GenerateDistrSATY_PS(GenSAT_PSIn input) : SV_Target
{
	// Vertical Pass
	float2 s = input.tex;
	float2 offset = float2(0.0f, 1.0/ float(g_texHeight) * float(g_offset));

	float4 sum = 0;
	for(int i=0;i<g_samples;i++)
	{
		float2 off = s + i * offset;
		float4 sample = map.Sample(SampPointBorder, off);
		if(off.x >= g_texBounds.x && off.y >= g_texBounds.y && off.x < g_texBounds.z && off.y < g_texBounds.w)	
			sum += sample;
	}

	return sum;
}

//=============================================================================
// Techniques
//=============================================================================

technique11 GenerateSAT
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, GenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateSATX_PS()));

        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p1
    {
        SetVertexShader(CompileShader(vs_5_0, GenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateSATY_PS()));
        
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p2
    {
        SetVertexShader(CompileShader(vs_5_0, PSGenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateSATX_PS()));

        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p3
    {
        SetVertexShader(CompileShader(vs_5_0, PSGenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateSATY_PS()));
        
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }
}

technique11 GenerateDistrSAT
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, GenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateDistrSATX_PS()));

        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p1
    {
        SetVertexShader(CompileShader(vs_5_0, GenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateDistrSATY_PS()));
        
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p2
    {
        SetVertexShader(CompileShader(vs_5_0, PSGenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateDistrSATX_PS()));

        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p3
    {
        SetVertexShader(CompileShader(vs_5_0, PSGenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateDistrSATY_PS()));
        
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }
}

technique11 GenerateUintSAT
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, GenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateUintSATX_PS()));

        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p1
    {
        SetVertexShader(CompileShader(vs_5_0, GenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateUintSATY_PS()));
        
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p2
    {
        SetVertexShader(CompileShader(vs_5_0, PSGenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateUintSATX_PS()));

        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }

	pass p3
    {
        SetVertexShader(CompileShader(vs_5_0, PSGenSAT_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GenerateUintSATY_PS()));
        
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSGenSAT );
    }
}
