#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(aiAnimation* pAIAnimation);
	HRESULT Initialize(class CModel* pModel);

	HRESULT Play_Animation(class CTransform* pTransform, _float fTimeDelta);
	HRESULT Play_Animation(class CModel* pModel, class CTransform* pTransform, CAnimation* pNextAnimation, _float fTimeDelta);
	void Reset_Animation();


public:
	const list<KEYFRAME> Get_Curr_KeyFrames();
	const list<KEYFRAME> Get_First_KeyFrames();

public:
	class CChannel* Get_Channel(const wstring & strChannelName);
	const vector<class CChannel*>& Get_Channels() { return m_Channels; }

	_float Get_Ratio() { return m_fRatio; }
	_float Get_Duration() { return m_fDuration; }

	void Set_AnimationPlayTime(class CTransform* pTransform, _float fPlayTime, _float fTimeDelta);
	_float Get_PlayTime() { return m_fPlayTime; }

	void Set_TickPerSecond(_float fTickPerSecond) { m_fTickPerSecond = fTickPerSecond; }
	_float Get_TickPerSecond() { return m_fTickPerSecond; }

public:
	_float Get_AnimationSpeed() { return m_fSpeed; }
	void Set_AnimationSpeed(_float fSpeed) { m_fSpeed = fSpeed; }

	void Set_Pause(_bool bPause) { m_bPause = bPause; }
	_bool Is_Pause() { return m_bPause; }

	const wstring& Get_AnimationName() { return m_strName; }
	void Set_AnimationName(const wstring& strName) { m_strName = strName; }

	void Set_RootAnimation(_bool bRootAnimation) { m_bRootAnimation = bRootAnimation; }
	_bool Is_RootAnimation() { return m_bRootAnimation; }
	
	void Set_Loop(_bool bLoop) { m_bLoop = bLoop; }
	_bool Is_Loop() { return m_bLoop; }


private:
	wstring						m_strName;

	/* �� �ִϸ��̼��� �����ϱ����� ���Ǵ� ���� ����. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* �ִϸ��̼� ����ϴµ� �ɸ��� ��ü�ð�. */
	_float						m_fDuration = 0.f;

	/* �ִϸ��̼��� �ʴ� ��� �ӵ�. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;
	_float						m_fSpeed = 1.f;
	_float						m_fRatio = 0.f;
	_float						m_fTweeningRatio = 0.f;

private: /* ������ �ִϸ��̼� ���� ���� ������. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;
	vector<_uint>					m_ChannelOldKeyFrames;

	_bool m_bPause = false;
	_bool m_bRootAnimation = false;
	_bool m_bLoop = false;
	// ID3D11ShaderResourceView* m_pSRV = nullptr;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	static CAnimation* Create_Bin();
	CAnimation* Clone(class CModel* pModel);

	virtual void Free() override;

	friend class CModel_Manager;
};

END