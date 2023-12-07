#include "stdafx.h"
#include "State_Monster_Regen.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"


CState_Monster_Regen::CState_Monster_Regen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine)
	: CState(pStateMachine)
{

}

HRESULT CState_Monster_Regen::Initialize(const list<wstring>& AnimationList)
{

	if (FAILED(__super::Initialize(AnimationList)))
		return E_FAIL;

	m_pOwnerMonster = dynamic_cast<CMonster*>(m_pOwner);
	if (nullptr == m_pOwnerMonster)
		return E_FAIL;
	
	return S_OK;
}

void CState_Monster_Regen::Enter_State(void* pArg)
{
	m_pOwner->Set_ActiveColliders(CCollider::DETECTION_TYPE::ATTACK, false);
	m_pModelCom->Set_AnimIndex(m_AnimIndices[0]);

	TCHAR strSoundFileName[MAX_PATH] = L"";
	switch (m_pOwnerMonster->Get_Monster_Type())
	{
	case Client::CMonster::NORMAL_0:
		lstrcatW(strSoundFileName, L"Voice_Normal_Monster_0_Regen_0");
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_MONSTER1, 1.f);
		break;
	case Client::CMonster::NORMAL_1:
		lstrcatW(strSoundFileName, L"Voice_Normal_Monster_1_Regen_0");
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_MONSTER2, 1.f);
		break;
	case Client::CMonster::NORMAL_2:
		lstrcatW(strSoundFileName, L"Voice_Normal_Monster_1_Regen");
		lstrcatW(strSoundFileName, L".wav");
		GI->Play_Sound(strSoundFileName, CHANNELID::SOUND_VOICE_MONSTER3, 1.f);
		break;
	}
}

void CState_Monster_Regen::Tick_State(_float fTimeDelta)
{
	if (m_pModelCom->Is_Animation_Finished(m_AnimIndices[0]))
		m_pStateMachineCom->Change_State(CMonster::MONSTER_STATE::IDLE);
}

void CState_Monster_Regen::Exit_State()
{
	
}












CState_Monster_Regen* CState_Monster_Regen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pStateMachine,const list<wstring>& AnimationList)
{
	CState_Monster_Regen* pInstance =  new CState_Monster_Regen(pDevice, pContext, pStateMachine);
	if (FAILED(pInstance->Initialize(AnimationList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed Create : CState_Monster_Regen");
		return nullptr;
	}
		
	return pInstance;
}

void CState_Monster_Regen::Free()
{
	__super::Free();
}
