
#include "Engine_Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//
//struct tagBoneMatrices
//{
//	matrix		BoneMatrix[256];
//};
//
//tagBoneMatrices		g_BoneMatrices;

Texture2D		g_DiffuseTexture;
Texture2D		g_NormalTexture;

Texture2D		g_MatrixPallete;
Texture2D		g_DissolveTexture;
float			g_fDissolveWeight;


float4			g_vDissolveColor = { 0.6f, 0.039f, 0.039f, 1.f };
float4			g_vRimColor = { 0.f, 0.f, 0.f, 0.f };
float4			g_vCamPosition;



struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

matrix Get_BoneMatrix(VS_IN In)
{
	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix	BoneMatrix =
		matrix(
			g_MatrixPallete.Load(int4((In.vBlendIndex.x - (int(In.vBlendIndex.x / 1000) * 1000)) * 4 + 0, int(In.vBlendIndex.x / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.x - (int(In.vBlendIndex.x / 1000) * 1000)) * 4 + 1, int(In.vBlendIndex.x / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.x - (int(In.vBlendIndex.x / 1000) * 1000)) * 4 + 2, int(In.vBlendIndex.x / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.x - (int(In.vBlendIndex.x / 1000) * 1000)) * 4 + 3, int(In.vBlendIndex.x / 1000), 0, 0))) * In.vBlendWeight.x
		+
		matrix(
			g_MatrixPallete.Load(int4((In.vBlendIndex.y - (int(In.vBlendIndex.y / 1000) * 1000)) * 4 + 0, int(In.vBlendIndex.y / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.y - (int(In.vBlendIndex.y / 1000) * 1000)) * 4 + 1, int(In.vBlendIndex.y / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.y - (int(In.vBlendIndex.y / 1000) * 1000)) * 4 + 2, int(In.vBlendIndex.y / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.y - (int(In.vBlendIndex.y / 1000) * 1000)) * 4 + 3, int(In.vBlendIndex.y / 1000), 0, 0))) * In.vBlendWeight.y

		+
		matrix(
			g_MatrixPallete.Load(int4((In.vBlendIndex.z - (int(In.vBlendIndex.z / 1000) * 1000)) * 4 + 0, int(In.vBlendIndex.z / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.z - (int(In.vBlendIndex.z / 1000) * 1000)) * 4 + 1, int(In.vBlendIndex.z / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.z - (int(In.vBlendIndex.z / 1000) * 1000)) * 4 + 2, int(In.vBlendIndex.z / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.z - (int(In.vBlendIndex.z / 1000) * 1000)) * 4 + 3, int(In.vBlendIndex.z / 1000), 0, 0))) * In.vBlendWeight.z

		+
		matrix(
			g_MatrixPallete.Load(int4((In.vBlendIndex.w - (int(In.vBlendIndex.w / 1000) * 1000)) * 4 + 0, int(In.vBlendIndex.w / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.w - (int(In.vBlendIndex.w / 1000) * 1000)) * 4 + 1, int(In.vBlendIndex.w / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.w - (int(In.vBlendIndex.w / 1000) * 1000)) * 4 + 2, int(In.vBlendIndex.w / 1000), 0, 0)),
			g_MatrixPallete.Load(int4((In.vBlendIndex.w - (int(In.vBlendIndex.w / 1000) * 1000)) * 4 + 3, int(In.vBlendIndex.w / 1000), 0, 0))) * In.vBlendWeight.w;

	return transpose(BoneMatrix);
}



struct VS_OUT
{
	float4		vPosition : SV_POSITION0;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
	float4		vProjPos : TEXCOORD1;
	float4		vWorldPosition : TEXCOORD2;
	
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;


	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float4x4	BoneMatrix = Get_BoneMatrix(In);

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	vector		vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vWorldPosition = mul(vPosition, g_WorldMatrix);
	Out.vWorldPosition = mul(vPosition, g_WorldMatrix);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTangent = normalize(mul(vTangent, g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
	float4		vProjPos : TEXCOORD1;
	float4		vWorldPosition : TEXCOORD2;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(ModelSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.0f);


	float fRimPower = 1.f - saturate(dot(In.vNormal.xyz, normalize((-1.f * (In.vWorldPosition - g_vCamPosition)))));
	fRimPower = pow(fRimPower, 2.f);

	vector vRimColor = g_vRimColor * fRimPower;
	Out.vDiffuse += vRimColor;

	if (0 == Out.vDiffuse.a)
		discard;

	return Out;
}




PS_OUT PS_MAIN_NORMAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	
	Out.vDiffuse = g_DiffuseTexture.Sample(ModelSampler, In.vTexUV);


	vector		vTextureNormal = g_NormalTexture.Sample(ModelSampler, In.vTexUV);

	float3		vNormal = vTextureNormal.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

	
	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.0f);

	

	float fRimPower = 1.f - saturate(dot(vNormal.xyz, normalize((-1.f * (In.vWorldPosition - g_vCamPosition)))));
	fRimPower = pow(fRimPower, 2.f);

	vector vRimColor = g_vRimColor * fRimPower;
	Out.vDiffuse += vRimColor;

	if (0 == Out.vDiffuse.a)
		discard;

	return Out;
}

PS_OUT PS_MAIN_AKAZA(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;


	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);


	vector		vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	float3		vNormal = vTextureNormal.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);


	vNormal = normalize(mul(vNormal, WorldMatrix));

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.0f);



	float fRimPower = 1.f - saturate(dot(vNormal.xyz, normalize((-1.f * (In.vWorldPosition - g_vCamPosition)))));
	fRimPower = pow(fRimPower, 2.f);

	vector vRimColor = g_vRimColor * fRimPower;
	Out.vDiffuse += vRimColor;

	if (0 == Out.vDiffuse.a)
		discard;

	return Out;
}

PS_OUT PS_DISSOLVE_DEAD(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vDissolve = g_DissolveTexture.Sample(ModelSampler, In.vTexUV);

	if (vDissolve.r <= g_fDissolveWeight)
		discard;

	if ((vDissolve.r - g_fDissolveWeight) < 0.1f)
		Out.vDiffuse = g_vDissolveColor;
	else if((vDissolve.r - g_fDissolveWeight) < 0.115f)
		Out.vDiffuse = g_vDissolveColor - 0.1f;
	else if ((vDissolve.r - g_fDissolveWeight) < 0.125f)
		Out.vDiffuse = g_vDissolveColor - 0.1f;
	else
		Out.vDiffuse = g_DiffuseTexture.Sample(ModelSampler, In.vTexUV);

	

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.0f, 0.0f);

	if (0 == Out.vDiffuse.a)
		discard;

	return Out;
}


// ±×¸²ÀÚ ÇÈ¼¿ ½¦ÀÌ´õ.
struct PS_OUT_SHADOW_DEPTH
{
	float4		vDepth : SV_TARGET0;
};


PS_OUT_SHADOW_DEPTH PS_SHADOW_DEPTH(PS_IN In)
{
	PS_OUT_SHADOW_DEPTH Out = (PS_OUT_SHADOW_DEPTH)0;

	vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	if(vColor.a <= 0.01f)
		discard;


	Out.vDepth = vector(In.vProjPos.w / 1000.0f, 0.f, 0.f, 1.f);

	return Out;
}




technique11 DefaultTechnique
{
	pass DefaultPass
	{
		// 0
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass NormalPass
	{
		// 1
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}

	pass DissolveDead
	{
		// 2
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE_DEAD();
	}

	pass Akaza
	{
		// 3
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_AKAZA();
	}

	pass Temp1
	{
		// 4
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Temp2
	{
		// 5
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Temp3
	{
		// 6
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Temp4
	{
		// 6
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Temp5
	{
		// 7
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Temp6
	{
		// 8
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Shadow_Depth
	{
		// 10
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Shadow, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW_DEPTH();
	}
};