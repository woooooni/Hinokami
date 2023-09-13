#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(aiAnimation* pAIAnimation);
	HRESULT Initialize(class CModel* pModel);
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

private:
	/* �� �ִϸ��̼��� �����ϱ����� ���Ǵ� ���� ����. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* �ִϸ��̼� ����ϴµ� �ɸ��� ��ü�ð�. */
	_float						m_fDuration = 0.f;

	/* �ִϸ��̼��� �ʴ� ��� �ӵ�. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;
	_bool						m_bPause = false;


	wstring						m_strAnimationName;

private: /* ������ �ִϸ��̼� ���� ���� ������. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;

	

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	CAnimation* Clone(class CModel* pModel);
	virtual void Free() override;
};

END