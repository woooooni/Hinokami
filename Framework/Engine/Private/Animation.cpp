#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "AsUtils.h"
#include "Shader.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
	, m_iFrameCount(rhs.m_iFrameCount)
	, m_fSpeed(rhs.m_fSpeed)
	, m_KeyFrames(rhs.m_KeyFrames)
	, m_pAnimTexSRV(rhs.m_pAnimTexSRV)
	, m_AnimTransforms(m_AnimTransforms)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	ZeroMemory(&m_tKeyDesc, sizeof(KEY_DESC));

	Safe_AddRef(m_pAnimTexSRV);
}

HRESULT CAnimation::Initialize_Prototype(aiAnimation* pAIAnimation)
{
	//m_fDuration = pAIAnimation->mDuration;
	//m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

	///* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	//m_iNumChannels = pAIAnimation->mNumChannels;


	///* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	//for (_uint i = 0; i < m_iNumChannels; ++i)
	//{
	//	/* aiNodeAnim : mChannel은 키프레임 정보들을 가지낟. */
	//	CChannel*	d	pChannel = CChannel::Create(pAIAnimation->mChannels[i]);
	//	if (nullptr == pChannel)
	//		return E_FAIL;

	//	/* 왜 모아두는데?> 특정 애님에ㅣ션 상태일때 애니메이션을 재생하면 모든 뼈의 상태를 갱신하는건 빡세. 느려. 
	//	현재 애미에시연을 구동하기위한 뼈대만 상태 갱신해주기 위해. */
	//	m_Channels.push_back(pChannel);
	//}

	return S_OK;
}

HRESULT CAnimation::Initialize(CModel* pModel)
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

	return S_OK;
}

HRESULT CAnimation::LoadData_FromAnimationFile(CAsFileUtils* pFileUtils, _fmatrix PivotMatrix)
{
	m_szName = CAsUtils::ToWString(pFileUtils->Read<string>());
	m_fDuration = pFileUtils->Read<_float>();
	m_fTickPerSecond = pFileUtils->Read<_float>();
	m_iFrameCount = pFileUtils->Read<uint32>();

	uint32 iKeyframesCount = pFileUtils->Read<uint32>();

	for (uint32 i = 0; i < iKeyframesCount; i++)
	{
		shared_ptr<ModelKeyframe> Keyframe = make_shared<ModelKeyframe>();
		Keyframe->szBoneName = CAsUtils::ToWString(pFileUtils->Read<string>());

		uint32 iSize = pFileUtils->Read<uint32>();

		if (iSize > 0)
		{
			Keyframe->KeyData.resize(iSize);
			void* ptr = &Keyframe->KeyData[0];
			pFileUtils->Read(&ptr, sizeof(ModelKeyframeData) * iSize);
		}

		m_KeyFrames[Keyframe->szBoneName] = Keyframe;
	}

	return S_OK;
}

HRESULT CAnimation::LoadData_FromConverter(shared_ptr<asAnimation> pAnimation, _fmatrix PivotMatrix)
{
	m_szName = CAsUtils::ToWString(pAnimation->name);
	m_fDuration = pAnimation->duration;
	m_fTickPerSecond = pAnimation->frameRate;
	m_iFrameCount = pAnimation->frameCount;

	uint32 iKeyframesCount = pAnimation->keyframes.size();

	for (uint32 i = 0; i < iKeyframesCount; i++)
	{
		shared_ptr<ModelKeyframe> Keyframe = make_shared<ModelKeyframe>();
		Keyframe->szBoneName = CAsUtils::ToWString(pAnimation->keyframes[i]->boneName);

		uint32 iSize = pAnimation->keyframes[i]->transforms.size();

		if (iSize > 0)
		{
			Keyframe->KeyData.resize(iSize);
			void* ptr = &Keyframe->KeyData[0];
			memcpy(ptr, pAnimation->keyframes[i]->transforms.data(), sizeof(ModelKeyframeData) * iSize);
		}

		m_KeyFrames[Keyframe->szBoneName] = Keyframe;
	}

	return S_OK;
}


HRESULT CAnimation::Play_Animation(_float fTimeDelta)
{
	if (!m_bPause)
	{
		m_tKeyDesc.fSumTime += fTimeDelta;
	}
		

	float fTimePerFrame = 1.f / (m_fTickPerSecond * m_fSpeed);
	if (m_tKeyDesc.fSumTime >= fTimePerFrame)
	{
		m_tKeyDesc.fSumTime = 0.f;
		m_tKeyDesc.iCurrFrame = (m_tKeyDesc.iCurrFrame + 1) % m_iFrameCount;
		m_tKeyDesc.iNextFrame = (m_tKeyDesc.iCurrFrame + 1) % m_iFrameCount;
	}


	m_tKeyDesc.fRatio = (m_tKeyDesc.fSumTime / fTimePerFrame);

	return S_OK;
}

HRESULT CAnimation::Create_Transform(ID3D11Device* pDevice, const vector<shared_ptr<ModelBone>>& Bones, _fmatrix PivotMatrix)
{
	m_AnimTransforms.resize(m_iFrameCount);
	m_iBoneSize = Bones.size();

	for (auto& iter : m_AnimTransforms)
		iter.reserve(m_iBoneSize);

	vector<Matrix> TempBoneTransforms(m_iBoneSize, Matrix::Identity);

	for (uint32 i = 0; i < m_iFrameCount; i++)
	{
		for (uint32 k = 0; k < m_iBoneSize; k++)
		{
			shared_ptr<ModelBone> Bone = Bones[k];

			Matrix matAnimation;

			shared_ptr<ModelKeyframe> Frame = m_KeyFrames[Bone->strName];
			if (Frame != nullptr)
			{
				ModelKeyframeData& Data = Frame->KeyData[i];

				Matrix S, R, T;
				S = Matrix::CreateScale(Data.vScale.x, Data.vScale.y, Data.vScale.z);
				R = Matrix::CreateFromQuaternion(Data.vRotation);
				T = Matrix::CreateTranslation(Data.vTranslation.x, Data.vTranslation.y, Data.vTranslation.z);

				matAnimation = S * R * T;
			}
			else
			{
				matAnimation = Matrix::Identity;
			}

			// [ !!!!!!! ]
			Matrix matToRoot = Bone->matTransform;
			Matrix matInvGlobal = matToRoot.Invert();

			int32 iParentIndex = Bone->iParentID;

			Matrix matParent = Matrix::Identity;
			if (iParentIndex >= 0)
				matParent = TempBoneTransforms[iParentIndex];

			TempBoneTransforms[k] = matAnimation * matParent;

			m_AnimTransforms[i].push_back(matInvGlobal * TempBoneTransforms[k] * PivotMatrix);
		}

		int iTemp = 0;
	}

	// CreateTexture

	ID3D11Texture2D* pAnimTex = nullptr;

	{
		D3D11_TEXTURE2D_DESC Desc;
		ZeroMemory(&Desc, sizeof(D3D11_TEXTURE2D_DESC));
		Desc.Width = m_iBoneSize * 4;
		Desc.Height = m_iFrameCount;
		Desc.ArraySize = 1;
		Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트
		Desc.Usage = D3D11_USAGE_IMMUTABLE;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		Desc.MipLevels = 1;
		Desc.SampleDesc.Count = 1;


		const uint32 iLineSize = m_iBoneSize * sizeof(Matrix);
		const uint32 iPageSize = iLineSize * m_iFrameCount;
		void* pFullData = nullptr;
		pFullData = ::malloc(iPageSize);

		uint32 iStartOffset = 0;
		for (_uint i = 0; i < m_iFrameCount; ++i)
		{
			BYTE* iStart = reinterpret_cast<BYTE*>(pFullData) + iStartOffset;
			void* pData = iStart;
			::memcpy(pData, m_AnimTransforms[i].data(), iLineSize);
			iStartOffset += iLineSize;
		}


		// 리소스 만들기
		D3D11_SUBRESOURCE_DATA Subresource;

		Subresource.pSysMem = pFullData;
		Subresource.SysMemPitch = m_iBoneSize * sizeof(Matrix);

		if (FAILED(pDevice->CreateTexture2D(&Desc, &Subresource, &pAnimTex)))
			return E_FAIL;

		::free(pFullData);
		pFullData = nullptr;
	}



	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		Desc.Texture2D.MipLevels = 1;

		if (FAILED(pDevice->CreateShaderResourceView(pAnimTex, &Desc, &m_pAnimTexSRV)))
			return E_FAIL;
	}

	int iTemp = 0;

	return S_OK;
}

shared_ptr<ModelKeyframe> CAnimation::GetKeyframe(const wstring& name)
{
	auto pKeyFrame = m_KeyFrames.find(name);
	if (pKeyFrame == m_KeyFrames.end())
		return nullptr;

	return pKeyFrame->second;
}

HRESULT CAnimation::SetUpAnimation_OnShader(CShader* pShader, const char* pConstantTexturename, const char* pConstantDescName)
{
	if (FAILED(pShader->Bind_Texture(pConstantTexturename, m_pAnimTexSRV)))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue(pConstantDescName, &m_tKeyDesc, sizeof(KEY_DESC))))
		return S_OK;
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
