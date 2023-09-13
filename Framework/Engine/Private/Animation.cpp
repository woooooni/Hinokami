#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "Utils.h"
CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
	, m_bPause(false)
	, m_strAnimationName(rhs.m_strAnimationName)
	, m_iFrameCount(rhs.m_iFrameCount)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize_Prototype(aiAnimation* pAIAnimation, _fmatrix PivotMatrix)
{
	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_iFrameCount = pAIAnimation->mDuration;


	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* aiNodeAnim : mChannel은 키프레임 정보들을 가지낟. */
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		/* 왜 모아두는데?> 특정 애님에ㅣ션 상태일때 애니메이션을 재생하면 모든 뼈의 상태를 갱신하는건 빡세. 느려. 
		현재 애미에시연을 구동하기위한 뼈대만 상태 갱신해주기 위해. */
		m_Channels.push_back(pChannel);
	}

	m_strAnimationName = CUtils::GetInstance()->string_to_wstring(string(pAIAnimation->mName.C_Str()));

	return S_OK;
}

HRESULT CAnimation::Initialize(CModel* pModel, ID3D11Device* pDevice)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);

		CHierarchyNode* pNode = pModel->Get_HierarchyNode(m_Channels[i]->Get_Name());

		if (nullptr == pNode)
			return E_FAIL;

		m_HierarchyNodes.push_back(pNode);

		Safe_AddRef(pNode);
	}

	Create_VTF_Texture(pDevice);
	

	return S_OK;
}


HRESULT CAnimation::Play_Animation(_float fTimeDelta)
{

	if (m_bPause == false)
		m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	

	if (m_fPlayTime >= m_fDuration)
	{
		m_fPlayTime = 0.f;

		for (auto& pChannel : m_Channels)
		{
			for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
				iCurrentKeyFrame = 0;			
		}
	}


	/* 이 애니메이션 구동을 위한 모든 뼈들을 순회하며 뼈들의 행렬을 갱신해준다. */
	/* Transformation : 전달된 시간에 따른 키프레임(시간, 스케일, 회전, 이동)정보를 이용하여 Transformation을 만든다. */
	/* 하이어라키 노드에 저장해준다. */
	_uint iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex]);
		++iChannelIndex;
	}



	return S_OK;
}

HRESULT CAnimation::Create_VTF_Texture(ID3D11Device* pDevice)
{

	_uint iChannelIndex = 0;
	vector<_float4x4> MatricesTemp;
	MatricesTemp.resize(m_Channels.size() * 100);
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_Transformation(iChannelIndex, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex]);

		XMStoreFloat4x4(&MatricesTemp[iChannelIndex], XMMatrixTranspose(m_HierarchyNodes[iChannelIndex]->Get_OffSetMatrix() * m_HierarchyNodes[iChannelIndex]->Get_CombinedTransformation() * XMLoadFloat4x4(&m_PivotMatrix)));
		++iChannelIndex;
	}

	const _uint iLineSize = iChannelIndex * sizeof(_float4x4);
	const _uint iPageSize = iLineSize * m_iFrameCount;
	void* pFullData = nullptr;
	pFullData = ::malloc(iPageSize);

	_uint iStartOffset = 0;
	for (_uint i = 0; i < m_iFrameCount; ++i)
	{
		BYTE* iStart = reinterpret_cast<BYTE*>(pFullData) + iStartOffset;
		void* pData = iStart;
		memcpy(pData, &MatricesTemp[i], iLineSize);
		iStartOffset += iLineSize;
	}

	ID3D11Texture2D* pTexture;
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iChannelIndex * 4;
	TextureDesc.Height = m_iFrameCount;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;



	D3D11_SUBRESOURCE_DATA Subresource;
	ZeroMemory(&Subresource, sizeof(D3D11_SUBRESOURCE_DATA));

	Subresource.pSysMem = pFullData;
	Subresource.SysMemPitch = MatricesTemp.size() * sizeof(_float4x4);

	if (FAILED(pDevice->CreateTexture2D(&TextureDesc, &Subresource, &pTexture)))
		return E_FAIL;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(pDevice->CreateShaderResourceView(pTexture, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, _fmatrix PivotMatrix)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(pAIAnimation, PivotMatrix)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone(CModel* pModel, ID3D11Device* pDevice)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pModel, pDevice)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();


	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();
}
