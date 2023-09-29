#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const wstring& Get_Name() const {
		return m_strName;
	}

public:
	HRESULT Initialize(aiNodeAnim* pAIChannel);
	_uint Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode, __out _matrix* pOutMatrix = nullptr);
	_uint Interpolation(_float fPlayTime, _float fTimeDelta, class CAnimation* pNextAnimation, _uint iCurrentKeyFrame, class CHierarchyNode* pNode, class CModel* pModel);

public:
	KEYFRAME Get_FirstFrame() { return m_KeyFrames.front(); }
	KEYFRAME Get_CurrFrame() { return m_KeyFrames[m_iCurrFrames]; }

private:
	wstring							m_strName;

	_uint							m_iNumKeyFrames = 0;
	_uint							m_iCurrFrames = 0;
	vector<KEYFRAME>				m_KeyFrames;

	_float	m_fInterpolationTime = 0.f;


public:
	static CChannel* Create(aiNodeAnim* pAIChannel);
	static CChannel* Create_Bin();
	virtual void Free() override;

	friend class CData_Manager;
};

END