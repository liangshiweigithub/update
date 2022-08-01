#include "include/Common.fx"

//=============================================================================
// States
//=============================================================================

DepthStencilState DSPostProcess
{
    DepthEnable = false;
    DepthWriteMask = 0;
    StencilEnable = false;
};

RasterizerState RSPostProcess
{
    CullMode = None;
    FillMode = Solid;
    ScissorEnable = false;
    MultisampleEnable = false;
};


//=============================================================================
// Uniforms
//=============================================================================

cbuffer cbPerFrame
{
	float2	 g_SourceSize;            
	float2	 g_BlurDim;
	uint		 g_BlurSamples;
	float4x4 g_mTex;
	float4	g_texBounds;
	float	g_Weights[MAX_FILTERSIZE];
};

Texture2D map;

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

PostProcess_VSOut PSPostProcess_VS(PostProcess_VSIn input)
{
	PostProcess_VSOut output = (PostProcess_VSOut)0.0;

	float2 uv  = float2((input.vertexId << 1) & 2, input.vertexId & 2);
	output.pos = float4(uv * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
	output.tex = mul(output.pos, g_mTex).xy;

	return output;
}

//=============================================================================
// Pixelshaders
//=============================================================================

float4 LogBoxBlur_PS(PostProcess_PSIn input) : SV_Target
{
	float2 texelSize = 1 / g_SourceSize;
	float2 sampleOffset = texelSize * g_BlurDim;
	float2 offset = 0.5f * float(g_BlurSamples-1) * sampleOffset;

	float2 uv = input.tex - offset;

	float4 sum, sample, sample0, sample1;
	float weight;

	weight = 1 / (float)g_BlurSamples;
	sample0 = map.SampleLevel(SampPointClamp, clampPoint(uv, g_texBounds), 0);
	sample1 = map.SampleLevel(SampPointClamp, clampPoint(uv + sampleOffset, g_texBounds), 0);
	sum = log_space(weight,sample0,weight,sample1);

	for (uint i = 2; i < g_BlurSamples; ++i)
	{
		sample = map.SampleLevel(SampPointClamp, clampPoint(uv + i * sampleOffset, g_texBounds), 0);
		sum = log_space(1.0, sum, weight, sample);
	}

	return sum;
}

float4 BoxBlur_PS(PostProcess_PSIn input) : SV_Target
{
	float2 texelSize = 1 / g_SourceSize;
	float2 sampleOffset = texelSize * g_BlurDim;
	float2 offset = 0.5f * float(g_BlurSamples-1) * sampleOffset;

	float2 uv = input.tex - offset;

	float4 sum, sample, sample0, sample1;
	float weight;

	weight = 1 / (float)g_BlurSamples;
	sample0 = map.SampleLevel(SampPointClamp, clampPoint(uv, g_texBounds), 0);
	sample1 = map.SampleLevel(SampPointClamp, clampPoint(uv + sampleOffset, g_texBounds), 0);
	sum = lin_space(weight,sample0,weight,sample1);

	for (uint i = 2; i < g_BlurSamples; ++i)
	{
		sample = map.SampleLevel(SampPointClamp, clampPoint(uv + i * sampleOffset, g_texBounds), 0);
		sum = lin_space(1.0, sum, weight, sample);
	}

	return sum;
}

float4 LogGaussianBlur_PS(PostProcess_PSIn input) : SV_Target
{
	float2 texelSize = 1 / g_SourceSize;
	float2 sampleOffset = texelSize * g_BlurDim;
	float2 offset = 0.5f * float(g_BlurSamples-1) * sampleOffset;

	float2 uv = input.tex - offset;

	float4 sum, sample, sample0, sample1;

	sample0 = map.SampleLevel(SampPointClamp, clampPoint(uv, g_texBounds), 0);
	sample1 = map.SampleLevel(SampPointClamp, clampPoint(uv + sampleOffset, g_texBounds), 0);
	sum = log_space(g_Weights[0],sample0,g_Weights[1],sample1);

	for (uint i = 2; i < g_BlurSamples; ++i)
	{
		sample = map.SampleLevel(SampPointClamp, clampPoint(uv + i * sampleOffset, g_texBounds), 0);
		sum = log_space(1.0, sum, g_Weights[i], sample);
	}

	return sum;
}

float4 GaussianBlur_PS(PostProcess_PSIn input) : SV_Target
{
	float2 texelSize = 1 / g_SourceSize;
	float2 sampleOffset = texelSize * g_BlurDim;
	float2 offset = 0.5f * float(g_BlurSamples-1) * sampleOffset;

	float2 uv = input.tex - offset;

	float4 sum, sample, sample0, sample1;

	sample0 = map.SampleLevel(SampPointClamp, clampPoint(uv, g_texBounds), 0);
	sample1 = map.SampleLevel(SampPointClamp, clampPoint(uv + sampleOffset, g_texBounds), 0);
	sum = lin_space(g_Weights[0],sample0,g_Weights[1],sample1);

	for (uint i = 2; i < g_BlurSamples; ++i)
	{
		sample = map.SampleLevel(SampPointClamp, clampPoint(uv + i * sampleOffset, g_texBounds), 0);
		sum = lin_space(1.0, sum, g_Weights[i], sample);
	}

	return sum;
}

//=============================================================================
// Techniques
//=============================================================================

technique11 BoxBlur
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, PostProcess_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, BoxBlur_PS()));
        
        SetDepthStencilState( DSPostProcess, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSPostProcess );
    }
    
    pass p1
    {
        SetVertexShader(CompileShader(vs_5_0, PSPostProcess_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, BoxBlur_PS()));
        
        SetDepthStencilState( DSPostProcess, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSPostProcess );
    }
}

technique11 LogBoxBlur
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, PostProcess_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, LogBoxBlur_PS()));
        
        SetDepthStencilState( DSPostProcess, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSPostProcess );
    }
    
    pass p1
    {
        SetVertexShader(CompileShader(vs_5_0, PSPostProcess_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, LogBoxBlur_PS()));
        
        SetDepthStencilState( DSPostProcess, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSPostProcess );
    }
}

technique11 GaussianBlur
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, PostProcess_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GaussianBlur_PS()));
        
        SetDepthStencilState( DSPostProcess, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSPostProcess );
    }
    
    pass p1
    {
        SetVertexShader(CompileShader(vs_5_0, PSPostProcess_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, GaussianBlur_PS()));
        
        SetDepthStencilState( DSPostProcess, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSPostProcess );
    }
}

technique11 LogGaussianBlur
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, PostProcess_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, LogGaussianBlur_PS()));
        
        SetDepthStencilState( DSPostProcess, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSPostProcess );
    }
    
    pass p1
    {
        SetVertexShader(CompileShader(vs_5_0, PSPostProcess_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, LogGaussianBlur_PS()));
        
        SetDepthStencilState( DSPostProcess, 0x00000000 );
        SetBlendState( BSNoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState( RSPostProcess );
    }
}