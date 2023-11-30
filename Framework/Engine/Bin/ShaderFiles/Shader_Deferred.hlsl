#include "Engine_Shader_Defines.hpp"


texture2D		g_Texture; // 디버그용 텍스쳐

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv;
matrix			g_ViewMatrixInv;
matrix			g_CamProjMatrix;
matrix			g_LightViewMatrix;

texture2D		g_DiffuseTarget;
texture2D		g_NormalTarget;
texture2D		g_ShadeTarget;
texture2D		g_SpecularTarget;
texture2D		g_DepthTarget;

texture2D		g_ShadowTarget;


texture2D		g_OriginEffectTarget;
texture2D		g_OriginBloomTarget;

texture2D		g_BlurBloomTarget;
texture2D		g_BlurEffectTarget;

texture2D		g_BlurTarget;
texture2D		g_BlurPowerTarget;
texture2D		g_UITarget;



vector g_vCamPosition;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector	g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector	g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

// 안개
float2 g_vFogStartEnd = { 1.f, 5.f };
float4 g_vFogColor = { .5f, .5f, .5f, 1.f };
float g_fBias = 0.2f;



struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{	
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float		fFogFactor : FOG;
};



VS_OUT VS_MAIN(/* 정점 */VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};


struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	return Out;
}

struct PS_OUT_LIGHT
{
	float4	vShade : SV_TARGET0;	
	float4	vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTarget.Sample(PointSampler, In.vTexcoord);
	vector		vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * 1000.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);

	vector		vWorldPos;

	/* 투영스페이스 상의 위치를 구한다. */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구한다. */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드까지 가자. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 20.f);
	return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTarget.Sample(PointSampler, In.vTexcoord);
	vector		vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * 1000.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;

	/* 투영스페이스 상의 위치를 구한다. */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구한다. */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드까지 가자. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

	Out.vShade = fAtt * g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));

	vector		vReflect = reflect(normalize(vLightDir), vNormal);

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = fAtt * (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 50.f);


	return Out;
}


float PCF_ShadowCaculation(float4 vLightPos, float3 vLightDir)
{
	float3 projCoords = vLightPos.xyz / vLightPos.w;
	
	projCoords.x = projCoords.x * 0.5f + 0.5f;
	projCoords.y = projCoords.y * -0.5f + 0.5f;
	
	float fCurrentDepth = projCoords.z;
	if (fCurrentDepth >= 1.f)
		return 1.f;

	float fShadow = 0.0f;
	float2 texelSize = float2(1.f / 1600.f, 1.f / 900.f);
	texelSize /= 10.f;


	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float fPCFDepth = g_ShadowTarget.Sample(PointSampler, projCoords.xy + (float2(x, y) * texelSize)).r;
			fShadow += vLightPos.w - g_fBias > fPCFDepth * 1000.f ? 0.5f : 1.0f;
		}
	}
	fShadow /= 9.0f;
	return fShadow;

}


float3x3 Kx = { -1.f, 0.f, 1.f,
				-2.f, 0.f, 2.f,
				-1.f, 0.f, 1.f };

float3x3 Ky = { 1.f,  2.f,  1.f,
				0.f,  0.f,  0.f,
			   -1.f, -2.f, -1.f };

float2 g_PixelOffset = float2(1.f / 1600.f, 1.f / 900.f);
float3 g_PixelDot = float3(.4f, .2f, 0.f);

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTarget.Sample(LinearSampler, In.vTexcoord);
	if (vDiffuse.a == 0.f)
		discard;

	// 툰 쉐이딩
	vector		vToonShade = g_ShadeTarget.Sample(LinearSampler, In.vTexcoord);
	vToonShade = saturate(vToonShade);
	vToonShade = ceil(vToonShade * 2.f) / 2.f;
	

	// 빛 연산 먹이기
	/*vector		vSpecular = g_SpecularTarget.Sample(LinearSampler, In.vTexcoord);*/

	vDiffuse = vDiffuse * vToonShade;
	Out.vColor = vDiffuse;



	//// 외곽선
	float Lx = 0;
	float Ly = 0;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x, y) * g_PixelOffset;
			float3 tex = g_ShadeTarget.Sample(PointSampler, In.vTexcoord + offset).rgb;
			float luminance = dot(tex, g_PixelDot);

			Lx += luminance * Kx[y + 1][x + 1];
			Ly += luminance * Ky[y + 1][x + 1];
		}
	}
	float L = sqrt((Lx * Lx) + (Ly * Ly));
	vector vOutline = vector(1.f - L.xxx, 1.f);
	Out.vColor *= vOutline;


	// 그림자
	vector		vWorldPos;
	vector		vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * 1000.f;

	/* 투영스페이스 상의 위치를 구한다. */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구한다. */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드까지 가자. */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
	
	vector	vLightPos = mul(vWorldPos, g_LightViewMatrix);
	float3 vLightDir = normalize(float3(0.f, 0.f, 0.f) - vLightPos.xyz);

	vLightPos = mul(vLightPos, g_CamProjMatrix);

	float fShadowColor = PCF_ShadowCaculation(vLightPos, vLightDir);
	vector vShadowColor = vector(fShadowColor, fShadowColor, fShadowColor, 1.f);
	Out.vColor *= vShadowColor;


	



	// 안개
	float fFogFactor = saturate(((g_vFogStartEnd.y - (fViewZ)) / (g_vFogStartEnd.y - g_vFogStartEnd.x)));
	Out.vColor = fFogFactor * Out.vColor + (1.f - fFogFactor) * g_vFogColor;
	

	return Out;
}




PS_OUT PS_BLUR_DOWNSCALE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_BlurTarget.Sample(LinearSampler, In.vTexcoord);
	return Out;
}


float2 g_PixleSize = float2(1.f / (1600.f / 2.f), 1.f / (900.f / 2.f));


PS_OUT PS_BLUR_Horizontal(PS_IN In) : SV_TARGET0
{
	PS_OUT Out = (PS_OUT)0;
	float fWeights[5] = { 1.f, 0.9f, 0.55f, 0.18f, 0.1f };
	float fBlurPower = g_BlurPowerTarget.Sample(PointSampler, In.vTexcoord).r * 100.f;
	float fNormalization = (fWeights[0] + 2.0f * (fWeights[1] + fWeights[2] + fWeights[3] + fWeights[4]));

	for (int i = 0; i < 5; ++i)
	{
		fWeights[i] /= fNormalization;
	}

	vector vFinalColor = float4(0.f, 0.f, 0.f, 0.f);
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * -4.f, 0.f))) * fWeights[4];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * -3.f, 0.f))) * fWeights[3];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * -2.f, 0.f))) * fWeights[2];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * -1.f, 0.f))) * fWeights[1];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * 0.f, 0.f))) * fWeights[0];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * 1.f, 0.f))) * fWeights[1];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * 2.f, 0.f))) * fWeights[2];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * 3.f, 0.f))) * fWeights[3];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(g_PixleSize.x * 4.f, 0.f))) * fWeights[4];

	Out.vColor = vFinalColor;
	return Out;
}

PS_OUT PS_BLUR_Vertical(PS_IN In) : SV_TARGET0
{
	PS_OUT Out = (PS_OUT)0;
	float fWeights[5] = { 1.f, 0.9f, 0.55f, 0.18f, 0.1f };
	float fBlurPower = g_BlurPowerTarget.Sample(PointSampler, In.vTexcoord).r * 100.f;
	float fNormalization = (fWeights[0] + 2.f * (fWeights[1] + fWeights[2] + fWeights[3] + fWeights[4]));

	for (int i = 0; i < 5; ++i)
	{
		fWeights[i] /= fNormalization;
	}

	vector vFinalColor = float4(0.f, 0.f, 0.f, 0.f);
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * -4.f))) * fWeights[4];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * -3.f))) * fWeights[3];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * -2.f))) * fWeights[2];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * -1.f))) * fWeights[1];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * 0.f))) * fWeights[0];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * 1.f))) * fWeights[1];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * 2.f))) * fWeights[2];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * 3.f))) * fWeights[3];
	vFinalColor += g_BlurTarget.Sample(PointSampler, (In.vTexcoord + float2(0.f, g_PixleSize.y * 4.f))) * fWeights[4];


	Out.vColor = vFinalColor;

	return Out;
}


PS_OUT PS_BLUR_UPSCALE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	Out.vColor = g_BlurTarget.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vOriginEffectColor = g_OriginEffectTarget.Sample(PointSampler, In.vTexcoord);
	vector vOriginBloomColor = g_OriginBloomTarget.Sample(PointSampler, In.vTexcoord);
	vector vBlurBloomColor = g_BlurBloomTarget.Sample(PointSampler, In.vTexcoord);
	vector vBlurEffectColor = g_BlurEffectTarget.Sample(PointSampler, In.vTexcoord);


	Out.vColor = g_DiffuseTarget.Sample(LinearSampler, In.vTexcoord);
	Out.vColor += vOriginEffectColor;
	Out.vColor += vOriginBloomColor;
	Out.vColor += vBlurBloomColor;
	Out.vColor += vBlurEffectColor;
	// Out.vColor += g_UITarget.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

technique11 DefaultTechnique
{
	pass Target_Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Deferred
	{
		// 3
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL; 
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
	}

	pass Bloom
	{
		// 4
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass BlurDownScale
	{
		// 5
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR_DOWNSCALE();
	}

	pass Blur_Horizontal
	{
		// 6
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR_Horizontal();
	}

	pass Blur_Vertical
	{
		// 7
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR_Vertical();
	}

	pass BlurUpScale
	{
		// 8
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR_UPSCALE();
	}

	pass Render_Final
	{
		// 9
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}
	
}




