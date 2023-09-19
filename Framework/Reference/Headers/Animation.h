#pragma once

#include "Base.h"
#include "AsTypes.h"
#include "AsFileUtils.h"

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
	HRESULT	LoadData_FromAnimationFile(CAsFileUtils* pFileUtils, _fmatrix PivotMatrix);
	HRESULT	LoadData_FromConverter(shared_ptr<asAnimation> pAnimation, _fmatrix PivotMatrix);


	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Create_Transform(ID3D11Device* pDevice, const vector<shared_ptr<ModelBone>>& Bones, _fmatrix PivotMatrix);

	KEY_DESC Get_KeyDesc() { return m_tKeyDesc; }
	_float Get_Duration() { return m_fDuration; }

	shared_ptr<ModelKeyframe> GetKeyframe(const wstring& name);
	HRESULT SetUpAnimation_OnShader(class CShader* pShader, const wstring& strMapname, const wstring& strDescName);



public:
	_float Get_AnimationSpeed() { return m_fSpeed; }
	void Set_AnimationSpeed(_float fSpeed) { m_fSpeed = fSpeed; }
	
	void Set_CurrFrame(_float fFrame) 
	{ 
		if (fFrame >= m_fDuration)
		{
			m_tKeyDesc.iCurrFrame = m_fDuration;
		}
		m_tKeyDesc.iCurrFrame = fFrame;
	}

	void Set_Pause(_bool bPause) { m_bPause = bPause; }
	_bool Is_Pause() { return m_bPause; }

	const wstring& Get_AnimationName() { return m_szName; }
	void Set_AnimationName(const wstring& strName) { m_szName = strName; }

private:
	wstring						m_szName;

	/* 이 애니메이션을 구동하기위해 사용되는 뼈의 갯수. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* 애니메이션 재생하는데 걸리는 전체시간. */
	_float						m_fDuration = 0.f;

	/* 애니메이션의 초당 재생 속도. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;
	_uint						m_iFrameCount;
	_float						m_fSpeed = 1.f;

	KEY_DESC					m_tKeyDesc;

private: /* 복제된 애니메이션 마다 따로 가진다. */
	_uint							m_iBoneSize;

	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;

	_bool m_bPause = false;

	unordered_map<wstring, shared_ptr<ModelKeyframe>>	m_KeyFrames;



	//Animation
	ID3D11ShaderResourceView* m_pAnimTexSRV;
	vector<vector<Matrix>>			m_AnimTransforms;
public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	CAnimation* Clone(class CModel* pModel);
	virtual void Free() override;
};

END