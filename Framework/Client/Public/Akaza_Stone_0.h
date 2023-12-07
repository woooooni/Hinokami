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
class CAkaza_Stone_0 final : public CGameObject
{

private:
	CAkaza_Stone_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CAkaza_Stone_0(const CAkaza_Stone_0& rhs);
	virtual ~CAkaza_Stone_0() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render_Instance(class CShader* pInstancingShader, class CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices) override;
	virtual HRESULT Render_Instance_Shadow(class CShader* pInstancingShader, class CVIBuffer_Instancing* pInstancingBuffer, const vector<_float4x4>& WorldMatrices) override;


private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Ready_Colliders();

private:
	_float m_fAccDeleteStart = 0.f;
	_float m_fDeleteStartTime = 1.f;
	_bool m_bDeleteStart = false;

private:
	_float m_fDissolveWeight = 0.f;
	

protected:
	class CShader* m_pShaderCom = nullptr;
	class CRenderer* m_pRendererCom = nullptr;
	class CTransform* m_pTransformCom = nullptr;
	class CModel* m_pModelCom = nullptr;
	class CTexture* m_pDissolveTexture = nullptr;


public:
	static CAkaza_Stone_0* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END

