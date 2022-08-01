//=============================================================================
// SamplerStates
//=============================================================================

#define MAX_LINEAR_DEPTH 1.e30f
#define MIN_LINEAR_DEPTH 0.0f

// clamped mip map nearest neigbor sampling
SamplerState SampPointClamp
{
    AddressU = Clamp;
    AddressV = Clamp;
    Filter = MIN_MAG_MIP_POINT;
};

SamplerState SampPointBorder
{
    AddressU = Border;
    AddressV = Border;
    Filter = MIN_MAG_MIP_POINT;
    BorderColor = float4(.0f, .0f, .0f, .0f);
};

// clamped bilinear sampling
SamplerState SampBilinearBorder
{
    AddressU = Border;
    AddressV = Border;
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    BorderColor = float4(MAX_LINEAR_DEPTH, .0f, .0f, .0f);
};

SamplerState SampBilinearClamp
{
    AddressU = Clamp;
    AddressV = Clamp;
    Filter = MIN_MAG_LINEAR_MIP_POINT;
};

SamplerState SampAnisotropicBorder
{
    AddressU = Border;
    AddressV = Border;
    Filter = ANISOTROPIC;
    MaxAnisotropy = 16;
    BorderColor = float4(MAX_LINEAR_DEPTH, .0f, .0f, .0f);
};

SamplerState SampAnisotropicClamp
{
    AddressU = Clamp;
    AddressV = Clamp;
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;//16;
};

SamplerState SampTrilinearClamp
{
    AddressU = Clamp;
    AddressV = Clamp;
    Filter = MIN_MAG_MIP_LINEAR;
};

// wrapped mip map nearest neigbor sampling
SamplerState SampPointWrap
{
    AddressU = Wrap;
    AddressV = Wrap;
    Filter = MIN_MAG_MIP_POINT;
};

// wrapped bilinear sampling
SamplerState SampBilinearWrap
{
    AddressU = Wrap;
    AddressV = Wrap;
    Filter = MIN_MAG_LINEAR_MIP_POINT;
};

// wrapped trinlinear sampling
SamplerState SampTrilinearWrap
{
	AddressU = Wrap;
    AddressV = Wrap;
    Filter = MIN_MAG_MIP_LINEAR;
};

// wrapped anisotropic sampling
SamplerState SampAnisotropicWrap
{
    AddressU = Wrap;
    AddressV = Wrap;
    Filter = ANISOTROPIC;
    MaxAnisotropy = 16;
};

// hardware pcf sampling
SamplerComparisonState SampPCF
{
	AddressU = Clamp;
    AddressV = Clamp;
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    ComparisonFunc = LESS_EQUAL;
    //BorderColor = float4(MAX_LINEAR_DEPTH, .0f, .0f, .0f);
};

// simple shadowmap lookup
SamplerComparisonState SampCmp
{
	AddressU = Clamp;
	AddressV = Clamp;
	Filter = COMPARISON_MIN_MAG_MIP_POINT;
	ComparisonFunc = LESS_EQUAL;
	//BorderColor = float4(MAX_LINEAR_DEPTH, .0f, .0f, .0f);
};

//=============================================================================
// DepthStencilStates
//=============================================================================

DepthStencilState DSEnableDepthLessEqual
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSEnableDepthLess
{
    DepthEnable = true;
    DepthFunc = LESS;
    DepthWriteMask = all;
};

DepthStencilState DSNoDepthStencil
{
    DepthEnable = false;
	StencilEnable = false;
};


//=============================================================================
// BlendStates
//=============================================================================

BlendState BSAlphaToCoverage
{
    AlphaToCoverageEnable = true;
    BlendEnable[0] = false;
};

BlendState BSNoBlending
{
    AlphaToCoverageEnable = false;
    BlendEnable[0] = false;
};

BlendState BSSrcAlphaBlendingAdd
{
    AlphaToCoverageEnable = false;
    BlendEnable[0] = true;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState BSSrcAlphaBlendingAddSrcDest
{
    BlendEnable[0] = true;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA ;
    BlendOp = ADD;
    SrcBlendAlpha = SRC_ALPHA;
    DestBlendAlpha = DEST_ALPHA;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

//=============================================================================
// RasterizerStates
//=============================================================================

RasterizerState RSSolidFillCullFront
{
	FrontCounterClockwise = true;
	CullMode = Front;
	FillMode = Solid;
	DepthBias = 0.0;
	SlopeScaledDepthBias = 0.0;
	MultisampleEnable = true;
	AntialiasedLineEnable = true;
};

RasterizerState RSSolidFillCullBack
{
	FrontCounterClockwise = true;
	CullMode = Back;
	FillMode = Solid;
	DepthBias = 0.0;
	SlopeScaledDepthBias = 0.0;
	MultisampleEnable = true;
	AntialiasedLineEnable = true;
};

RasterizerState RSSolidFillCullNone
{
    FillMode = Solid;
    CullMode = None;
    DepthBias = 0.0;
    SlopeScaledDepthBias = 0.0;
	MultisampleEnable = true;
	AntialiasedLineEnable = true;
};

RasterizerState RSLightRenderDepth
{
	FrontCounterClockwise = true;
    CullMode = Back;
    FillMode = Solid;
    MultisampleEnable = false;
    AntialiasedLineEnable = false;
    DepthBias = 3.0e3;
    SlopeScaledDepthBias = 4.0;
};

RasterizerState RSLightRenderSATDepth
{
	FrontCounterClockwise = true;
    CullMode = Back;
    FillMode = Solid;
    MultisampleEnable = false;
    AntialiasedLineEnable = false;
    DepthBias = 0.0;
    SlopeScaledDepthBias = 1.0;
};
