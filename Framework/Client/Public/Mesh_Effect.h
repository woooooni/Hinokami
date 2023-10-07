#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CMesh;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;

END

BEGIN(Client)

class CMesh_Effect final : public CEffect
{

private:
	CMesh_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag);
	CMesh_Effect(const CMesh_Effect& rhs);
	virtual ~CMesh_Effect() = default;


public:
	virtual HRESULT Initialize_Prototype(const wstring& strEffectName, const wstring& strModelFolderPath, const wstring& strModelFileName, const MESH_EFFECT_DESC& EffectDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Save_EffectInfo() override;


protected:
	virtual HRESULT Ready_Components(const wstring& strModelFolderPath, const wstring& strModelFileName);


private:
	class CModel* m_pModelCom = nullptr;
	wstring m_strEffectName;
	wstring m_strModelFolderPath;
	wstring m_strModelFileName;

public:
	static CMesh_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const wstring& strEffectName,
		const wstring& strObjectTag, 
		const wstring& strFolderPath, 
		const wstring& strFileName, 
		const MESH_EFFECT_DESC& EffectDesc);

	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END

