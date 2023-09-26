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
	_float Get_Duration() { return m_fDuration; }


public:
	_float Get_AnimationSpeed() { return m_fSpeed; }
	void Set_AnimationSpeed(_float fSpeed) { m_fSpeed = fSpeed; }

	void Set_Pause(_bool bPause) { m_bPause = bPause; }
	_bool Is_Pause() { return m_bPause; }


	
	const wstring& Get_AnimationName() { return m_strName; }
	void Set_AnimationName(const wstring& strName) { m_strName = strName; }


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

private: /* ������ �ִϸ��̼� ���� ���� ������. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;

	_bool m_bPause = false;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	CAnimation* Clone(class CModel* pModel);

	virtual void Free() override;

	friend class CData_Manager;
};

END