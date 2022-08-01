#include "include/Poisson.fx"
#include "include/Common.fx"

//=============================================================================
// Constants
//=============================================================================

static const float MinVariance = 0.000001f;

//=============================================================================
// Macros
//=============================================================================

// disable warning 3570: gradient instruction in a loop with varying iteration
#pragma warning( disable : 3570 )

#ifdef PRESET
#define PCF_PRESET PRESET
#else
#define PCF_PRESET 2
#endif

#if PCF_PRESET == 0

#define SEARCH_POISSON_COUNT 16		// 16x Poisson Blockersearch
#define SEARCH_POISSON Poisson16
#define PCF_POISSON_COUNT 25		// 25x Poisson PCF
#define PCF_POISSON Poisson25

#define SEARCH_BOX_COUNT 1			// 2x2 Box Blockersearch
#define PCF_BOX_COUNT 2				// 4x4 Box PCF

#elif PCF_PRESET == 1

#define SEARCH_POISSON_COUNT 25		// 25x Poisson Blockersearch
#define SEARCH_POISSON Poisson25
#define PCF_POISSON_COUNT 32		// 32x Poisson PCF
#define PCF_POISSON Poisson32

#define SEARCH_BOX_COUNT 2			// 4x4 Box Blockersearch
#define PCF_BOX_COUNT 3				// 6x6 Box PCF

#else

#define SEARCH_POISSON_COUNT 32		// 32x Poisson Blockersearch
#define SEARCH_POISSON Poisson32
#define PCF_POISSON_COUNT 64		// 64x Poisson PCF
#define PCF_POISSON Poisson64

#define SEARCH_BOX_COUNT 3			// 6x6 Box Blockersearch
#define PCF_BOX_COUNT 4				// 8x8 Box PCF

#endif

//=============================================================================
// Uniforms
//=============================================================================

float g_LightZNear = 0.1;
float g_LightZFar = 600.0;

//=============================================================================
// Functions
//=============================================================================

float3 ShadowLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver)
{
	return (float3)shadowMap.SampleCmpLevelZero(SampCmp, uv, zReceiver);
}

float3 HWPCFShadowLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver)
{
	return (float3)shadowMap.SampleCmpLevelZero(SampPCF, uv, zReceiver);
}

float3 ShadowLookUpArray(Texture2DArray shadowMap, float2 uv, int index, float zReceiver)
{
	float depth = shadowMap.Sample(SampPointBorder, float3(uv.x, uv.y, index)).r;
	
	return depth < zReceiver ? (float3)0.0f : (float3)1.0f;
}

// Light bleeding reduction
float ReduceLightBleeding(float p, float lbramount)
{
    return smoothstep(lbramount, 1.0f, p);
}

float ChebyshevUpperBound(float2 moments, float depth, float minvar)
{
	float mean = moments.x;
	float meanSqr = moments.y;
	
	float Ex_2 = mean * mean;
	float E_x2 = meanSqr;

	float p = (depth <= mean);

	float variance = max(minvar, E_x2 - Ex_2);
	float d = depth - mean;
	float pmax = variance / (variance + d * d);
	
	return max(p, pmax);
}

float GetMipLevel(float2 uv, int filtersize, float texsize)
{
	float minmiplevel = round(log2(max(1,filtersize)));

	float2 dx = ddx(uv * texsize);
	float2 dy = ddy(uv * texsize);
	float d = max(dot(dx,dx), dot(dy,dy));
	float miplevel = 0.5f * log2(d);

	if(miplevel <= minmiplevel)
		miplevel = 0;

	return miplevel;
}

float3 VSMLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float lbr, float miplevel = 0)
{
	float2 moments = shadowMap.SampleLevel(SampTrilinearClamp, uv, miplevel).rg;

	float lit = ChebyshevUpperBound(moments, zReceiver, MinVariance);
	
	lit = ReduceLightBleeding(lit, lbr);
	
	return (float3)lit;
}

float3 VSMLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float lbr, int filtersize, float texsize, out float miplevel)
{
	miplevel = GetMipLevel(uv, filtersize, texsize);

	return VSMLookUp(shadowMap, uv, zReceiver, lbr, miplevel);
}

float BilinearFilter(float tl, float tr, float bl, float br, float2 uv, float texMapSize)
{
	float2 f = frac( uv * texMapSize ); 
    float top = lerp( tl, tr, f.x );
    float bottom = lerp( bl, br, f.x );
    return lerp( top, bottom, f.y );
}

float SampleESM(Texture2D<float4> shadowMap, SamplerState samp, float2 uv, float zReceiver, float overdark, bool logblur, float miplevel)
{
	float occluder = shadowMap.SampleLevel(samp,uv,miplevel).r;
	float lit = 0.0;

	if(logblur)
		lit = exp(occluder - overdark * zReceiver);
	else
		lit = occluder/exp(overdark*zReceiver);

	return lit;
}

float PCFFallback(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float texMapSize, bool logblur, float overdark)
{
    float2 texelSz = float2(1.0 / texMapSize, 1.0 / texMapSize);  

	float tl = clamp(SampleESM(shadowMap, SampPointClamp, uv + float2( 0.0, 0.0 ) * texelSz,zReceiver,overdark,logblur, 0.0),0.0,1.0);
    float tr = clamp(SampleESM(shadowMap, SampPointClamp, uv + float2( 1.0, 0.0 ) * texelSz,zReceiver,overdark,logblur, 0.0),0.0,1.0);
    float bl = clamp(SampleESM(shadowMap, SampPointClamp, uv + float2( 0.0, 1.0 ) * texelSz,zReceiver,overdark,logblur, 0.0),0.0,1.0);
    float br = clamp(SampleESM(shadowMap, SampPointClamp, uv + float2( 1.0, 1.0 ) * texelSz,zReceiver,overdark,logblur, 0.0),0.0,1.0);
    
	return BilinearFilter(tl, tr, bl, br, uv, texMapSize);
}

float3 ESMLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float overdark, bool logblur, float texMapSize, out bool usePCF, float miplevel = 0)
{
	usePCF = false;
	float lit = SampleESM(shadowMap,SampTrilinearClamp,uv,zReceiver,overdark,logblur,miplevel);

	if(lit > 1.0 + 0.020)
	{
		usePCF = true;
		lit = PCFFallback(shadowMap,uv,zReceiver,texMapSize, logblur, overdark);
	}
	else
		lit = clamp(lit, 0.0, 1.0);

	return (float3)lit;
}

float3 ESMLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float overdark, bool logblur, float texMapSize, int filtersize, out float miplevel, out bool usePCF)
{
	miplevel = GetMipLevel(uv, filtersize, texMapSize);

	return ESMLookUp(shadowMap, uv, zReceiver, overdark, logblur, texMapSize, usePCF, miplevel);
}

float3 EVSMLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float2 overdark, float miplevel = 0)
{
	float4 moments = shadowMap.SampleLevel(SampTrilinearClamp, uv, miplevel);

    float2 posMoments = moments.xy;
    float2 negMoments = moments.zw;
    float expPosDepth = exp(overdark.x * zReceiver);
	float expNegDepth = -exp(-overdark.y * zReceiver);
    
    float posDepthScale = overdark.x * expPosDepth;
    float posMinVariance = MinVariance * (posDepthScale * posDepthScale);
    float poslit = ChebyshevUpperBound(posMoments, expPosDepth, posMinVariance);
    
    float negDepthScale = overdark.y * expNegDepth;
    float negMinVariance = MinVariance * (negDepthScale * negDepthScale);
    float neglit = ChebyshevUpperBound(negMoments, expNegDepth, negMinVariance);
    
    float lit = min(poslit, neglit);

    return (float3)lit;
}

float3 EVSMLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float2 overdark, int filtersize, float texsize, out float miplevel)
{
	miplevel = GetMipLevel(uv, filtersize, texsize);

	return EVSMLookUp(shadowMap, uv, zReceiver, overdark, miplevel);
}

float2 SAGetMoments(Texture2D<float4> map, float2 uv, float texMapSize, bool distr, int filterSize, float4 bounds)
{
	float2 halftexel = 0.5 / texMapSize;

	uv += halftexel; //pixel to texel align

	halftexel *= float2(filterSize,filterSize);

	float2 br = uv + float2(halftexel.x,halftexel.y);
	float2 bl = uv + float2(-halftexel.x,halftexel.y);
	float2 tr = uv + float2(halftexel.x,-halftexel.y);
	float2 tl = uv + float2(-halftexel.x,-halftexel.y);

	br = clampPoint(br, bounds);
	bl = clampPoint(bl, bounds);
	tr = clampPoint(tr, bounds);
	tl = clampPoint(tl, bounds);

	float4 c0 = map.Sample(SampPointBorder, br);
	float4 c1 = map.Sample(SampPointBorder, bl);
	float4 c2 = map.Sample(SampPointBorder, tr);
	float4 c3 = map.Sample(SampPointBorder, tl);

	float w = (br.x - bl.x)*texMapSize;
	float h = (br.y - tr.y)*texMapSize;

	float div = w*h;
	div = div <= 0.0 ? 1.0 : div;

	float4 moments = (c3 - c2 - c1 + c0) / div;

	if(distr)
		return RecombinePrecision(moments);
	else
		return moments.rg;
}

float2 SAGetIntMoments(Texture2D<uint2> map, float2 uv, float texMapSize, int filterSize, float4 bounds)
{
	float2 halftexel = 0.5 / texMapSize;

	uv += halftexel; //pixel to texel align

	halftexel *= float2(filterSize,filterSize);

	float2 br = uv + float2(halftexel.x,halftexel.y);
	float2 bl = uv + float2(-halftexel.x,halftexel.y);
	float2 tr = uv + float2(halftexel.x,-halftexel.y);
	float2 tl = uv + float2(-halftexel.x,-halftexel.y);

	br = clampPoint(br, bounds);
	bl = clampPoint(bl, bounds);
	tr = clampPoint(tr, bounds);
	tl = clampPoint(tl, bounds);
	
	uint2 c0 = map.Load(int3(br * texMapSize,0));
	uint2 c1 = map.Load(int3(bl * texMapSize,0));
	uint2 c2 = map.Load(int3(tr * texMapSize,0));
	uint2 c3 = map.Load(int3(tl * texMapSize,0));
	
	float w = (br.x - bl.x)*texMapSize;
	float h = (br.y - tr.y)*texMapSize;
	
	float div = w*h;
	div = div <= 0.0 ? 1.0 : div;

	float2 moments = (c3 - c2 - c1 + c0) / FloatToUintSAT / (filterSize*filterSize);
	
	return moments;
}

float SampleSAVSM(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float texMapSize, bool distr, float lbr, int filterSize, float4 bounds)
{
	float2 moments = SAGetMoments(shadowMap, uv, texMapSize, distr, filterSize, bounds);

	float lit = ChebyshevUpperBound(moments, zReceiver, MinVariance);
	
	lit = ReduceLightBleeding(lit, lbr);
	
	return lit;
}

float3 SAVSMLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float texMapSize, bool distr, float lbr, int filterSize, float4 bounds)
{
	float halftexelSz = 0.5 / texMapSize;  

	float tl = SampleSAVSM(shadowMap,uv + float2(-halftexelSz, -halftexelSz),zReceiver,texMapSize,distr,lbr,filterSize, bounds);
    float tr = SampleSAVSM(shadowMap,uv + float2(halftexelSz, -halftexelSz),zReceiver,texMapSize,distr,lbr,filterSize, bounds);
    float bl = SampleSAVSM(shadowMap,uv + float2(-halftexelSz, halftexelSz),zReceiver,texMapSize,distr,lbr,filterSize, bounds);
    float br = SampleSAVSM(shadowMap,uv + float2(halftexelSz, halftexelSz),zReceiver,texMapSize,distr,lbr,filterSize, bounds);
    
	return (float3)BilinearFilter(tl, tr, bl, br, uv + float2(-halftexelSz, -halftexelSz), texMapSize);
}

float3 SAVSMLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float texMapSize, bool distr, float lbr, int filtersize, float4 bounds, out float miplevel)
{
	miplevel = GetMipLevel(uv, filtersize, texMapSize);
	filtersize = max(filtersize, pow(2.0,miplevel));

	return SAVSMLookUp(shadowMap, uv, zReceiver, texMapSize, distr, lbr, filtersize, bounds);
}

float SampleUintSAVSM(Texture2D<uint2> shadowMap, float2 uv, float zReceiver, float texMapSize, float lbr, int filterSize, float4 bounds)
{
	float2 moments = SAGetIntMoments(shadowMap, uv, texMapSize, filterSize, bounds);

	float lit = ChebyshevUpperBound(moments, zReceiver, MinVariance);
	
	lit = ReduceLightBleeding(lit, lbr);
	
	return lit;
}

float3 UintSAVSMLookUp(Texture2D<uint2> shadowMap, float2 uv, float zReceiver, float texMapSize, float lbr, int filterSize, float4 bounds)
{
    float halftexelSz = 0.5 / texMapSize;

	float tl = SampleUintSAVSM(shadowMap,uv + float2(-halftexelSz, -halftexelSz), zReceiver,texMapSize,lbr,filterSize, bounds);
	float tr = SampleUintSAVSM(shadowMap,uv + float2(halftexelSz, -halftexelSz), zReceiver,texMapSize,lbr,filterSize, bounds);
    float bl = SampleUintSAVSM(shadowMap,uv + float2(-halftexelSz, halftexelSz), zReceiver,texMapSize,lbr,filterSize, bounds);
    float br = SampleUintSAVSM(shadowMap,uv + float2(halftexelSz, halftexelSz), zReceiver,texMapSize,lbr,filterSize, bounds);
    
	return (float3)BilinearFilter(tl, tr, bl, br, uv + float2(-halftexelSz, -halftexelSz), texMapSize);
}

float3 UintSAVSMLookUp(Texture2D<uint2> shadowMap, float2 uv, float zReceiver, float texMapSize, float lbr, int filtersize, float4 bounds, out float miplevel)
{
	miplevel = GetMipLevel(uv, filtersize, texMapSize);
	filtersize = max(filtersize, pow(2.0,miplevel));

	return UintSAVSMLookUp(shadowMap, uv, zReceiver, texMapSize, lbr, filtersize, bounds);
}

// Using similar triangles from the surface point to the area light
float2 SearchRegionRadiusUV(float2 lightRadius, float zWorld)
{
    return lightRadius * (zWorld - g_LightZNear) / zWorld;
}

// Using similar triangles between the area light, the blocking plane and the surface point
float2 PenumbraRadiusUV(float2 lightRadius, float zReceiver, float zBlocker)
{
    return lightRadius * (zReceiver - zBlocker) / zBlocker;
}

// Project UV size to the near plane of the light
float2 ProjectToLightUV(float2 sizeUV, float zWorld)
{
    return sizeUV * g_LightZNear / zWorld;
}

// Derivatives of light-space depth with respect to texture coordinates
float2 DepthGradient(float2 uv, float z)
{
    float2 dz_duv = 0;

    float3 duvdist_dx = ddx(float3(uv, z));
    float3 duvdist_dy = ddy(float3(uv, z));

    dz_duv.x = duvdist_dy.y * duvdist_dx.z;
    dz_duv.x -= duvdist_dx.y * duvdist_dy.z;
    
    dz_duv.y = duvdist_dx.x * duvdist_dy.z;
    dz_duv.y -= duvdist_dy.x * duvdist_dx.z;

    float det = (duvdist_dx.x * duvdist_dy.y) - (duvdist_dx.y * duvdist_dy.x);
    dz_duv /= det;

    return dz_duv;
}

float BiasedZ(float z, float2 dz_duv, float2 offset)
{
    return z + dot(dz_duv, offset);
}

float ZClipToZEye(float zClip)
{
	return g_LightZFar * g_LightZNear / (g_LightZFar - zClip * (g_LightZFar - g_LightZNear));   
}

float3 PoissonPCF(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float texMapSize, bool hwPCF)
{
    float sum = 0;
    float2 texelSz = float2(1.0 / texMapSize, 1.0 / texMapSize);
    
    for (int i = 0; i < PCF_POISSON_COUNT; ++i)
    {
        float2 offset = PCF_POISSON[i] * texelSz;
        float z = zReceiver;
        
        if (hwPCF) sum += HWPCFShadowLookUp(shadowMap, uv + offset, z).r;
		else sum += ShadowLookUp(shadowMap, uv + offset, z).r;
    }
    
    return (float3)sum / PCF_POISSON_COUNT;
}

float3 PoissonPCFShadowLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float2 dz_duv, float2 filterRadiusUV, bool hwPCF)
{
    float sum = 0;
    
    for (int i = 0; i < PCF_POISSON_COUNT; ++i)
    {
        float2 offset = PCF_POISSON[i] * filterRadiusUV;
        float z = BiasedZ(zReceiver, dz_duv, offset);
        
        if (hwPCF) sum += HWPCFShadowLookUp(shadowMap, uv + offset, z).r;
		else sum += ShadowLookUp(shadowMap, uv + offset, z).r;
    }
    
    return (float3)sum / PCF_POISSON_COUNT;
}

float3 BoxPCF(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float texMapSize, bool hwPCF, int filtersize, int maxsamples)
{
    float sum = 0;
    float2 texelSz = float2(1.0 / texMapSize, 1.0 / texMapSize);

	filtersize = min(filtersize, maxsamples);

    float pcfboxcnt = (float)filtersize / 2.0;

	for (float x = -pcfboxcnt + 0.5; x <= pcfboxcnt - 0.5; ++x)
		for (float y = -pcfboxcnt + 0.5; y <= pcfboxcnt - 0.5; ++y)
        {
            float2 offset = float2( x, y ) * texelSz;
            float z = zReceiver;
            
            if (hwPCF) sum += HWPCFShadowLookUp(shadowMap, uv + offset, z).r;
			else sum += ShadowLookUp(shadowMap, uv + offset, z).r;
        }

    return (float3)sum / (filtersize * filtersize);
}

float3 BoxPCF(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float texMapSize, bool hwPCF, int filtersize, int maxsamples, out float miplevel)
{
	miplevel = GetMipLevel(uv, filtersize, texMapSize);
	filtersize = max(filtersize, pow(2.0,miplevel));

	return BoxPCF(shadowMap, uv, zReceiver, texMapSize, hwPCF, filtersize, maxsamples);
}

float3 BoxPCF(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float texMapSize, bool hwPCF)
{
    float sum = 0;
    float2 texelSz = float2(1.0 / texMapSize, 1.0 / texMapSize);
    
	for (float x = -PCF_BOX_COUNT + 0.5; x <= PCF_BOX_COUNT - 0.5; ++x)
		for (float y = -PCF_BOX_COUNT + 0.5; y <= PCF_BOX_COUNT - 0.5; ++y)
        {
            float2 offset = float2( x, y ) * texelSz;
            float z = zReceiver;
            
            if (hwPCF) sum += HWPCFShadowLookUp(shadowMap, uv + offset, z).r;
			else sum += ShadowLookUp(shadowMap, uv + offset, z).r;
        }
    float numSamples = (2 * PCF_BOX_COUNT);
    
    return (float3)sum / (numSamples * numSamples);
}

float3 BoxPCFShadowLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float2 dz_duv, float2 filterRadiusUV, bool hwPCF)
{
    float sum = 0;
    
	float2 stepUV = filterRadiusUV / PCF_BOX_COUNT;
    for (float x = -PCF_BOX_COUNT + 0.5; x <= PCF_BOX_COUNT - 0.5; ++x)
        for (float y = -PCF_BOX_COUNT + 0.5; y <= PCF_BOX_COUNT - 0.5; ++y)
        {
            float2 offset = float2( x, y ) * stepUV;
            float z = BiasedZ(zReceiver, dz_duv, offset);
            
            if (hwPCF) sum += HWPCFShadowLookUp(shadowMap, uv + offset, z).r;
			else sum += ShadowLookUp(shadowMap, uv + offset, z).r;
        }
    float numSamples = (2 * PCF_BOX_COUNT);
    
    return (float3)sum / (numSamples * numSamples);
}

// Returns average blocker depth in the search region, as well as the number of found blockers.
// Blockers are defined as shadow-map samples between the surface point and the light.
float FindBlockersPoisson(out float avgBlockerDepth, Texture2D<float4> shadowMap,
				   float2 uv, float zReceiver, float2 dz_duv, float2 searchRegionRadiusUV)
{
    float blockerDepth = 0;
    float numBlockers = 0;

    for (int i = 0; i < SEARCH_POISSON_COUNT; ++i)
    {
        float2 offset = SEARCH_POISSON[i] * searchRegionRadiusUV;
        float depth = shadowMap.SampleLevel(SampAnisotropicClamp, uv + offset, 0).r;
        float z = BiasedZ(zReceiver, dz_duv, offset);
        if ( depth < z )
        {
            blockerDepth += depth;
            numBlockers++;
        }
    }
    avgBlockerDepth = blockerDepth / numBlockers;
    
    return numBlockers;
}

// Returns average blocker depth in the search region, as well as the number of found blockers.
// Blockers are defined as shadow-map samples between the surface point and the light.
float FindBlockersBox(out float avgBlockerDepth, Texture2D<float4> shadowMap,
				   float2 uv, float zReceiver, float2 dz_duv, float2 searchRegionRadiusUV)
{
    float blockerDepth = 0;
    float numBlockers = 0;

    float2 stepUV = searchRegionRadiusUV / SEARCH_BOX_COUNT;
    for (float x = -SEARCH_BOX_COUNT + 0.5; x <= SEARCH_BOX_COUNT - 0.5; ++x)
        for (float y = -SEARCH_BOX_COUNT + 0.5; y <= SEARCH_BOX_COUNT - 0.5; ++y)
        {
            float2 offset = float2( x, y ) * stepUV;
            float depth = shadowMap.SampleLevel(SampAnisotropicClamp, uv + offset, 0).r;
            float z = BiasedZ(zReceiver, dz_duv, offset);
            if ( depth < z )
            {
                blockerDepth += depth;
                numBlockers++;
            }
        }

    avgBlockerDepth = blockerDepth / numBlockers;
    
    return numBlockers;
}

float3 PoissonPCFPCSSLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float zEye, float2 lightRadius, bool hwPCF)
{
    // ------------------------
    // blocker search
    // ------------------------
    //float2 dz_duv = DepthGradient(uv, zReceiver);
    float2 dz_duv = 0;
    float2 searchRegionRadiusUV = SearchRegionRadiusUV(lightRadius, zEye);
	float avgBlockerDepth = 0;
    float numBlockers = FindBlockersPoisson(avgBlockerDepth, shadowMap, uv, zReceiver, dz_duv, searchRegionRadiusUV);

    // Early out if no blocker found
    if (numBlockers == 0) return (float3)1.0;

    // ------------------------
    // penumbra size
    // ------------------------
    float avgBlockerDepthWorld = ZClipToZEye(avgBlockerDepth);
    float2 penumbraRadiusUV = PenumbraRadiusUV(lightRadius, zEye, avgBlockerDepthWorld);
    
    // ------------------------
    // filtering
    // ------------------------
    float2 filterRadiusUV = ProjectToLightUV(penumbraRadiusUV, zEye);
    return PoissonPCFShadowLookUp(shadowMap, uv, zReceiver, dz_duv, filterRadiusUV, hwPCF);
}

float3 BoxPCFLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float zEye, float2 lightRadius, bool hwPCF, bool blockerSearch)
{
    //float2 dz_duv = DepthGradient(uv, zReceiver);
    float2 dz_duv = 0;
    float numBlockers = 1;
    
    if (blockerSearch)
    {
		// ------------------------
		// blocker search
		// ------------------------		
		float2 searchRegionRadiusUV = SearchRegionRadiusUV(lightRadius, zEye);
		float avgBlockerDepth = 0;
		numBlockers = FindBlockersBox(avgBlockerDepth, shadowMap, uv, zReceiver, dz_duv, searchRegionRadiusUV);
	}
	
	// Early out if no blocker found
	if (numBlockers == 0) return (float3)1.0;
	
	// ------------------------
    // filtering
    // ------------------------
    float2 filterRadiusUV = lightRadius;
    return BoxPCFShadowLookUp(shadowMap, uv, zReceiver, dz_duv, filterRadiusUV, hwPCF);
}

float3 PoissonPCFLookUp(Texture2D<float4> shadowMap, float2 uv, float zReceiver, float zEye, float2 lightRadius, bool hwPCF, bool blockerSearch)
{
	//float2 dz_duv = DepthGradient(uv, zReceiver);
    float2 dz_duv = 0;
	float numBlockers = 1;
    
    if (blockerSearch)
    {
		// ------------------------
		// blocker search
		// ------------------------
		float2 searchRegionRadiusUV = SearchRegionRadiusUV(lightRadius, zEye);
		float avgBlockerDepth = 0;	    
		numBlockers = FindBlockersPoisson(avgBlockerDepth, shadowMap, uv, zReceiver, dz_duv, searchRegionRadiusUV);
	}
	
	// Early out if no blocker found
	if (numBlockers == 0) return (float3)1.0;
	
	// ------------------------
    // filtering
    // ------------------------
    float2 filterRadiusUV = lightRadius;
    return PoissonPCFShadowLookUp(shadowMap, uv, zReceiver, dz_duv, filterRadiusUV, hwPCF);
}
