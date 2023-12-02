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
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;

	uint	iInstanceID : SV_INSTANCEID;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
	uint		iInstanceID : SV_INSTANCEID;

};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float4x4 InstanceWorld = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), InstanceWorld);
	Out.vPosition = mul(Out.vPosition, matVP);

	EffectDesc Desc = g_EffectDesc[In.iInstanceID];
	Out.vTexUV = float2(
		((Desc.g_fUVIndex.x + In.vTexUV.x) / Desc.g_fMaxCountX) + Desc.g_fUVFlow.x,
		((Desc.g_fUVIndex.y + In.vTexUV.y) / Desc.g_fMaxCountY) + Desc.g_fUVFlow.y);

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), InstanceWorld)).xyz;
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
	Out.vProjPos = Out.vPosition;
	Out.iInstanceID = In.iInstanceID;
	return Out;
}


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
	uint		iInstanceID : SV_INSTANCEID;
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

	float brightness = dot(vColor, g_EffectDesc[iInstanceID].g_vBloomPower);

	if (brightness > 0.99f)
	{
		BrightColor = float4(vColor.rgb, 1.0f);
	}
		

	return BrightColor;
}

PS_OUT PS_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_EffectDesc[In.iInstanceID].g_iCutUV)
	{
		if ((In.vTexUV.x > 1.f) || (In.vTexUV.y > 1.f) || (In.vTexUV.x < 0.f) || (In.vTexUV.y < 0.f))
			discard;
	}

	Out.vDiffuse = vector(g_EffectDesc[In.iInstanceID].g_fAdditiveDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);
	if (0 == Out.vDiffuse.a)
		discard;

	Out.vBrightness = CalcBrightness(Out.vDiffuse, In.iInstanceID);
	Out.vBlurPower = vector(g_fBlurPower / 100.f, 0.f, 0.f, 1.f);
	return Out;

};


PS_OUT PS_NO_ALPHA_WITH_DIFFUSE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_EffectDesc[In.iInstanceID].g_iCutUV)
	{
		if ((In.vTexUV.x > 1.f) || (In.vTexUV.y > 1.f) || (In.vTexUV.x < 0.f) || (In.vTexUV.y < 0.f))
			discard;
	}

	vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vAdditiveColor = vector(g_EffectDesc[In.iInstanceID].g_fAdditiveDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);


	if ((vTextureDiffuse.r <= 0.01f) && (vTextureDiffuse.g <= 0.01f) && (vTextureDiffuse.b <= 0.01f))
		discard;

	vTextureDiffuse.a = 0.f;
	vector vDiffuseColor = vTextureDiffuse + vAdditiveColor;
	saturate(vDiffuseColor);

	vector vMtrlColor = vector(vDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);
	Out.vDiffuse = vMtrlColor;

	if (0 == Out.vDiffuse.a)
		discard;

	Out.vBrightness = CalcBrightness(Out.vDiffuse, In.iInstanceID);
	Out.vBlurPower = vector(g_fBlurPower / 100.f, 0.f, 0.f, 1.f);
	return Out;

};

PS_OUT PS_NO_DIFFUSE_WITH_ALPHA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_EffectDesc[In.iInstanceID].g_iCutUV)
	{
		if ((In.vTexUV.x > 1.f) || (In.vTexUV.y > 1.f) || (In.vTexUV.x < 0.f) || (In.vTexUV.y < 0.f))
			discard;
	}

	vector vTextureAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);

	if ((vTextureAlpha.r <= 0.01f) && (vTextureAlpha.g <= 0.01f) && (vTextureAlpha.b <= 0.01f))
		discard;

	Out.vDiffuse = vector(g_EffectDesc[In.iInstanceID].g_fAdditiveDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);

	if (0 == Out.vDiffuse.a)
		discard;

	Out.vBrightness = CalcBrightness(Out.vDiffuse, In.iInstanceID);
	Out.vBlurPower = vector(g_fBlurPower / 100.f, 0.f, 0.f, 1.f);
	return Out;

};


PS_OUT PS_BOTH(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (0 < g_EffectDesc[In.iInstanceID].g_iCutUV)
	{
		if ((In.vTexUV.x > 1.f) || (In.vTexUV.y > 1.f) || (In.vTexUV.x < 0.f) || (In.vTexUV.y < 0.f))
			discard;
	}

	vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector vTextureAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
	vector vAdditiveColor = vector(g_EffectDesc[In.iInstanceID].g_fAdditiveDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);


	if (vTextureAlpha.r <= 0.1f && vTextureAlpha.g <= 0.1f && vTextureAlpha.b <= 0.1f)
		discard;

	vector vDiffuseColor = vTextureDiffuse + vAdditiveColor;

	saturate(vDiffuseColor);
	vDiffuseColor.a = 0.f;

	vector vMtrlColor = vector(vDiffuseColor.rgb, g_EffectDesc[In.iInstanceID].g_fAlpha);
	Out.vDiffuse = vMtrlColor;

	if (0 == Out.vDiffuse.a)
		discard;

	Out.vBrightness = CalcBrightness(Out.vDiffuse, In.iInstanceID);
	Out.vBlurPower = vector(g_fBlurPower / 100.f, 0.f, 0.f, 1.f);
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