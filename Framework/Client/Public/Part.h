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
		CGameObject* pOwner;
		CHierarchyNode* pSocketBone;
		_float4x4		SocketPivot;
		CTransform* pParentTransform;
	}PART_DESC;

protected:
	CPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObejctTag, _uint iObjectType);
	CPart(const CPart& rhs);

	virtual ~CPart() = default;

public:
	CHierarchyNode* Get_Socket(const wstring& strNodeName);
	_float4x4 Get_SocketPivotMatrix();

	void Set_SocketBone(class CHierarchyNode* pNode);

	_float3 Get_PrevRotation() { return m_vPrevRotation; }
	CHierarchyNode* Get_Current_SocketBone() { return m_pSocketBone; }

	void Set_OriginRotation_Transform(_fmatrix RotationMatrix) { XMStoreFloat4x4(&m_OriginRotationTransform, RotationMatrix); }

	class CGameObject* Get_Owner() { return m_pOwner; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) PURE;
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Collision_Enter(const COLLISION_INFO& tInfo) {};
	virtual void Collision_Continue(const COLLISION_INFO& tInfo) {};
	virtual void Collision_Exit(const COLLISION_INFO& tInfo) {};


protected:
	CTransform*				m_pParentTransform = { nullptr };
	CHierarchyNode*			m_pSocketBone = { nullptr };
	_float4x4				m_OriginRotationTransform;
	_float4x4				m_SocketPivotMatrix;
	_float3					m_vPrevRotation = {};


protected: /* �ش� ��ü�� ����ؾ��� ������Ʈ���� �����ϳ�. */
	CTransform* m_pTransformCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CGameObject* m_pOwner = { nullptr };

protected:
	HRESULT Compute_RenderMatrix(_fmatrix ChildMatrix);


public:
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END