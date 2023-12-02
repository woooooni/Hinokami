#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Player.h"
#include "Camera_Free.h"
#include "Camera_Main.h"
#include "FileUtils.h"

#include "Sky.h"

#include "Tanjiro.h"
#include "Zenitsu.h"
#include "Sweath.h"
#include "Sword.h"

#include "Npc_Stand_0.h"
#include "Npc_Stand_1.h"
#include "Npc_Stand_2.h"
#include "Npc_Stand_3.h"

#include "Monster_Normal_0.h"
#include "Monster_Normal_1.h"
#include "Monster_Normal_2.h"
#include "Enmu_Projectile.h"
#include "Akaza_Projectile.h"

#include "Boss_Enmu.h"
#include "Boss_Akaza.h"


#include "Npc_Defence_Zenitsu.h"

#include "Dummy.h"
#include "Terrain.h"
#include <filesystem>
#include "Building.h"
#include "Prop.h"
#include "Grass.h"
#include "Tree.h"
#include "Utils.h"
#include "Ground.h"
#include "ImGui_Manager.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"


#include "UI.h"
#include "UI_Logo_Title.h"
#include "UI_Logo_BackGround.h"
#include "UI_Logo_SelectBase.h"
#include "UI_NextFog.h"
#include "UI_Loading_Background.h"
#include "UI_Loading_Anim.h"
#include "UI_Loading_Icon.h"



_bool CLoader::g_bFirstLoading = false;
CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY ThreadEntry(void* pArg)
{
	
	CoInitializeEx(nullptr, 0);

	/* ���Ӱ� ������ �����尡 ���Ѵ�. */
	CLoader*		pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevel, const wstring& strFolderName)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = eNextLevel;
	m_strFolderName = strFolderName;
	/* ���ο� �����带 ������. */
	/* ������ : �� �ڵ带 �о��ִ� �ϳ��� �帧? */
	/* 3 : ������ �����尡 ȣ���ؾ��ϴ� �Լ��� �ּ� */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_int CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = 0;
	

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Level_Logo();
		break;
	case LEVEL_TRAIN_STATION:
		hr = Loading_For_Level_Train_Station();
		break;

	case LEVEL_TRAIN:
		hr = Loading_For_Level_Train();
		break;

	case LEVEL_TRAIN_BOSS:
		hr = Loading_For_Level_Train_Boss();
		break;

	case LEVEL_FINAL_BOSS:
		hr = Loading_For_Level_Final_Boss();
		break;

	case LEVEL_TOOL:
		hr = Loading_For_Level_Tool();
		break;
	}

	if (FAILED(hr))
		return -1;	

	LeaveCriticalSection(&m_Critical_Section);

	return 0;	
}

HRESULT CLoader::Loading_For_Level_Logo()
{
	/* For.Texture */
	m_strLoading = TEXT("�ؽ��ĸ� �ε� �� �Դϴ�.");

	/* For.Prototype_Component_Texture_Logo_BackGround*/
	if (FAILED(GI->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Logo/Logo_BackGround.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Logo_Title */
	if (FAILED(GI->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Logo/Game_Title.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Logo_NonSelected */
	if (FAILED(GI->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_NonSelected"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Logo/Title_Base.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Logo_Selected */
	if (FAILED(GI->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Selected"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Logo/Title_Select.png"), 1))))
		return E_FAIL;


	/* For.Shader */
	m_strLoading = TEXT("���̴��� �ε� �� �Դϴ�.");

	/* For.GameObject */
	m_strLoading = TEXT("��ü������ �ε� �� �Դϴ�.");


	
	///* For.Prototype_GameObject_UI_Logo_Title */
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Logo_Title"), CUI_Logo_Title::Create(m_pDevice, m_pContext,
		{ g_iWinSizeX - 350.f, 190.f, 350.f, 350.f }), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	///* For.Prototype_GameObject_UI_Logo_BackGround */
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Logo_BackGround"), CUI_Logo_BackGround::Create(m_pDevice, m_pContext,
		{ 1232.f * 0.5f, g_iWinSizeY / 2.f, 1232.f, g_iWinSizeY }), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	///* For.Prototype_GameObject_UI_Logo_SelectBase */
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Logo_SelectBase"), CUI_Logo_SelectBase::Create(m_pDevice, m_pContext), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	///* For.Prototype_GameObject_UI_Logo_NextFog */
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Logo_NextFog"), CUI_NextFog::Create(m_pDevice, m_pContext,
		{ g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, g_iWinSizeX, g_iWinSizeY }), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;




	m_strLoading = TEXT("�ε� ��.");
	m_isFinished = true;

	return S_OK;
}


HRESULT CLoader::Loading_For_Level_Train_Station()
{
	/* For.Texture */
	m_strLoading = TEXT("�ؽ��ĸ� �ε� �� �Դϴ�.");


	/* For.Mesh */
	m_strLoading = TEXT("�޽ø� �ε� �� �Դϴ�.");


	/* For.Shader */
	m_strLoading = TEXT("���̴��� �ε� �� �Դϴ�.");





	m_strLoading = TEXT("��ü ������ �ε� �� �Դϴ�.");
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Logo_NextFog"), CUI_NextFog::Create(m_pDevice, m_pContext,
		{ g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, g_iWinSizeX, g_iWinSizeY }), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sky_Sunset"),
		CSky::Create(m_pDevice, m_pContext, CSky::SKY_TYPE::SUNSET), LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sky_Night"),
		CSky::Create(m_pDevice, m_pContext, CSky::SKY_TYPE::NIGHT), LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext, TEXT("Free_Camera")), LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Main"),
		CCamera_Main::Create(m_pDevice, m_pContext, TEXT("Main_Camera")), LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;


	CMonster::MONSTER_STAT tMonsterStat = {};
	tMonsterStat.fHp = 10.f;
	tMonsterStat.fMp = 10.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_0"),
		CMonster_Normal_0::Create(m_pDevice, m_pContext, TEXT("NormalMonster0"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;


	tMonsterStat.fHp = 20.f;
	tMonsterStat.fMp = 10.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_1"),
		CMonster_Normal_1::Create(m_pDevice, m_pContext, TEXT("NormalMonster1"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	tMonsterStat.fHp = 30.f;
	tMonsterStat.fMp = 0.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_2"),
		CMonster_Normal_2::Create(m_pDevice, m_pContext, TEXT("NormalMonster2"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Tanjiro"),
		CTanjiro::Create(m_pDevice, m_pContext, TEXT("Tanjiro"), CCharacter::CHARACTER_TYPE::TANJIRO), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Npc_Defence_Zenitsu"),
		CNpc_Defence_Zenitsu::Create(m_pDevice, m_pContext, TEXT("Npc_Defence_Zenitsu")), LAYER_TYPE::LAYER_NPC)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Npc_Stand_0"),
		CNpc_Stand_0::Create(m_pDevice, m_pContext), LAYER_TYPE::LAYER_NPC)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Npc_Stand_1"),
		CNpc_Stand_1::Create(m_pDevice, m_pContext), LAYER_TYPE::LAYER_NPC)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Npc_Stand_2"),
		CNpc_Stand_2::Create(m_pDevice, m_pContext), LAYER_TYPE::LAYER_NPC)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Npc_Stand_3"),
		CNpc_Stand_3::Create(m_pDevice, m_pContext), LAYER_TYPE::LAYER_NPC)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sword_Tanjiro"),
		CSword::Create(m_pDevice, m_pContext, TEXT("Tanjiro_Sword"), TEXT("Prototype_Component_Model_Sword_Tanjiro")), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sweath_Tanjiro"),
		CSweath::Create(m_pDevice, m_pContext, TEXT("Tanjiro_Sweath"), TEXT("Prototype_Component_Model_Sweath_Tanjiro")), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;


	m_strLoading = TEXT("���� �ε� �� �Դϴ�.");
	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Export/Weapon/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Resources/Sky/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Character/Tanjiro/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Character/Zenitsu/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Enemy/Monster/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Npc/")))
		return E_FAIL;



	//if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Export/Map/")))
	//	return E_FAIL;

	if (FAILED(Loading_Proto_AllObjects(L"../Bin/Export/Map/")))
		return E_FAIL;

	if (FAILED(Load_Map_Data(m_strFolderName)))
		return E_FAIL;

	if (FAILED(Load_Navi_Data(m_strFolderName)))
		return E_FAIL;

	 

	//if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Zenitsu"),
	//	CZenitsu::Create(m_pDevice, m_pContext, TEXT("Zenitsu"), CCharacter::CHARACTER_TYPE::ZENITSU), LAYER_TYPE::LAYER_CHARACTER)))
	//	return E_FAIL;


	//if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sword_Zenitsu"),
	//	CSword::Create(m_pDevice, m_pContext, TEXT("Zenitsu_Sword"), TEXT("Prototype_Component_Model_Sword_Zenitsu")), LAYER_TYPE::LAYER_CHARACTER)))
	//	return E_FAIL;

	//if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sweath_Zenitsu"),
	//	CSweath::Create(m_pDevice, m_pContext, TEXT("Zenitsu_Sweath"), TEXT("Prototype_Component_Model_Sweath_Zenitsu")), LAYER_TYPE::LAYER_CHARACTER)))
	//	return E_FAIL;

	m_strLoading = TEXT("�ε� ��.");
	m_isFinished = true;

	return S_OK;
}


HRESULT CLoader::Loading_For_Level_Train()
{
	/* For.Texture */
	m_strLoading = TEXT("�ؽ��ĸ� �ε� �� �Դϴ�.");


	/* For.Mesh */
	m_strLoading = TEXT("�޽ø� �ε� �� �Դϴ�.");


	/* For.Shader */
	m_strLoading = TEXT("���̴��� �ε� �� �Դϴ�.");





	m_strLoading = TEXT("��ü ������ �ε� �� �Դϴ�.");
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Logo_NextFog"), CUI_NextFog::Create(m_pDevice, m_pContext,
		{ g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, g_iWinSizeX, g_iWinSizeY }), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sky_SunSet"),
		CSky::Create(m_pDevice, m_pContext, CSky::SKY_TYPE::SUNSET), LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sky_Night"),
		CSky::Create(m_pDevice, m_pContext, CSky::SKY_TYPE::NIGHT), LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext, TEXT("Free_Camera")), LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Main"),
		CCamera_Main::Create(m_pDevice, m_pContext, TEXT("Main_Camera")), LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;


	CMonster::MONSTER_STAT tMonsterStat = {};
	tMonsterStat.fHp = 10.f;
	tMonsterStat.fMp = 10.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_0"),
		CMonster_Normal_0::Create(m_pDevice, m_pContext, TEXT("NormalMonster0"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;


	tMonsterStat.fHp = 20.f;
	tMonsterStat.fMp = 10.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_1"),
		CMonster_Normal_1::Create(m_pDevice, m_pContext, TEXT("NormalMonster1"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	tMonsterStat.fHp = 30.f;
	tMonsterStat.fMp = 0.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_2"),
		CMonster_Normal_2::Create(m_pDevice, m_pContext, TEXT("NormalMonster2"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;




	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Tanjiro"),
		CTanjiro::Create(m_pDevice, m_pContext, TEXT("Tanjiro"), CCharacter::CHARACTER_TYPE::TANJIRO), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Npc_Defence_Zenitsu"),
		CNpc_Defence_Zenitsu::Create(m_pDevice, m_pContext, TEXT("Npc_Defence_Zenitsu")), LAYER_TYPE::LAYER_NPC)))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sword_Tanjiro"),
		CSword::Create(m_pDevice, m_pContext, TEXT("Tanjiro_Sword"), TEXT("Prototype_Component_Model_Sword_Tanjiro")), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sweath_Tanjiro"),
		CSweath::Create(m_pDevice, m_pContext, TEXT("Tanjiro_Sweath"), TEXT("Prototype_Component_Model_Sweath_Tanjiro")), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;


	m_strLoading = TEXT("���� �ε� �� �Դϴ�.");
	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Export/Weapon/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Resources/Sky/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Character/Tanjiro/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Character/Zenitsu/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Enemy/Monster/")))
		return E_FAIL;



	//if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Export/Map/")))
	//	return E_FAIL;

	if (FAILED(Loading_Proto_AllObjects(L"../Bin/Export/Map/")))
		return E_FAIL;

	if (FAILED(Load_Map_Data(m_strFolderName)))
		return E_FAIL;

	if (FAILED(Load_Navi_Data(m_strFolderName)))
		return E_FAIL;

	CPool<CMonster_Normal_0>::Ready_Pool(m_pDevice, m_pContext, L"Prototype_GameObject_NormalMonster_0", LAYER_TYPE::LAYER_MONSTER, nullptr, 20);
	CPool<CMonster_Normal_1>::Ready_Pool(m_pDevice, m_pContext, L"Prototype_GameObject_NormalMonster_1", LAYER_TYPE::LAYER_MONSTER, nullptr, 20);
	CPool<CMonster_Normal_2>::Ready_Pool(m_pDevice, m_pContext, L"Prototype_GameObject_NormalMonster_2", LAYER_TYPE::LAYER_MONSTER, nullptr, 20);

	m_strLoading = TEXT("�ε� ��.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Train_Boss()
{

	/* For.Texture */
	m_strLoading = TEXT("�ؽ��ĸ� �ε� �� �Դϴ�.");


	/* For.Mesh */
	m_strLoading = TEXT("�޽ø� �ε� �� �Դϴ�.");


	/* For.Shader */
	m_strLoading = TEXT("���̴��� �ε� �� �Դϴ�.");





	m_strLoading = TEXT("��ü ������ �ε� �� �Դϴ�.");
	if (FAILED(GAME_INSTANCE->Add_Prototype(TEXT("Prototype_GameObject_UI_Logo_NextFog"), CUI_NextFog::Create(m_pDevice, m_pContext,
		{ g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, g_iWinSizeX, g_iWinSizeY }), LAYER_TYPE::LAYER_UI)))
		return E_FAIL;


	
		

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext, TEXT("Free_Camera")), LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sky_Sunset"),
		CSky::Create(m_pDevice, m_pContext, CSky::SUNSET), LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sky_Night"),
		CSky::Create(m_pDevice, m_pContext, CSky::NIGHT), LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;
	


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Main"),
		CCamera_Main::Create(m_pDevice, m_pContext, TEXT("Main_Camera")), LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;


	CMonster::MONSTER_STAT tMonsterStat = {};
	tMonsterStat.fHp = 10.f;
	tMonsterStat.fMp = 10.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_0"),
		CMonster_Normal_0::Create(m_pDevice, m_pContext, TEXT("NormalMonster0"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;


	tMonsterStat.fHp = 20.f;
	tMonsterStat.fMp = 10.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_1"),
		CMonster_Normal_1::Create(m_pDevice, m_pContext, TEXT("NormalMonster1"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	tMonsterStat.fHp = 30.f;
	tMonsterStat.fMp = 0.f;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NormalMonster_2"),
		CMonster_Normal_2::Create(m_pDevice, m_pContext, TEXT("NormalMonster2"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	
	tMonsterStat.fHp = 300.f;
	tMonsterStat.fMp = 1000.f;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Enmu"),
		CBoss_Enmu::Create(m_pDevice, m_pContext, TEXT("Enmu"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Akaza"),
		CBoss_Akaza::Create(m_pDevice, m_pContext, TEXT("Akaza"), tMonsterStat), LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Enmu_Projectile"),
		CEnmu_Projectile::Create(m_pDevice, m_pContext), LAYER_TYPE::LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Akaza_Projectile"),
		CAkaza_Projectile::Create(m_pDevice, m_pContext), LAYER_TYPE::LAYER_EFFECT)))
		return E_FAIL;



	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Tanjiro"),
		CTanjiro::Create(m_pDevice, m_pContext, TEXT("Tanjiro"), CCharacter::CHARACTER_TYPE::TANJIRO), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sword_Tanjiro"),
		CSword::Create(m_pDevice, m_pContext, TEXT("Tanjiro_Sword"), TEXT("Prototype_Component_Model_Sword_Tanjiro")), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sweath_Tanjiro"),
		CSweath::Create(m_pDevice, m_pContext, TEXT("Tanjiro_Sweath"), TEXT("Prototype_Component_Model_Sweath_Tanjiro")), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Zenitsu"),
		CZenitsu::Create(m_pDevice, m_pContext, TEXT("Zenitsu"), CCharacter::CHARACTER_TYPE::ZENITSU), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sword_Zenitsu"),
		CSword::Create(m_pDevice, m_pContext, TEXT("Zenitsu_Sword"), TEXT("Prototype_Component_Model_Sword_Zenitsu")), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Sweath_Zenitsu"),
		CSweath::Create(m_pDevice, m_pContext, TEXT("Zenitsu_Sweath"), TEXT("Prototype_Component_Model_Sweath_Zenitsu")), LAYER_TYPE::LAYER_CHARACTER)))
		return E_FAIL;

	m_strLoading = TEXT("���� �ε� �� �Դϴ�.");
	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Export/Weapon/")))
		return E_FAIL;


	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Export/Sky/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Npc/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Character/Tanjiro/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Character/Zenitsu/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Enemy/Boss/Enmu/")))
		return E_FAIL;

	if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Enemy/Boss/Akaza/")))
		return E_FAIL;

	//if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_ANIM, L"../Bin/Export/Enemy/Monster/")))
	//	return E_FAIL;

	//if (FAILED(GI->Ready_Model_Data_FromPath(LEVEL_STATIC, CModel::TYPE_NONANIM, L"../Bin/Export/Map/")))
	//	return E_FAIL;

	if (FAILED(Loading_Proto_AllObjects(L"../Bin/Export/Map/")))
		return E_FAIL;

	if (FAILED(Load_Map_Data(m_strFolderName)))
		return E_FAIL;

	if (FAILED(Load_Navi_Data(m_strFolderName)))
		return E_FAIL;

	//CPool<CMonster_Normal_0>::Ready_Pool(m_pDevice, m_pContext, L"Prototype_GameObject_NormalMonster_0", LAYER_TYPE::LAYER_MONSTER, nullptr, 20);
	//CPool<CMonster_Normal_1>::Ready_Pool(m_pDevice, m_pContext, L"Prototype_GameObject_NormalMonster_1", LAYER_TYPE::LAYER_MONSTER, nullptr, 20);
	//CPool<CMonster_Normal_2>::Ready_Pool(m_pDevice, m_pContext, L"Prototype_GameObject_NormalMonster_2", LAYER_TYPE::LAYER_MONSTER, nullptr, 20);

	m_strLoading = TEXT("�ε� ��.");
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Final_Boss()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Tool()
{
	/* For.ImGuiManager */
	m_strLoading = TEXT("�� �۾��� �غ����Դϴ�.");


	/* For.Shader */
	m_strLoading = TEXT("���̴��� �ε� �� �Դϴ�.");

	/* For.Prototype_Component_Shader_Model */


	m_strLoading = TEXT("��ü ������ �ε� �� �Դϴ�.");

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext, TEXT("Free_Camera")), LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Main"),
		CCamera_Main::Create(m_pDevice, m_pContext, TEXT("Main_Camera")), LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Dummy"),
		CDummy::Create(m_pDevice, m_pContext, TEXT("Dummy")), LAYER_TYPE::LAYER_PLAYER)))
		return E_FAIL;

 	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext), LAYER_TYPE::LAYER_TERRAIN)))
		return E_FAIL;

	if (FAILED(Loading_Proto_AllObjects(L"../Bin/Export/Map/")))
		return E_FAIL;
		
	

	m_strLoading = TEXT("���� �ε� �� �Դϴ�.");
	_matrix		PivotMatrix = XMMatrixIdentity();



	m_strLoading = TEXT("�ε� ��.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Navi_Data(const wstring& strNaviFileName)
{
	const wstring strNaviFilePath = L"../Bin/DataFiles/Map/" + strNaviFileName + L"/" + strNaviFileName + L".nav";
	CNavigation* pPrototypeNavigation = dynamic_cast<CNavigation*>(GI->Find_Prototype_Component(LEVEL_STATIC, L"Prototype_Component_Navigation"));

	if (nullptr == pPrototypeNavigation)
		return E_FAIL;

	if (FAILED(pPrototypeNavigation->Load_NaviData(strNaviFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Map_Data(const wstring& strMapFileName)
{
	wstring strMapFilePath = L"../Bin/DataFiles/Map/" + strMapFileName + L"/" + strMapFileName + L".map";

	shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
	File->Open(strMapFilePath, FileMode::Read);

	for (_uint i = 0; i < LAYER_TYPE::LAYER_END; ++i)
	{
		if (i == LAYER_TYPE::LAYER_CAMERA
			|| i == LAYER_TYPE::LAYER_TERRAIN
			|| i == LAYER_TYPE::LAYER_BACKGROUND
			|| i == LAYER_TYPE::LAYER_SKYBOX
			|| i == LAYER_TYPE::LAYER_UI
			|| i == LAYER_TYPE::LAYER_PLAYER
			|| i == LAYER_TYPE::LAYER_PROJECTILE
			|| i == LAYER_TYPE::LAYER_EFFECT
			|| i == LAYER_TYPE::LAYER_TRAIL
			|| i == LAYER_TYPE::LAYER_NPC)
			continue;

		// 2. ObjectCount
		_uint iObjectCount = File->Read<_uint>();

		for (_uint j = 0; j < iObjectCount; ++j)
		{
			// 3. Object_Prototype_Tag
			wstring strPrototypeTag = CUtils::ToWString(File->Read<string>());
			wstring strObjectTag = CUtils::ToWString(File->Read<string>());

			CGameObject* pObj = nullptr;
			if (FAILED(GI->Add_GameObject(m_eNextLevel, i, strPrototypeTag, nullptr, &pObj)))
			{
				MSG_BOX("Load_Objects_Failed.");
				return E_FAIL;
			}

			if (nullptr == pObj)
			{
				MSG_BOX("Add_Object_Failed.");
				return E_FAIL;
			}
			pObj->Set_ObjectTag(strObjectTag);

			CTransform* pTransform = pObj->Get_Component<CTransform>(L"Com_Transform");
			if (nullptr == pTransform)
			{
				MSG_BOX("Get_Transform_Failed.");
				return E_FAIL;
			}

			// 6. Obejct States
			_float4 vRight, vUp, vLook, vPos;

			File->Read<_float4>(vRight);
			File->Read<_float4>(vUp);
			File->Read<_float4>(vLook);
			File->Read<_float4>(vPos);

			pTransform->Set_State(CTransform::STATE_RIGHT, XMLoadFloat4(&vRight));
			pTransform->Set_State(CTransform::STATE_UP, XMLoadFloat4(&vUp));
			pTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&vLook));
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));


			for (_uint iCollider = 0; iCollider < CCollider::DETECTION_TYPE::DETECTION_END; iCollider++)
			{
				_uint iColliderCount = File->Read<_uint>();
				for (_uint iObejctColliderCount = 0; iObejctColliderCount < iColliderCount; ++iObejctColliderCount)
				{
					_uint iColliderType = File->Read<_uint>();
					_float3 vColliderOffset = File->Read<_float3>();

					if (iColliderType == CCollider::AABB)
					{
						BoundingBox tBox = File->Read<BoundingBox>();

						CCollider_AABB::AABB_COLLIDER_DESC tDesc;
						ZeroMemory(&tDesc, sizeof tDesc);

						if (nullptr == pObj->Get_Component<CModel>(L"Com_Model"))
							XMStoreFloat4x4(&tDesc.ModePivotMatrix, XMMatrixIdentity());
						else
							XMStoreFloat4x4(&tDesc.ModePivotMatrix, pObj->Get_Component<CModel>(L"Com_Model")->Get_PivotMatrix());

						tDesc.vOffsetPosition = vColliderOffset;
						tDesc.pOwnerTransform = pTransform;
						tDesc.pNode = nullptr;
						tDesc.tBox = tBox;

						pObj->Add_Collider(LEVEL_STATIC, iColliderType, iCollider, &tDesc);
					}
					else if (iColliderType == CCollider::SPHERE)
					{
						BoundingSphere tSphere = File->Read<BoundingSphere>();

						CCollider_Sphere::SPHERE_COLLIDER_DESC tDesc;
						ZeroMemory(&tDesc, sizeof tDesc);

						if (nullptr == pObj->Get_Component<CModel>(L"Com_Model"))
							XMStoreFloat4x4(&tDesc.ModePivotMatrix, XMMatrixIdentity());
						else
							XMStoreFloat4x4(&tDesc.ModePivotMatrix, pObj->Get_Component<CModel>(L"Com_Model")->Get_PivotMatrix());

						tDesc.vOffsetPosition = vColliderOffset;
						tDesc.pOwnerTransform = pTransform;
						tDesc.pNode = nullptr;
						tDesc.tSphere = tSphere;


						pObj->Add_Collider(LEVEL_STATIC, iColliderType, iCollider, &tDesc);
					}
				}
			}
		}
	}

	// MSG_BOX("Map_Loaded.");
	return S_OK;
}

HRESULT CLoader::Loading_Proto_AllObjects(const wstring& strPath)
{
	for (auto& p : std::filesystem::directory_iterator(strPath))
	{
		if (p.is_directory())
		{
			Loading_Proto_AllObjects(p.path().wstring());
		}

 		wstring strFilePath = CUtils::PathToWString(p.path().wstring());
		
		_tchar strFileName[MAX_PATH];
		_tchar strFolderName[MAX_PATH];
		_tchar strExt[MAX_PATH];

		_wsplitpath_s(strFilePath.c_str(), nullptr, 0, strFolderName, MAX_PATH, strFileName, MAX_PATH, strExt, MAX_PATH);

		if (0 == lstrcmp(TEXT(".fbx"), strExt) || 0 == lstrcmp(TEXT(".mesh"), strExt))
		{
			if (strFilePath.find(L"Buildings") != wstring::npos)
			{
				if (FAILED(GI->Add_Prototype(wstring(strFileName),
					CBuilding::Create(m_pDevice, m_pContext, wstring(strFileName), strFolderName, wstring(strFileName) + strExt), LAYER_TYPE::LAYER_BUILDING)))
				{
					return E_FAIL;
				}
			}
			else if ((strFilePath.find(L"Objects") != wstring::npos) || (strFilePath.find(L"Prop") != wstring::npos))
			{
				if (FAILED(GI->Add_Prototype(wstring(strFileName),
					CProp::Create(m_pDevice, m_pContext, wstring(strFileName), strFolderName, wstring(strFileName) + strExt), LAYER_TYPE::LAYER_PROP)))
				{
					return E_FAIL;
				}
			}
			else if (strFilePath.find(L"Grass") != wstring::npos)
			{
				if (FAILED(GI->Add_Prototype(wstring(strFileName),
					CGrass::Create(m_pDevice, m_pContext, wstring(strFileName), strFolderName, wstring(strFileName) + strExt), LAYER_TYPE::LAYER_GRASS)))
					return E_FAIL;
			}
			else if (strFilePath.find(L"Grounds") != wstring::npos)
			{
				if (FAILED(GI->Add_Prototype(wstring(strFileName),
					CGround::Create(m_pDevice, m_pContext, wstring(strFileName), strFolderName, wstring(strFileName) + strExt), LAYER_TYPE::LAYER_GROUND)))
					return E_FAIL;
			}
			else if (strFilePath.find(L"Rocks") != wstring::npos)
			{
				if (FAILED(GI->Add_Prototype(wstring(strFileName),
					CProp::Create(m_pDevice, m_pContext, wstring(strFileName), strFolderName, wstring(strFileName) + strExt), LAYER_TYPE::LAYER_GROUND)))
					return E_FAIL;
			}
			else if (strFilePath.find(L"Tree") != wstring::npos)
			{
				if (FAILED(GI->Add_Prototype(wstring(strFileName),
					CTree::Create(m_pDevice, m_pContext, wstring(strFileName), strFolderName, wstring(strFileName) + strExt), LAYER_TYPE::LAYER_TREE)))
					return E_FAIL;
			}
			else if (strFilePath.find(L"Road") != wstring::npos)
			{
				if (FAILED(GI->Add_Prototype(wstring(strFileName),
					CProp::Create(m_pDevice, m_pContext, wstring(strFileName), strFolderName, wstring(strFileName) + strExt), LAYER_TYPE::LAYER_ROAD)))
					return E_FAIL;
			}

			else if (strFilePath.find(L"Mountain") != wstring::npos)
			{
				if (FAILED(GI->Add_Prototype(wstring(strFileName),
					CProp::Create(m_pDevice, m_pContext, wstring(strFileName), strFolderName, wstring(strFileName) + strExt), LAYER_TYPE::LAYER_MOUNTAIN)))
					return E_FAIL;
			}
		}

	}
	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevel, const wstring& strFolderName)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel, strFolderName)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);	

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}