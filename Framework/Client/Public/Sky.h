#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CSky final : public CGameObject
{
public:
	enum SKY_TYPE { SUNSET, NIGHT, SKY_END };

protected:	
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, SKY_TYPE eSkyType);
	CSky(const CSky& rhs); /* ���� ������. */
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CModel*					m_pModelCom = { nullptr };
	class CRenderer*				m_pRendererCom = { nullptr };
	class CTransform*				m_pTransformCom = { nullptr };
	class CShader*					m_pShaderCom = { nullptr };


private:
	SKY_TYPE m_eSkyType = SKY_TYPE::SKY_END;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, SKY_TYPE eSkyType);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END