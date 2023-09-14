#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
	struct KEY_FRAME_DESC
	{
		_uint iCurrFrame;
		_uint iNextFrame;
		_float fRatio;
		_float fSumTime;
		_float fSpeed;
		_float2 fPadding;
	};

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(aiAnimation* pAIAnimation);
	HRESULT Initialize(class CModel* pModel, ID3D11Device* pDevice);
	HRESULT Play_Animation(_float fTimeDelta);

public:
	const vector<class CChannel*>& Get_Channels() { return m_Channels; }
	_uint Get_ChannelCount() { return m_iNumChannels; }

	const vector<class CHierarchyNode*>& Get_HierachyNodes() { return m_HierarchyNodes; }
	const vector<_uint>& Get_ChannelKeyFrames() { return m_ChannelKeyFrames; }

	const _float Get_Duration() { return m_fDuration; }
	const _float Get_CurrPlayTime() { return m_fPlayTime; }
	void Set_PlayTime(_float fPlayTime) { m_fPlayTime = fPlayTime; }

	void Set_Pause(_bool _bPause) { m_bPause = _bPause; }
	_bool Is_Pause() { return m_bPause; }

	const wstring& Get_AnimationName() { return m_strAnimationName; }
	
	_float Get_AnimationSpeed() { return m_fTickPerSecond; }
	void Set_AnimationSpeed(_float _fSpeed) { m_fTickPerSecond = _fSpeed; }

public:
	HRESULT SetUp_OnShader(class CShader* pShader, const wstring& strConstNameconst);

private:
	/* 이 애니메이션을 구동하기위해 사용되는 뼈의 갯수. */
	_uint						m_iNumChannels = 0;
	_uint						m_iFrameCount = 0;
	vector<class CChannel*>		m_Channels;

	/* 애니메이션 재생하는데 걸리는 전체시간. */
	_float						m_fDuration = 0.f;

	/* 애니메이션의 초당 재생 속도. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;
	_bool						m_bPause = false;


	wstring						m_strAnimationName;
	KEY_FRAME_DESC				m_tKeyFrame;

private: /* 복제된 애니메이션 마다 따로 가진다. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;

	ID3D11ShaderResourceView* m_pSRV = nullptr;

private:
	HRESULT Create_VTF_Texture(ID3D11Device* pDevice);
	

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	CAnimation* Clone(class CModel* pModel, ID3D11Device* pDevice);
	virtual void Free() override;
};

END