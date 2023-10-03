#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "Shader.h"
#include "Utils.h"
#include "GameInstance.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
	, m_strName(rhs.m_strName)
	/*, m_pSRV(rhs.m_pSRV)*/
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	/*if(nullptr != m_pSRV)
		Safe_AddRef(m_pSRV);*/
}

HRESULT CAnimation::Initialize_Prototype(aiAnimation* pAIAnimation)
{
	m_strName = CUtils::ToWString(pAIAnimation->mName.C_Str());
	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

	/* ���� �ִϸ��̼ǿ��� �����ؾ��� ������ ������ �����Ѵ�. */
	m_iNumChannels = pAIAnimation->mNumChannels;


	/* ���� �ִϸ��̼ǿ��� �����ؾ��� ���������� �����Ͽ� �����Ѵ�. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* aiNodeAnim : mChannel�� Ű������ �������� ������. */
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		/* �� ��Ƶδµ�?> Ư�� �ִԿ��Ӽ� �����϶� �ִϸ��̼��� ����ϸ� ��� ���� ���¸� �����ϴ°� ����. ����. 
		���� �ֹ̿��ÿ��� �����ϱ����� ���븸 ���� �������ֱ� ����. */
		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(CModel* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);
		m_ChannelOldKeyFrames.push_back(0);

		CHierarchyNode* pNode = pModel->Get_HierarchyNode(m_Channels[i]->Get_Name());

		if (nullptr == pNode)
			return E_FAIL;

		m_HierarchyNodes.push_back(pNode);

		Safe_AddRef(pNode);
	}

	/*if (FAILED(Ready_AnimationTexture()))
		return E_FAIL;*/

	return S_OK;
}

void CAnimation::Reset_Animation()
{
	m_fPlayTime = 0.f; 
	m_bPause = false;

	for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
		iCurrentKeyFrame = 0;
}

HRESULT CAnimation::Play_Animation(_float fTimeDelta)
{
	m_fPlayTime += m_fSpeed * m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime >= m_fDuration)
	{
		m_fPlayTime = 0.f;

		for (auto& pChannel : m_Channels)
		{
			for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
				iCurrentKeyFrame = 0;
		}
	}

	_uint		iChannelIndex = 0;

	/* �� �ִϸ��̼� ������ ���� ��� ������ ��ȸ�ϸ� ������ ����� �������ش�. */
	/* Transformation : ���޵� �ð��� ���� Ű������(�ð�, ������, ȸ��, �̵�)������ �̿��Ͽ� Transformation�� �����. */
	/* ���̾��Ű ��忡 �������ش�. */
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex], nullptr);
		m_ChannelOldKeyFrames[iChannelIndex] = m_ChannelKeyFrames[iChannelIndex];
		++iChannelIndex;
	}

	return S_OK;
}

HRESULT CAnimation::Play_Animation(CModel* pModel, CAnimation* pNextAnimation, _float fTimeDelta)
{
	if (pModel->Is_InterpolatingAnimation())
	{
		_uint		iChannelIndex = 0;
		for (auto& pChannel : m_Channels)
		{
			pChannel->Interpolation(m_fPlayTime, fTimeDelta, pNextAnimation, m_ChannelOldKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex], pModel);
			++iChannelIndex;
		}
	}



	return S_OK;
}

const list<KEYFRAME> CAnimation::Get_Curr_KeyFrames()
{
	list<KEYFRAME> Frames;

	for (auto& Channel : m_Channels)
		Frames.push_back(Channel->Get_CurrFrame());

	return Frames;
}

const list<KEYFRAME> CAnimation::Get_First_KeyFrames()
{
	list<KEYFRAME> Frames;

	for (auto& Channel : m_Channels)
		Frames.push_back(Channel->Get_FirstFrame());

	return Frames;
}

CChannel* CAnimation::Get_Channel(const wstring& strChannelName)
{
	for (auto& iter : m_Channels)
	{
		if (strChannelName == iter->Get_Name())
			return iter;
	}

	return nullptr;
}

#pragma region Deprecated
//HRESULT CAnimation::Ready_AnimationTexture()
//{
//	CGameInstance* GAME_INSTANCE = GET_INSTANCE(CGameInstance);
//	
//	_matrix TempMatrix = XMMatrixIdentity();
//	_float4x4 TempMat;
//	vector<vector<_float4x4>> Matrices;
//
//	Matrices.resize(_uint(m_fDuration));
//
//
//	for (_uint i = 0; i < _uint(m_fDuration); ++i)
//	{
//		Matrices[i].reserve(m_iNumChannels);
//
//		_uint		iChannelIndex = 0;
//		for (auto& pChannel : m_Channels)
//		{
//			m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(i, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex], &TempMatrix);
//			++iChannelIndex;
//		}
//
//		XMStoreFloat4x4(&TempMat, TempMatrix);
//		Matrices[i].push_back(TempMat);
//	}
//
//
//	ID3D11Texture2D* pTexture;
//	D3D11_TEXTURE2D_DESC TextureDesc;
//	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
//	TextureDesc.Width = m_iNumChannels * 4;
//	TextureDesc.Height = _uint(m_fDuration);
//	TextureDesc.ArraySize = 1;
//	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16����Ʈ
//	TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
//	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//	TextureDesc.MipLevels = 1;
//	TextureDesc.SampleDesc.Count = 1;
//
//	const _uint iLineSize = m_iNumChannels * sizeof(_float4x4);
//	const _uint iPageSize = iLineSize * _uint(m_fDuration);
//	void* pFullData = nullptr;
//	pFullData = ::malloc(iPageSize);
//	
//
//	_uint iStartOffset = 0;
//	for (_uint i = 0; i < _uint(m_fDuration); ++i)
//	{
//		BYTE* iStart = reinterpret_cast<BYTE*>(pFullData) + iStartOffset;
//		void* pData = iStart;
//		::memcpy(pData, Matrices[i].data(), iLineSize);
//		iStartOffset += iLineSize;
//	}
//
//
//	// ���ҽ� �����
//	D3D11_SUBRESOURCE_DATA Subresource;
//
//	Subresource.pSysMem = pFullData;
//	Subresource.SysMemPitch = m_iNumChannels * sizeof(_float4x4);
//
//	if (FAILED(GAME_INSTANCE->Get_Device()->CreateTexture2D(&TextureDesc, &Subresource, &pTexture)))
//		return E_FAIL;
//
//	::free(pFullData);
//	pFullData = nullptr;
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc;
//	ZeroMemory(&SrvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
//	SrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	SrvDesc.Texture2D.MipLevels = 1;
//
//	if (FAILED(GAME_INSTANCE->Get_Device()->CreateShaderResourceView(pTexture, &SrvDesc, &m_pSRV)))
//		return E_FAIL;
//
//
//
//
//	return S_OK;
//}
//
//HRESULT CAnimation::SetUp_AnimationTexture_OnShader(CShader* pShader, const char* pConstantName)
//{
//	if (FAILED(pShader->Bind_Texture(pConstantName, m_pSRV)))
//		return E_FAIL;
//
//	return S_OK;
//}

#pragma endregion

CAnimation* CAnimation::Create(aiAnimation* pAIAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(pAIAnimation)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create_Bin()
{
	CAnimation* pInstance = new CAnimation();
	return pInstance;
}

CAnimation* CAnimation::Clone(CModel* pModel)
{
	CAnimation* pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pModel)))
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
