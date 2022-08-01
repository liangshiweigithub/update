#include "include/States.fx"
#include "include/Structures.fx"

static const float3 MipMapColors[4] =
{
	float3(1.0f,  .0f,  .0f),
	float3(1.0f, 1.0f,  .0f),
	float3( .0f, 1.0f,  .0f),
	float3( .0f,  .0f, 1.0f)
};

//=============================================================================
// Buffers and constants
//=============================================================================

static const float TexAtlasBoundaryBias = 0.00001f;
static const float DistrFactor = 256.0f;
static const float FloatToUintSAT = 1 << 21;
static const uint MAX_FILTERSIZE = 33;

cbuffer cbConstant
{
	static const float3 iv[] =
	{
		float3(.0, 1/7.75, 1/7.75),
		float3(1/7.75, 1/3.25, 1/3.25-1/7.75),
		float3(1/3.25, 1, 1-1/3.25),
		float3(1, 3.25, 3.25-1),
		float3(3.25, 7.75, 7.75-3.25),
		float3(7.75, 10, 10-7.75)
		
	};
	
	static const float3 clr[] =
	{
		float3(0.2, 0, 0),
		float3(1, 0.2, 0),
		float3(1, 1, 0),
		float3(0, 1, 0),
		float3(0.3, 0.8, 1),
		float3(0, 0, 1),
		float3(0, 0, 0.4)
	};
};

//=============================================================================
// Common uniforms
//=============================================================================

Light    g_light;
float4x4 g_mWorld;
float4x4 g_mDiffuseTex;

//=============================================================================
// Material
//=============================================================================

float4 matAmbient;
float4 matDiffuse;
float4 matSpecular;
float  matShininess;

//=============================================================================
// Texturing
//=============================================================================

bool      textured;
bool	  gammaCorrect;
Texture2D texDiffuse;

//=============================================================================
// Functions
//=============================================================================

float4 log_space(float w0, float4 d0, float w1, float4 d1){
	return (d0 + log(w0 + (w1 * exp(d1 - d0))));
}

float4 lin_space(float w0, float4 d0, float w1, float4 d1){
	return (w0 * d0 + w1 * d1);
}

//clamp uv coords to texture border in texture atlas
float2 clampPoint(float2 uv, float4 bounds, float bias = TexAtlasBoundaryBias)
{
	float2 clamped = uv;

	clamped.x = min(max(clamped.x, bounds.x), bounds.z-bias);
	clamped.y = min(max(clamped.y, bounds.y), bounds.w-bias);

	return clamped;
}

float4 DistributePrecision(float2 moments)  
{  
	float FactorInv = 1 / DistrFactor;  
	
	// Split precision  
	float2 IntPart;  
	float2 FracPart = modf(moments * DistrFactor, IntPart);  
  
	return float4(IntPart * FactorInv, FracPart);  
}

float2 RecombinePrecision(float4 moments)  
{  
	float FactorInv = 1 / DistrFactor;  
	return (moments.zw * FactorInv + moments.xy);
}

float4 Lighting(float3 viewVec, float3 normal, float3 lightVec, float2 texCoord, float3 shadowContrib)
{
	float4 diffuseColor = matDiffuse;

	//if gammacorrect linearize diffuse material colors before lighting calculations
	//no need to "unlinearize" because srgb-backbuffer is then used which does it anyway
	if(gammaCorrect) 
		diffuseColor = float4(pow(abs(matDiffuse.rgb), 2.2), matDiffuse.a);

    //if ( textured )
	//	diffuseColor *= texDiffuse.Sample(SampAnisotropicWrap, texCoord);
    
    float diffuseAmount  = saturate( dot(normal, lightVec) );
    float specularAmount = 0;
    
    float4 lightContrib = matAmbient * g_light.ambient;
    
    if (diffuseAmount > 0)
    {
		float3 reflVec = normalize( 2.0f * diffuseAmount * normal - lightVec );
		specularAmount = pow( saturate(dot(reflVec, viewVec)), matShininess );
		
		lightContrib += float4(shadowContrib, 1.0f) *
						(diffuseAmount * diffuseColor * g_light.diffuse + specularAmount * matSpecular * g_light.specular);
	}

	lightContrib.a = diffuseColor.a;
	
	if ( textured )
		lightContrib *= texDiffuse.Sample(SampTrilinearWrap, texCoord);

    return lightContrib;
}

float4 WithoutShading(float2 texCoord)
{
	if(textured)
		return texDiffuse.Sample(SampAnisotropicWrap, texCoord);
	else if(gammaCorrect) //return linear colors because srgb backbuffer will "unlinearize" it
		return float4(pow(abs(matDiffuse.rgb), 2.2), matDiffuse.a);
	else
		return matDiffuse;
}

float4 VisualizeTexels(float2 uv, float texMapSize, int size, float splitFactor)
{
	float4 color = (float4)1.0f;

	float texelSize = size * splitFactor / texMapSize;
	float lineWidth = 1.0f;

	// compute grid using the scaled texelsize
	// from "Logarithmic Perspective Shadow Maps" [Llo07]
	float2 scaledTC = uv / texelSize;

	float2 dS = float2(ddx(scaledTC.x), ddy(scaledTC.x));
	float2 dT = float2(ddx(scaledTC.y), ddy(scaledTC.y));
	float2 m = frac( scaledTC );
	
	if( m.x < lineWidth * length(dS) || m.y < lineWidth * length(dT) )
		color = float4(.0f, .0f, .0f, 1.0f);
	
	return color;
}

float4 VisualizeSampling(float2 uv, float texMapSize, float splitFactor, int error)
{
	float2 dSdX = 1.0f/splitFactor * texMapSize * ddx( uv );
	float2 dSdY = 1.0f/splitFactor * texMapSize * ddy( uv );
	
	float area = .0f;
	
	if (error == 0)
		// determinant of x- and y-direction
		area = abs(dSdX.x * dSdY.y - dSdY.x * dSdX.y);
		//area = max(length(dSdX + dSdY), length(dSdX - dSdY));
	else if (error == 1)
		//	sampling rate for x-direction
	    area = length(float2(dSdX.x, dSdY.x));
    else
		// sampling rate for y-direction
        area = length(float2(dSdX.y, dSdY.y)); 

	float3 result = (float3)1.0f;

	[unroll] for (int i = 0; i < 6; ++i)
	{
		if (area >= iv[i].x && area < iv[i].y)
		{
			result = lerp(clr[i], clr[i+1], (area-iv[i].x)/iv[i].z);
			break;
		}
		else
			result = clr[6];
	}
	
    return float4(result, 1.0);       
}




