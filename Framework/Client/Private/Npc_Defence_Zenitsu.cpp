#include "stdafx.h"
#include "GameInstance.h"
#include "Npc_Defence_Zenitsu.h"
#include "HierarchyNode.h"
#include "Part.h"
#include "Pool.h"
#include "Monster_Normal_0.h"
#include "Monster_Normal_1.h"
#include "Monster_Normal_2.h"
#include "UI_Manager.h"
#include "UI_NextFog.h"
#include "Character.h"
#include "Utils.h"

USING(Client)
CNpc_Defence_Zenitsu::CNpc_Defence_Zenitsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
	: CNpc(pDevice, pContext, strObjectTag)
{
	
}

CNpc_Defence_Zenitsu::CNpc_Defence_Zenitsu(const CNpc_Defence_Zenitsu& rhs)
	: CNpc(rhs)	

{	
	
}

HRESULT CNpc_Defence_Zenitsu::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc_Defence_Zenitsu::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_Colliders()))
		return E_FAIL;

	if (FAILED(Ready_Defence()))
		return E_FAIL;

	if (FAILED(Ready_Scripts()))
		return E_FAIL;

	return S_OK;
}

void CNpc_Defence_Zenitsu::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/*if (KEY_TAP(KEY::R))
		GI->Lock_Mouse();*/

	if(m_bStartDefence)
		Tick_Defence(fTimeDelta);

	if (m_bReserveNextScene)
	{
		m_bReserveNextScene = false;

		CUI_NextFog::NEXT_INFO NextInfo;
		NextInfo.eNextLevel = LEVELID::LEVEL_TRAIN_BOSS;
		NextInfo.strFolderName = "Train_Boss";
		GI->Stop_All();
		if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_Logo_NextFog", &NextInfo)))
			assert(nullptr);

		
		Set_Dead(true);
		return;
	}

	if (m_bTalking)
	{
		Talking(fTimeDelta);
	}
}

void CNpc_Defence_Zenitsu::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CNpc_Defence_Zenitsu::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CNpc_Defence_Zenitsu::Render_ShadowDepth()
{
	__super::Render_ShadowDepth();

	return S_OK;
}

void CNpc_Defence_Zenitsu::Collision_Enter(const COLLISION_INFO& tInfo)
{

}

void CNpc_Defence_Zenitsu::Collision_Continue(const COLLISION_INFO& tInfo)
{
	__super::Collision_Continue(tInfo);
	if (tInfo.pOther->Get_ObjectType() == OBJ_TYPE::OBJ_CHARACTER)
	{
		// TODO UI 띄우기
		if (true == m_bTalking || true == m_bStartDefence)
			return;

		if (KEY_TAP(KEY::F))
		{
			CTransform* pTransform = tInfo.pOther->Get_Component<CTransform>(L"Com_Transform");
			if (nullptr == pTransform)
				return;


			m_pTransformCom->LookAt_ForLandObject(pTransform->Get_State(CTransform::STATE_LOOK));
			m_bTalking = true;
			Talk();
		}
			
	}
}

void CNpc_Defence_Zenitsu::Collision_Exit(const COLLISION_INFO& tInfo)
{
	
	__super::Collision_Exit(tInfo);
}





void CNpc_Defence_Zenitsu::On_Damaged(CGameObject* pAttacker, _uint eDamageType, _float fDamage)
{
	__super::On_Damaged(pAttacker, eDamageType, fDamage);
}

HRESULT CNpc_Defence_Zenitsu::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Zenitsu"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateCom)))
		return E_FAIL;

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissoveTexture)))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 3.5, 150.f, 1.f));

	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof NavigationDesc);

	XMStoreFloat4(&NavigationDesc.vStartWorldPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	NavigationDesc.bInitialize_Index = true;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat4(&m_pNavigationCom->Get_NaviDesc().vStartWorldPosition), 1.f));

	CRigidBody::RIGID_BODY_DESC RigidDesc;
	ZeroMemory(&RigidDesc, sizeof RigidDesc);


	RigidDesc.pNavigation = m_pNavigationCom;
	RigidDesc.pTransform = m_pTransformCom;

	/* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigidBodyCom, &RigidDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc_Defence_Zenitsu::Ready_States()
{
	m_iRandomAnim_Indices.reserve(22);
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn01_L"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn01_R"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn02_L"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn02_R"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn03_L"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn03_R"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn04_L"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn04_R"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn05_L"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn05_R"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn06_L"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn06_R"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn07_L"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn07_R"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn10_0"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn10_1"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn11_0"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn11_1"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn12_0"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn12_1"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn13_0"));
	m_iRandomAnim_Indices.push_back(m_pModelCom->Find_AnimationIndex(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvRxn13_1"));

	m_pModelCom->Set_Animation(L"SK_P0003_V00_C00.ao|A_P0003_V00_C00_AdvNut03_1");

	return S_OK;
}

HRESULT CNpc_Defence_Zenitsu::Ready_Colliders()
{
	CCollider_Sphere::SPHERE_COLLIDER_DESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	BoundingSphere tSphere;
	ZeroMemory(&tSphere, sizeof(BoundingSphere));
	tSphere.Radius = 1.f;
	XMStoreFloat4x4(&ColliderDesc.ModePivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColliderDesc.pOwnerTransform = m_pTransformCom;

	ColliderDesc.tSphere = tSphere;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"Root");
	ColliderDesc.vOffsetPosition = _float3(0.f, 1.f, 0.f);

	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BOUNDARY, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.tSphere.Radius = .6f;
	ColliderDesc.pNode = m_pModelCom->Get_HierarchyNode(L"Root");
	ColliderDesc.vOffsetPosition = _float3(0.f, 0.5f, 0.f);
	if (FAILED(__super::Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::SPHERE, CCollider::DETECTION_TYPE::BODY, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc_Defence_Zenitsu::Ready_Defence()
{
	m_RegenPositions.reserve(8);
	m_RegenPositions.push_back({ -2.5f, 3.5f, 40.f, 1.f });
	m_RegenPositions.push_back({ 2.5f, 3.5f, 40.f, 1.f });
	m_RegenPositions.push_back({ 0.f, 3.5f, 48.f, 1.f });
	m_RegenPositions.push_back({ -2.5f, 3.5f, 52.f, 1.f });
	m_RegenPositions.push_back({ 2.5f, 3.5f, 52.f, 1.f });
	m_RegenPositions.push_back({ 0.f, 3.5f, 56.f, 1.f });
	m_RegenPositions.push_back({ -2.5f, 3.5f, 60.f, 1.f });
	m_RegenPositions.push_back({ 2.5f, 3.5f, 60.f, 1.f });



	return S_OK;
}

HRESULT CNpc_Defence_Zenitsu::Ready_Scripts()
{
	m_StartScripts.reserve(100);
	m_EndScripts.reserve(100);

	m_StartScripts.push_back(L"히이이익! 혈귀가 있는곳으로 이동하는게 아니라, 여기서 나오는거 였어?\n싫어~~! 나 내릴래~~!!");
	m_StartScripts.push_back(L"싫어! 내릴래! 내릴거야!! \n 여기 있기 싫어!");

	m_EndScripts.push_back(L"들린다.. 이상한 소리가 들려. 탄지로.");

	return S_OK;
}

void CNpc_Defence_Zenitsu::Talk()
{
	m_bTalking = true;
	m_iTalkIdx = 0;
}

void CNpc_Defence_Zenitsu::Talking(_float fTimeDelta)
{
	if (m_bDead)
		return;

	m_fTextAlpha -= fTimeDelta;
	if (m_fTextAlpha <= 0.f)
		m_fTextAlpha = 0.f;


	if (m_bClearDefence)
	{
		if (KEY_TAP(KEY::F) && m_fTextAlpha == 0.f)
		{
			++m_iTalkIdx;
			if (m_iTalkIdx >= m_EndScripts.size())
			{
				m_bTalking = false;
				m_bClearDefence = false;
				m_bReserveNextScene = true;
				return;
			}
			m_fTextAlpha = 1.f;
		}
		
		CRenderer::TEXT_DESC TextDesc;

		TextDesc.strFontTag = L"Basic";
		TextDesc.strText = L"[아가츠마 젠이츠]";
		TextDesc.vPosition = { g_iWinSizeX / 2.f - (5.f * m_EndScripts[m_iTalkIdx].size()), g_iWinSizeY - 130.f };
		TextDesc.vColor = { 1.f, 1.f, 1.f, m_fTextAlpha };

		m_pRendererCom->Add_Text(TextDesc);

		TextDesc.strFontTag = L"Basic";
		TextDesc.strText = m_EndScripts[m_iTalkIdx];
		TextDesc.vPosition = { g_iWinSizeX / 2.f - (5.f * m_EndScripts[m_iTalkIdx].size()), g_iWinSizeY - 100.f };
		TextDesc.vColor = { 1.f, 1.f, 1.f, m_fTextAlpha };

		m_pRendererCom->Add_Text(TextDesc);
	}
	else
	{
		if (KEY_TAP(KEY::F) && m_fTextAlpha == 0.f)
		{
			++m_iTalkIdx;
			if (m_iTalkIdx >= m_StartScripts.size())
			{
				Start_Defence();
				m_bTalking = false;
				m_iTalkIdx = 0;
				return;
			}
			m_fTextAlpha = 1.f;
		}

		CRenderer::TEXT_DESC TextDesc;
		TextDesc.strFontTag = L"Basic";
		TextDesc.strText = L"[아가츠마 젠이츠]";
		TextDesc.vPosition = { g_iWinSizeX / 2.f - (5.f * m_StartScripts[m_iTalkIdx].size()), g_iWinSizeY - 130.f };
		TextDesc.vColor = { 1.f, 1.f, 1.f, m_fTextAlpha };

		m_pRendererCom->Add_Text(TextDesc);

		TextDesc.strFontTag = L"Basic";
		TextDesc.strText = m_StartScripts[m_iTalkIdx];
		TextDesc.vPosition = { g_iWinSizeX / 2.f - (5.f * m_StartScripts[m_iTalkIdx].size()), g_iWinSizeY - 100.f };
		TextDesc.vColor = { 1.f, 1.f, 1.f, m_fTextAlpha };

		m_pRendererCom->Add_Text(TextDesc);
	}


	
}

void CNpc_Defence_Zenitsu::Start_Defence()
{
	m_bStartDefence = true;
	m_iDefenceDifficulty = 0;

	GI->Stop_All();
	GI->Play_BGM(L"Train.wav", 1.f);

	list<CGameObject*>& Monsters = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_MONSTER);
	for (auto& pMonster : Monsters)
		pMonster->Set_Dead(true);

	if (FAILED(CUI_Manager::GetInstance()->Battle_Start()))
		MSG_BOX("StartBattle Failed.");

	list<CGameObject*>& Characters = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_CHARACTER);
	for (auto& pCharacter : Characters)
	{
		CCharacter* pMainCharacter = dynamic_cast<CCharacter*>(pCharacter);
		if (nullptr == pMainCharacter)
		{
			MSG_BOX("StartBattle Failed.");
			return;
		}

		if (FAILED(CUI_Manager::GetInstance()->Reserve_HpBar(CUI_Manager::GAUGE_BARTYPE::LEFT_HP, pMainCharacter, pMainCharacter->Get_CharacterType())))
		{
			MSG_BOX("StartBattle Failed.");
			return;
		}
	}
	
}

void CNpc_Defence_Zenitsu::Tick_Defence(_float fTimeDelta)
{
	if (m_bDead)
		return;

	if (m_bStartDefence)
	{
		m_fAccRandomAnim += fTimeDelta;
		if (m_fAccRandomAnim >= m_fChangeAnimTime)
		{
			m_fAccRandomAnim = 0.f;
			_uint iRandomIndex = (rand() + rand() + rand()) % m_iRandomAnim_Indices.size();
			m_pModelCom->Set_AnimIndex(m_iRandomAnim_Indices[iRandomIndex]);
		}

		list<CGameObject*>& Monsters = GI->Find_GameObjects(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_MONSTER);
		if (0 == Monsters.size())
		{
			m_iDefenceDifficulty++;
			if (m_iDefenceDifficulty > 3)
			{
				m_bStartDefence = false;
				m_bClearDefence = true;

				if (FAILED(CUI_Manager::GetInstance()->Battle_End()))
					MSG_BOX("EndBattle Failed.");
			}
			else
			{
				for (_uint i = 0; i < 3 + m_iDefenceDifficulty; ++i)
				{
					_int iRandomNum = CUtils::Random_Int(0, 2);
					CGameObject* pMonster = nullptr;
					switch (iRandomNum)
					{
					case 0:
						pMonster = CPool<CMonster_Normal_0>::Get_Obj();
						break;
					case 1:
						pMonster = CPool<CMonster_Normal_1>::Get_Obj();
						break;
					case 2:
						pMonster = CPool<CMonster_Normal_2>::Get_Obj();
						break;
					}

					CTransform* pTransform = pMonster->Get_Component<CTransform>(L"Com_Transform");
					CNavigation* pNavigation = pMonster->Get_Component<CNavigation>(L"Com_Navigation");

					CNavigation::NAVIGATION_DESC NaviDesc;
					NaviDesc.bInitialize_Index = true;
					NaviDesc.vStartWorldPosition = m_RegenPositions[i];
					if (FAILED(pNavigation->Initialize(&NaviDesc)))
						MSG_BOX("Gen Defence Mob FAILED.");
					else
						pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&NaviDesc.vStartWorldPosition));


					if (FAILED(GI->Add_GameObject(GI->Get_CurrentLevel(), LAYER_TYPE::LAYER_MONSTER, pMonster)))
						MSG_BOX("Add_GameObject Failed.");
				}


			}
		}
		else
		{
			if (KEY_TAP(KEY::R))
			{
				for (auto& pMonster : Monsters)
				{
					pMonster->Set_Dead(true);
				}
			}
		}
	}
}

CNpc_Defence_Zenitsu* CNpc_Defence_Zenitsu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strObjectTag)
{
	CNpc_Defence_Zenitsu* pInstance = new CNpc_Defence_Zenitsu(pDevice, pContext, strObjectTag);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CNpc_Defence_Zenitsu");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CNpc_Defence_Zenitsu::Clone(void* pArg)
{
	CNpc_Defence_Zenitsu* pInstance = new CNpc_Defence_Zenitsu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNpc_Defence_Zenitsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNpc_Defence_Zenitsu::Free()
{
	__super::Free();
}
