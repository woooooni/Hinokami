
sampler LinearSampler = sampler_state{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler ModelSampler = sampler_state{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = clamp;
	AddressV = clamp;
};


sampler PointSampler = sampler_state{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};


sampler ShadowSampler = sampler_state{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};




RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_NoneCull
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
	FillMode = solid;

	/* �ո��� �ø��ϰڴ�. == �ĸ��� �����ְڴ�. */
	CullMode = Back;

	/* �ո��� �ð�������� ���ڴ�. */
	FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = LESS_EQUAL;
};


DepthStencilState DSS_Shadow
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_Sky
{
	DepthEnable = false;
	DepthWriteMask = zero;
};


DepthStencilState DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState BS_OneBlend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;

};


