#include "Engine_Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;


float g_fAlpha = 1.f;
float4	g_vDiffuseColor = { 0.1f, 0.1f, 0.1f, 1.f };

Texture2D	g_DiffuseTexture;
Texture2D	g_AlphaTexture;
Texture2D	g_DepthTexture;


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.2f >= Out.vColor.a)
		discard;

	return Out;	
}

PS_OUT PS_OPACTITY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_REVERSE_OPACTITY(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vAlphaColor = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);

	if (vAlphaColor.r >= 0.2f)
		discard;

	Out.vColor = vector(vAlphaColor.rgb, 1.f - vAlphaColor.r);

	return Out;
}

PS_OUT PS_USING_ALPHA_VALUE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vTextureColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	if (vTextureColor.a == 0.f || g_fAlpha == 0.f)
		discard;

	vTextureColor.a = g_fAlpha;
	Out.vColor = vTextureColor;

	return Out;
	
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass OpacityPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OPACTITY();
	}

	pass ReverseOpacityPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_REVERSE_OPACTITY();
	}

	pass UsingAlphaValue
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_USING_ALPHA_VALUE();
	}
	
}