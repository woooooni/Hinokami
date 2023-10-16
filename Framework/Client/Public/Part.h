#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CHierarchyNode;
class CModel;
class CShader;
class CTransform;
class CRenderer;
END

BEGIN(Client)

class CPart abstract : public CGameObject
{
public:
	typedef struct tagPartDesc
	{
		CTransform* pParentTransform;
	}PART_DESC;

protected:
	CPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObejctTag, _uint iObjectType);
	CPart(const CPart& rhs);

	virtual ~CPart() = default;

public:
	CHierarchyNode* Get_Socket(const wstring& strNodeName);
	_float4x4 Get_SocketPivotMatrix();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) PURE;
	virtual void LateTick(_float fTimeDelta) PURE;
	virtual HRESULT Render() PURE;

protected:
	CTransform*				m_pParentTransform = { nullptr };
	_float4x4				m_WorldMatrix;


protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CTransform* m_pTransformCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

protected:
	HRESULT Compute_RenderMatrix(_fmatrix ChildMatrix);


public:
	static CPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END