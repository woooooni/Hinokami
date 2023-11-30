#include "Engine_Shader_Defines.hpp"


matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D	g_DiffuseTexture;
Texture2D	g_AlphaTexture;
float g_fBlurPower = 0.f;


struct EffectDesc
{
	int			g_iCutUV;
	float		g_fMaxCountX;
	float		g_fMaxCountY;
	float		g_fAlpha;
	float2		g_fUVIndex;
	float2		g_fUVFlow;

	float4		g_fAdditiveDiffuseColor;
	float4		g_vBloomPower;
};
EffectDesc g_EffectDesc[1000];




struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;

	uint	iInstanceID : SV_INSTANCEID;

};




struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;

	uint	iInstanceID : SV_INSTANCEID;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	float4x4	InstanceWorld = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	Out.vPosition = mul(float4(In.vPosition, 1.f), InstanceWorld);
	Out.vPosition = mul(Out.vPosition, matVP);
	
	Out.vTexUV = float2(
		((g_EffectDesc[In.iInstanceID].g_fUVIndex.x + In.vTexUV.x) / g_EffectDesc[In.iInstanceID].g_fMaxCountX) + g_EffectDesc[In.iInstanceID].g_fUVFlow.x,
		((g_EffectDesc[In.iInstanceID].g_fUVIndex.y + In.vTexUV.y) / g_EffectDesc[In.iInstanceID].g_fMaxCountY) + g_EffectDesc[In.iInstanceID].g_fUVFlow.y);

	Out.iInstanceID = In.iInstanceID;

	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;

	uint	iInstanceID : SV_INSTANCEID;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vBlurPower : SV_TARGET1;
	float4      vBrightness : SV_TARGET2;
};



float4 CalcBrightness(float4 vColor, uint iInstanceID)
{
	float BrightColor = 0.f;

	float brightness = dot(vColor.rgb, g_EffectDesc[iInstanceID].g_vBloomPower.rgb);
	if (brightness > 0.99f)
		BrightColor = float4(vColor.rgb, 1.0f);

	return BrightColor;
}



PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_EffectDesc[In.iInstanceID].g_iCutUV)
	{
		if (In.vTexUV.x > 1.f && In.vTexUV.y > 1.f)
			discard;
	}

	Out.vDiffuse = vector(g_EffectDesc[In.iInstanceID].g_fAdditiveDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);
	if (0 == Out.vDiffuse.a)
		discard;

	Out.vBlurPower = vector(g_fBlurPower / 100.f, 0.f, 0.f, 1.f);
	Out.vBrightness = CalcBrightness(Out.vDiffuse, In.iInstanceID);
	
	return Out;

};

PS_OUT PS_NO_DIFFUSE_WITH_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_EffectDesc[In.iInstanceID].g_iCutUV)
	{
		if (In.vTexUV.x > 1.f && In.vTexUV.y > 1.f)
			discard;
	}

	vector vTextureAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);

	if (vTextureAlpha.r <= 0.001f)
		discard;

	Out.vDiffuse = vector(g_EffectDesc[In.iInstanceID].g_fAdditiveDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);

	if (0 == Out.vDiffuse.a)
		discard;

	Out.vBlurPower = vector(g_fBlurPower / 100.f, 0.f, 0.f, 1.f);
	Out.vBrightness = CalcBrightness(Out.vDiffuse, In.iInstanceID);
	return Out;

};

PS_OUT PS_NO_ALPHA_WITH_DIFFUSE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_EffectDesc[In.iInstanceID].g_iCutUV)
	{
		if (In.vTexUV.x > 1.f && In.vTexUV.y > 1.f)
			discard;
	}

	vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vAdditiveColor = vector(g_EffectDesc[In.iInstanceID].g_fAdditiveDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);


	if (vTextureDiffuse.r <= 0.01f)
		discard;

	vTextureDiffuse.a = 0.f;
	vector vDiffuseColor = vTextureDiffuse + vAdditiveColor;
	saturate(vDiffuseColor);

	vector vMtrlColor = vector(vDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);
	Out.vDiffuse = vMtrlColor;

	if (0 == Out.vDiffuse.a)
		discard;

	Out.vBlurPower = vector(g_fBlurPower / 100.f, 0.f, 0.f, 1.f);
	Out.vBrightness = CalcBrightness(Out.vDiffuse, In.iInstanceID);

	return Out;

};

PS_OUT PS_BOTH(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_EffectDesc[In.iInstanceID].g_iCutUV)
	{
		if (In.vTexUV.x > 1.f && In.vTexUV.y > 1.f)
			discard;
	}

	vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vTextureAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
	vector vAdditiveColor = vector(g_EffectDesc[In.iInstanceID].g_fAdditiveDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);


	if (vTextureAlpha.r <= 0.1f)
		discard;

	vector vDiffuseColor = vTextureDiffuse + vAdditiveColor;

	saturate(vDiffuseColor);
	vDiffuseColor.a = 0.f;

	vector vMtrlColor = vector(vDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);
	Out.vDiffuse = vMtrlColor;

	if (0 == Out.vDiffuse.a)
		discard;

	Out.vBlurPower = vector(g_fBlurPower / 100.f, 0.f, 0.f, 1.f);
	Out.vBrightness = CalcBrightness(Out.vDiffuse, In.iInstanceID);

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