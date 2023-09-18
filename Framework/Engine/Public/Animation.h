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
	wstring	 Get_Name() { return m_szName; }
	_float Get_Duration() { return m_fDuration; }

	shared_ptr<ModelKeyframe> GetKeyframe(const wstring& name);

	HRESULT SetUpAnimation_OnShader(class CShader* pShader, const wstring& strMapname, const wstring& strDescName);



private:

private:
	wstring						m_szName;

	/* �� �ִϸ��̼��� �����ϱ����� ���Ǵ� ���� ����. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* �ִϸ��̼� ����ϴµ� �ɸ��� ��ü�ð�. */
	_float						m_fDuration = 0.f;

	/* �ִϸ��̼��� �ʴ� ��� �ӵ�. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;
	_uint						m_iFrameCount;
	_float						m_fSpeed = 1.f;

	KEY_DESC					m_tKeyDesc;

private: /* ������ �ִϸ��̼� ���� ���� ������. */
	_uint							m_iBoneSize;

	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;


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