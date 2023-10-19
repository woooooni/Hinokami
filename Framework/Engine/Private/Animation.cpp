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
	, m_fSpeed(rhs.m_fSpeed)
	, m_bRootAnimation(rhs.m_bRootAnimation)
	, m_bLoop(rhs.m_bLoop)
	/*, m_pSRV(rhs.m_pSRV)*/
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

}

HRESULT CAnimation::Initialize_Prototype(aiAnimation* pAIAnimation)
{
	m_strName = CUtils::ToWString(pAIAnimation->mName.C_Str());
	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->mNumChannels;


	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* aiNodeAnim : mChannel은 키프레임 정보들을 가지낟. */
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		/* 왜 모아두는데?> 특정 애님에ㅣ션 상태일때 애니메이션을 재생하면 모든 뼈의 상태를 갱신하는건 빡세. 느려. 
		현재 애미에시연을 구동하기위한 뼈대만 상태 갱신해주기 위해. */
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
	m_bFinished = false;

	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = 0;
	}
}

HRESULT CAnimation::Play_Animation(CTransform* pTransform, _float fTimeDelta)
{
	m_fPlayTime += m_fSpeed * m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime >= m_fDuration)
	{
		
		if (m_bLoop)
		{
			m_fPlayTime = 0.f;
			for (auto& pChannel : m_Channels)
			{
				for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
					iCurrentKeyFrame = 0;
			}
		}
		else
			m_fPlayTime = m_fDuration;

		m_bFinished = true;
	}

	_uint		iChannelIndex = 0;

	/* 이 애니메이션 구동을 위한 모든 뼈들을 순회하며 뼈들의 행렬을 갱신해준다. */
	/* Transformation : 전달된 시간에 따른 키프레임(시간, 스케일, 회전, 이동)정보를 이용하여 Transformation을 만든다. */
	/* 하이어라키 노드에 저장해준다. */
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, fTimeDelta, m_ChannelKeyFrames[iChannelIndex], pTransform, m_HierarchyNodes[iChannelIndex],&m_fRatio);
		m_ChannelOldKeyFrames[iChannelIndex] = m_ChannelKeyFrames[iChannelIndex];
		++iChannelIndex;
	}

	return S_OK;
}

HRESULT CAnimation::Play_Animation(CModel* pModel, CTransform* pTransform, CAnimation* pNextAnimation, _float fTimeDelta)
{
	if (pModel->Is_InterpolatingAnimation())
	{
		m_fPlayTime += m_fSpeed * m_fTickPerSecond * fTimeDelta;

		_uint		iChannelIndex = 0;
		for (auto& pChannel : m_Channels)
		{
			pChannel->Interpolation(m_fPlayTime, fTimeDelta, this, pNextAnimation, pTransform, m_ChannelOldKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex], pModel, &m_fRatio);
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

void CAnimation::Set_AnimationPlayTime(CTransform* pTransform, _float fPlayTime, _float fTimeDelta)
{
	m_fPlayTime = fPlayTime;

	_uint		iChannelIndex = 0;
	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, fTimeDelta, m_ChannelKeyFrames[iChannelIndex], pTransform, m_HierarchyNodes[iChannelIndex], nullptr);
		m_ChannelOldKeyFrames[iChannelIndex] = m_ChannelKeyFrames[iChannelIndex];
		++iChannelIndex;
	}
}

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
