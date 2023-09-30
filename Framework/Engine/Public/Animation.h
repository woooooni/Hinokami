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

	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Play_Animation(class CModel* pModel, CAnimation* pNextAnimation, _float fTimeDelta);
	void Reset_Animation();

public:
	// HRESULT Ready_AnimationTexture();
	// HRESULT SetUp_AnimationTexture_OnShader(class CShader* pShader, const char* pConstantName);
	const list<KEYFRAME> Get_Curr_KeyFrames();
	const list<KEYFRAME> Get_First_KeyFrames();

public:
	class CChannel* Get_Channel(const wstring & strChannelName);
	const vector<class CChannel*>& Get_Channels() { return m_Channels; }
	_float Get_Duration() { return m_fDuration; }
	_float Get_CurrFrame() { return m_fPlayTime; }
	_float Get_TickPerSecond() { return m_fTickPerSecond; }

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
	vector<_uint>					m_ChannelOldKeyFrames;

	_bool m_bPause = false;
	// ID3D11ShaderResourceView* m_pSRV = nullptr;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	static CAnimation* Create_Bin();
	CAnimation* Clone(class CModel* pModel);

	virtual void Free() override;

	friend class CModel_Manager;
};

END