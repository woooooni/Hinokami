#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTransform;
END

class CPicking_Manager : public CBase
{
	DECLARE_SINGLETON(CPicking_Manager)

private:
	CPicking_Manager();
	virtual ~CPicking_Manager() = default;

public:
	HRESULT Reserve_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float fTimeDelta);

public:
	template<typename T>
	_bool Is_Picking(class CTransform* pTransform, const  vector<T>& Vertices, const vector<FACEINDICES32>& Indices, _uint iPrimitiveCount, __out _vector* vOut);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;
};

