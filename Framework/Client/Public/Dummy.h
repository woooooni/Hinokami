#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;
class CStateMachine;
class CHierarchyNode;
END

BEGIN(Client)

class CDummy : public CGameObject
{

private:
	CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader* Get_ShaderCom(CModel::TYPE eType) 
	{ 
		if (eType == CModel::TYPE::TYPE_ANIM)
			return m_pAnimShaderCom;
		else
			return m_pNonAnimShaderCom;
	}
	CTransform* Get_TransformCom() { return m_pTransformCom; }
	CModel* Get_ModelCom() { return m_pModelCom; }

public:
	HRESULT Ready_ModelCom(_uint eType, const wstring& strFilePath, const wstring& strFileName);

public:
	HRESULT Export_Model_Bin(const wstring& strFilePath, const wstring& strFileName);
	HRESULT Import_Model();
	
protected:
	CShader* m_pNonAnimShaderCom = nullptr;
	CShader* m_pAnimShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;

protected:
	virtual HRESULT Ready_Components() override;

private:
	void Input(_float fTimeDelta);


public:
	static CDummy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END

