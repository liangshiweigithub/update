
//=============================================================================
// Macros
//=============================================================================

#ifdef NBROFSPLITS
	#if NBROFSPLITS > 2
	#define NUMSPLITS NBROFSPLITS
	#else
	#define NUMSPLITS 2
	#endif
#else
#define NUMSPLITS 2
#endif

#if NUMSPLITS < 5
#define UV UV4
#define FACTOR 1.0/2.0
#elif NUMSPLITS < 10
#define UV UV9
#define FACTOR 1.0/3.0
#else
#define UV UV16
#define FACTOR 1.0/4.0
#endif

//=============================================================================
// Constants
//=============================================================================

static const float4 UV4[4] = 
{
	float4(.0f, .0f,  .5f,  .5f),
	float4(.5f, .0f, 1.0f,  .5f),
	float4(.0f, .5f,  .5f, 1.0f),
	float4(.5f, .5f, 1.0f, 1.0f)
};

static const float4 UV9[9] = 
{
	float4(      .0f,       .0f, 1.0f/3.0f, 1.0f/3.0f),
	float4(1.0f/3.0f,       .0f, 2.0f/3.0f, 1.0f/3.0f),
	float4(2.0f/3.0f,       .0f,      1.0f, 1.0f/3.0f),
	float4(      .0f, 1.0f/3.0f, 1.0f/3.0f, 2.0f/3.0f),
	float4(1.0f/3.0f, 1.0f/3.0f, 2.0f/3.0f, 2.0f/3.0f),
	float4(2.0f/3.0f, 1.0f/3.0f,      1.0f, 2.0f/3.0f),
	float4(      .0f, 2.0f/3.0f, 1.0f/3.0f,      1.0f),
	float4(1.0f/3.0f, 2.0f/3.0f, 2.0f/3.0f,      1.0f),
	float4(2.0f/3.0f, 2.0f/3.0f,      1.0f,      1.0f)
};

static const float4 UV16[16] = 
{
	float4( .0f,  .0f, .25f, .25f),
	float4(.25f,  .0f,  .5f, .25f),
	float4( .5f,  .0f, .75f, .25f),
	float4(.75f,  .0f, 1.0f, .25f),
	float4( .0f, .25f, .25f,  .5f),
	float4(.25f, .25f,  .5f,  .5f),
	float4( .5f, .25f, .75f,  .5f),
	float4(.75f, .25f, 1.0f,  .5f),
	float4( .0f,  .5f, .25f, .75f),
	float4(.25f,  .5f,  .5f, .75f),
	float4( .5f,  .5f, .75f, .75f),
	float4(.75f,  .5f, 1.0f, .75f),
	float4( .0f, .75f, .25f, 1.0f),
	float4(.25f, .75f,  .5f, 1.0f),
	float4( .5f, .75f, .75f, 1.0f),
	float4(.75f, .75f, 1.0f, 1.0f)
};

static const float3 SplitColors[4] =
{
	float3(1.0f,  .0f,  .0f),
	float3(1.0f, 1.0f,  .0f),
	float3( .0f, 1.0f,  .0f),
	float3( .0f,  .0f, 1.0f)
};
