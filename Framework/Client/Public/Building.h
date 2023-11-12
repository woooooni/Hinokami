#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;
END


BEGIN(Client)
class CBuilding final : public CGameObject
{

private:
	CBuilding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CBuilding(const CBuilding& rhs);
	virtual ~CBuilding() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strFilePath, const wstring& strFileName);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render(class CVIBuffer_Instancing* pBufferInstance, const vector<_float4x4>& WorldMatrices) override;
	virtual HRESULT Render_ShadowDepth(class CVIBuffer_Instancing* pBufferInstance, const vector<_float4x4>& WorldMatrices) override;

public:
	CShader* Get_ShaderCom() { return m_pShaderCom; }
	CTransform* Get_TransformCom() { return m_pTransformCom; }
	CModel* Get_ModelCom() { return m_pModelCom; }

private:
	virtual HRESULT Ready_Components() override;

protected: /* �ش� ��ü�� ����ؾ��� ������Ʈ���� �����ϳ�. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;


private:
	wstring m_strBuildingName;

public:
	static CBuilding* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		const wstring& strObjectTag, const wstring& strFilePath, const wstring& strFileName);

	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END

