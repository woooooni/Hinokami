#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameInstance.h"

BEGIN(Client)

template <typename T>
class CPool : public CBase
{
public:
	CPool() {}
	~CPool()
	{
		Free();
		__super::Free();
	}

public:
	static HRESULT Ready_Pool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag, LAYER_TYPE eType, void* pArg, const _uint& _iSize)
	{
		for (_uint i = 0; i < _iSize; ++i)
		{
			T* pObj = dynamic_cast<T*>(GI->Clone_GameObject(strPrototypeTag, eType, pArg));
			if (nullptr == pObj)
				return E_FAIL;

			Safe_AddRef(pObj);
			g_objQueue.push(pObj);
		}
		return S_OK;
	}

	static T* Get_Obj()
	{
		if (g_objQueue.empty()) 
			return nullptr;

		T* pObj = g_objQueue.front();
		if (nullptr == pObj)
			return nullptr;

		pObj->Set_Dead(false);
		pObj->Enter_Scene();

		g_objQueue.pop();
		return pObj;
	}

	static _bool Return_Obj(T* pObj)
	{
		if (nullptr == pObj)
			return nullptr;

		pObj->Return_Pool();
		g_objQueue.push(pObj);
		return true;
	}

	void Free()
	{
		if (g_objQueue.empty())
			return;

		_int iSize = g_objQueue.size();

		for (_int i = 0; i < iSize; ++i)
		{
			T* pObj = g_objQueue.front();
			Safe_Release(pObj);
			g_objQueue.pop();
		}
	}

	static queue<T*>& Get_Que() { return g_objQueue; }

private:
	static queue<T*> g_objQueue;     // 비활성화된 오브젝트들만 갖고있다.
};

template <typename T>
queue<T*> CPool<T>::g_objQueue;

END