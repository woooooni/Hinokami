#include "Engine_Shader_Defines.hpp"


matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D	g_DiffuseTexture;
Texture2D	g_AlphaTexture;


float		g_fMaxCountX;
float		g_fMaxCountY;
float		g_fAlpha;
float2		g_fUVIndex;
float2		g_fUVFlow;
float3		g_fAdditiveDiffuseColor;

int			g_iCutUV;




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

	matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = float2(
		((g_fUVIndex.x + In.vTexUV.x) / g_fMaxCountX) + g_fUVFlow.x,
		((g_fUVIndex.y + In.vTexUV.y) / g_fMaxCountY) + g_fUVFlow.y);

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
};


PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_iCutUV)
	{
		if (In.vTexUV.x > 1.f && In.vTexUV.y > 1.f)
			discard;
	}

	Out.vDiffuse = vector(g_fAdditiveDiffuseColor, g_fAlpha);
	if (0 == Out.vDiffuse.a)
		discard;

	return Out;

};

PS_OUT PS_NO_DIFFUSE_WITH_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_iCutUV)
	{
		if (In.vTexUV.x > 1.f && In.vTexUV.y > 1.f)
			discard;
	}

	vector vTextureAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);

	if (vTextureAlpha.r <= 0.001f)
		discard;

	Out.vDiffuse = vector(g_fAdditiveDiffuseColor, g_fAlpha);

	if (0 == Out.vDiffuse.a)
		discard;

	return Out;

};

PS_OUT PS_NO_ALPHA_WITH_DIFFUSE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_iCutUV)
	{
		if (In.vTexUV.x > 1.f && In.vTexUV.y > 1.f)
			discard;
	}

	vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vAdditiveColor = vector(g_fAdditiveDiffuseColor, g_fAlpha);


	if (vTextureDiffuse.r <= 0.01f)
		discard;

	vTextureDiffuse.a = 0.f;
	vector vDiffuseColor = vTextureDiffuse + vAdditiveColor;
	saturate(vDiffuseColor);

	vector vMtrlColor = vector(vDiffuseColor.rgb, g_fAlpha);
	Out.vDiffuse = vMtrlColor;

	if (0 == Out.vDiffuse.a)
		discard;

	return Out;

};

PS_OUT PS_BOTH(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_iCutUV)
	{
		if (In.vTexUV.x > 1.f && In.vTexUV.y > 1.f)
			discard;
	}

	vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vTextureAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
	vector vAdditiveColor = vector(g_fAdditiveDiffuseColor, g_fAlpha);


	if (vTextureAlpha.r <= 0.1f)
		discard;

	vector vDiffuseColor = vTextureDiffuse + vAdditiveColor;

	saturate(vDiffuseColor);
	vDiffuseColor.a = 0.f;

	vector vMtrlColor = vector(vDiffuseColor.rgb, g_fAlpha);
	Out.vDiffuse = vMtrlColor;

	if (0 == Out.vDiffuse.a)
		discard;

	return Out;

};

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_NoneCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DEFAULT();
	}


	pass NoAlphaPass
	{
		SetRasterizerState(RS_NoneCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NO_ALPHA_WITH_DIFFUSE();
	}

	pass NoDiffusePass
	{
		SetRasterizerState(RS_NoneCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NO_DIFFUSE_WITH_ALPHA();
	}


	pass BothPass
	{
		SetRasterizerState(RS_NoneCull);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BOTH();
	}

}