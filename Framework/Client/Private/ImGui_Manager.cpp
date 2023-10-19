#include "stdafx.h"
#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Dummy.h"
#include "Terrain.h"
#include "Key_Manager.h"
#include "Camera_Free.h"
#include "Picking_Manager.h"


#include "Prop.h"
#include "Ground.h"
#include "Mesh.h"
#include "Utils.h"
#include "FileUtils.h"
#include "Effect.h"
#include <filesystem>
#include "tinyxml2.h"

USING(Client)
IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{

}

HRESULT CImGui_Manager::Reserve_Manager(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

    m_hWnd = hWnd;

    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);


    Load_EffectsModelPath(L"../Bin/Resources/Effect/Model/");


    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    style.Alpha = m_fWindowAlpha;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\batang.ttc", 12.f, NULL, io.Fonts->GetGlyphRangesKorean());

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    return S_OK;
}

void CImGui_Manager::Tick(_float fTimeDelta)
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Tick_Basic_Tool(fTimeDelta);
    Tick_Hierachy(fTimeDelta);
    Tick_Inspector(fTimeDelta);
    

    
   
    ImGui::EndFrame();
}


void CImGui_Manager::Render_ImGui()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}




// Progress

void CImGui_Manager::Tick_Basic_Tool(_float fTimeDelta)
{
    // Loop
    ImGui::Begin("Tool");

    ImGui::Text("Window Alpha");
    IMGUI_SAME_LINE;
    if (ImGui::DragFloat("WindowAlphaSlider", &m_fWindowAlpha, 0.01f, 0.1f, 1.f, "%.1f"))
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Alpha = m_fWindowAlpha;
    }

    ImGui::BeginTabBar("##NoNameTabBar");
    ImGui::Checkbox("Demo_Window", &m_bShowDemo);

    IMGUI_NEW_LINE;

    ImGui::Checkbox("Model_Editor", &m_bShowModelWindow);
    ImGui::Checkbox("Animation", &m_bShowModelWindow);
    ImGui::Checkbox("Effect", &m_bShowEffectWindow);
    ImGui::Checkbox("Map", &m_bShowMapWindow);
    ImGui::Checkbox("Terrain", &m_bShowTerrainWindow);
  

    if (m_bShowDemo)
        ImGui::ShowDemoWindow(&m_bShowDemo);


    if (m_bShowModelWindow)
    {
        Tick_Model_Tool(fTimeDelta);
        Tick_Animation_Tool(fTimeDelta);
    }
        
    if(m_bShowEffectWindow)
        Tick_Effect_Tool(fTimeDelta);
    if(m_bShowMapWindow)
        Tick_Map_Tool(fTimeDelta);
    if(m_bShowTerrainWindow)
        Tick_Terrain_Tool(fTimeDelta);
    
    ImGui::EndTabBar();
    ImGui::End();
    
}

void CImGui_Manager::Tick_Camera_Tool(_float fTimeDelta)
{
}

void CImGui_Manager::Tick_Hierachy(_float fTimeDelta)
{
    ImGui::Begin("Hierachy");

    for (_uint i = 0; i < LAYER_TYPE::LAYER_END; ++i)
    {
        if (i == LAYER_TYPE::LAYER_CAMERA
            || i == LAYER_TYPE::LAYER_TERRAIN
            || i == LAYER_TYPE::LAYER_BACKGROUND
            || i == LAYER_TYPE::LAYER_SKYBOX
            || i == LAYER_TYPE::LAYER_UI)
            continue;


        if (ImGui::CollapsingHeader(STR_LAYER_NAME[i]))
        {
            list<CGameObject*>& GameObjects = GAME_INSTANCE->Find_GameObjects(LEVEL_TOOL, i);
            char szListBoxLable[MAX_PATH] = "##ListBox";
            strcat_s(szListBoxLable, STR_LAYER_NAME[i]);

            if (ImGui::BeginListBox(szListBoxLable, ImVec2(300, 0)))
            {
                _uint iIdx = 0;
                for (auto& Object : GameObjects)
                {
                    string ObjectTag = CUtils::ToString(Object->Get_ObjectTag());
                    string TargetObjectTag;

                    if (m_pTarget)
                        TargetObjectTag = CUtils::ToString(m_pTarget->Get_ObjectTag());
                    else
                        TargetObjectTag = "";

                    if (ImGui::Selectable(ObjectTag.c_str(), ObjectTag == TargetObjectTag, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(300, 15)))
                    {
                        m_pTarget = Object;
                        if (ImGui::IsMouseDoubleClicked(0))
                        {
                            CTransform* pCameraTransform = dynamic_cast<CTransform*>(m_pCamera->Get_Component(L"Com_Transform"));
                            CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(L"Com_Transform"));
                            
                            _float4 vObjectPosition;
                            XMStoreFloat4(&vObjectPosition, pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION));
                            
                            vObjectPosition.y += 10.f;
                            vObjectPosition.z -= 10.f;

                            
                            pCameraTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vObjectPosition));

                            vObjectPosition.y -= 10.f;
                            vObjectPosition.z += 10.f;
                            pCameraTransform->LookAt(XMLoadFloat4(&vObjectPosition));
                        }

                       
                    }
                    iIdx++;
                }

                ImGui::EndListBox();
            }

            if (KEY_TAP(KEY::DEL) && nullptr != m_pTarget)
            {
                list<CGameObject*>::iterator iter = GameObjects.begin();
                for (; iter != GameObjects.end(); ++iter)
                {
                    if ((*iter)->Get_ObjectTag() == m_pTarget->Get_ObjectTag())
                    {
                        Safe_Release(m_pTarget);
                        iter = GameObjects.erase(iter);
                        break;
                    }
                }
            }
            

            if (ImGui::Button("Auto Rename"))
            {
                _uint iIdx = 0;
                for (auto& Object : GameObjects)
                {
                    wstring strObjectTag = Object->Get_ObjectTag();

                    if (strObjectTag.find_first_of(L"0123456789") == std::string::npos)
                        strObjectTag += to_wstring(iIdx++);
                    else
                        strObjectTag.replace(strObjectTag.find_first_of(L"0123456789"), strObjectTag.size() - 1, to_wstring(iIdx++));

                    Object->Set_ObjectTag(strObjectTag);
                }
            }
        }
    }
    
    ImGui::End();
}

void CImGui_Manager::Tick_Inspector(_float fTimeDelta)
{
    ImGui::Begin("Inspector");
    if (!m_pTarget)
    {
        ImGui::End();
        return;
    }
        
    if (nullptr != m_pTarget)
    {

        static char pTargetName[MAX_PATH] = "";
        ImGui::Text("Name : ");
        IMGUI_SAME_LINE;

        ImGui::InputText("##TargetName", pTargetName, MAX_PATH);
        IMGUI_SAME_LINE;

        if (ImGui::Button("Rename"))
        {
            m_pTarget->Set_ObjectTag(CUtils::ToWString(pTargetName));
        }

        if (KEY_TAP(KEY::ENTER) && ImGui::IsWindowFocused())
        {
            m_pTarget->Set_ObjectTag(CUtils::ToWString(pTargetName));
        }


        CTransform* pTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(L"Com_Transform"));
        if (nullptr == pTransform)
        {
            ImGui::End();
            return;
        }
        IMGUI_NEW_LINE;

        ImGui::Text("Transform");
        ImGui::BeginChild("##Transform");

        IMGUI_NEW_LINE;

#pragma region Position
        // Postion
        _float3 vPos;
        XMStoreFloat3(&vPos, pTransform->Get_State(CTransform::STATE_POSITION));

        ImGui::Text("Position");
        ImGui::DragFloat3("##Position", (_float*)&vPos, 0.01f, -999.f, 999.f, "%.3f");

        pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
#pragma endregion

        IMGUI_NEW_LINE;

#pragma region Rotaion
        ImGui::Text("Rotation");
        _float3 vRotation = pTransform->Get_Rotaion_Degree();

        if (ImGui::DragFloat3("##Object_Rotation", (_float*)&vRotation, 0.1f))
        {
            vRotation.x = XMConvertToRadians(vRotation.x);
            vRotation.y = XMConvertToRadians(vRotation.y);
            vRotation.z = XMConvertToRadians(vRotation.z);

            _vector vRot = XMLoadFloat3(&vRotation);
            vRot = XMVectorSetW(vRot, 0.f);

            pTransform->Set_Rotation(vRot);
        }
#pragma endregion

        IMGUI_NEW_LINE;

#pragma region Scale
    // Scale
        _float3 vScale = pTransform->Get_Scale();

        ImGui::Text("Scale");
        ImGui::DragFloat3("##Scale", (_float*)&vScale, 0.01f, 0.01f, 100.f);

        if (vScale.x >= 0.01f
            && vScale.y >= 0.01f
            && vScale.z >= 0.01f)
            pTransform->Set_Scale(XMLoadFloat3(&vScale));
#pragma endregion

        ImGui::EndChild();
    }
    ImGui::End();
}

void CImGui_Manager::Tick_Model_Tool(_float fTimeDelta)
{
    
    ImGui::Begin("Model Editor");
    if (nullptr != m_pDummy)
    {
        if (ImGui::Button("Reset Transform"))
        {
            m_pDummy->Get_TransformCom()->Set_State(CTransform::STATE::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
            m_pDummy->Get_TransformCom()->Set_State(CTransform::STATE::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
            m_pDummy->Get_TransformCom()->Set_State(CTransform::STATE::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
            m_pDummy->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
        }


        char szFilePath[MAX_PATH];
        char szFileName[MAX_PATH];
        

        sprintf_s(szFilePath, CUtils::ToString(m_strFilePath).c_str());
        sprintf_s(szFileName, CUtils::ToString(m_strFileName).c_str());
        


        ImGui::Text("Path");
        IMGUI_SAME_LINE;
        if (ImGui::InputText("##ModelPathText", szFilePath, MAX_PATH))
        {
            m_strFilePath = CUtils::ToWString(string(szFilePath));
        }
        
        ImGui::Text("File");
        IMGUI_SAME_LINE;
        if (ImGui::InputText("##ModelFileText", szFileName, MAX_PATH))
        {
            m_strFileName = CUtils::ToWString(string(szFileName));
        }



        {
            const char* szImportModelTypes[] = { "NON_ANIM", "ANIM" };
            static const char* szImportModelType = NULL;
            static _int iSelectedImportModelType = -1;

            if (ImGui::BeginCombo("##ImportModelType", szImportModelType))
            {
                for (int n = 0; n < IM_ARRAYSIZE(szImportModelTypes); n++)
                {
                    bool is_selected = (szImportModelType == szImportModelTypes[n]); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(szImportModelTypes[n], is_selected))
                    {
                        szImportModelType = szImportModelTypes[n];
                        iSelectedImportModelType = n;
                    }

                }

                ImGui::EndCombo();
            }


            if (ImGui::Button("Import"))
            {
                if (iSelectedImportModelType != -1)
                    m_pDummy->Ready_ModelCom(iSelectedImportModelType, m_strFilePath, m_strFileName);
                else
                    MSG_BOX("모델 타입을 선택해주세요");
                
            }
        }
        


        IMGUI_NEW_LINE;

        static char szExportFolderName[MAX_PATH];
        ImGui::Text("Export_Folder_Name");
        ImGui::InputText("##ModelExportFolder", szExportFolderName, MAX_PATH);

        if (ImGui::Button("Export"))
        {
            if (strlen(szExportFolderName) > 0)
            {
                if (FAILED(m_pDummy->Export_Model_Bin(CUtils::ToWString(szExportFolderName), m_strFileName)))
                    MSG_BOX("Failed Save.");
                else
                    MSG_BOX("Save Success");
            }
        }

        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;

        {
            static char szAllObjectExportFolderName[MAX_PATH] = "";
            ImGui::Text("Export_All_Object_To_SubFolder");
            ImGui::InputText("##All_ModelExportFolder", szAllObjectExportFolderName, MAX_PATH);


            const char* szExportModelTypes[] = { "NON_ANIM", "ANIM" };
            static const char* szExportObjectModelType;
            static _int iSelectedExportModelType = -1;
            if (ImGui::BeginCombo("##ExportAllObject_ModelType", szExportObjectModelType))
            {
                for (int n = 0; n < IM_ARRAYSIZE(szExportModelTypes); n++)
                {
                    bool is_selected = (szExportObjectModelType == szExportModelTypes[n]); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(szExportModelTypes[n], is_selected))
                    {
                        szExportObjectModelType = szExportModelTypes[n];
                        iSelectedExportModelType = n;
                    }

                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Export_All"))
            {
                if (0 != strcmp(szAllObjectExportFolderName, "") && iSelectedExportModelType != -1)
                {
                    GI->Export_Model_Data_FromPath(iSelectedExportModelType, CUtils::ToWString(szAllObjectExportFolderName));
                }
                else
                {
                    MSG_BOX("폴더 경로 혹은 모델 타입 지정을 확인하세요.");
                }
            }
        }
    }
    ImGui::End();
}


#pragma region Animation
void CImGui_Manager::Tick_Animation_Tool(_float fTimeDelta)
{
    
    ImGui::Begin("Animation");

    static char szAnimationName[255];
    
    if (nullptr != m_pDummy->Get_ModelCom())
    {
        CModel* pModelCom = m_pDummy->Get_ModelCom();
        const vector<CAnimation*>& Animations = pModelCom->Get_Animations();

        if (CModel::TYPE::TYPE_NONANIM == pModelCom->Get_ModelType())
        {
            ImGui::End();
            return;
        }
        


        // AnimationList
        if (ImGui::BeginListBox("##Animation_List"))
        {
            for(size_t i = 0; i< Animations.size(); ++i)
            {
                string AnimationName = CUtils::ToString(Animations[i]->Get_AnimationName());
                if (ImGui::Selectable(AnimationName.c_str(), i == pModelCom->Get_CurrAnimationIndex()))
                {
                    pModelCom->Set_AnimIndex(i);
                    sprintf_s(szAnimationName, CUtils::ToString(Animations[pModelCom->Get_CurrAnimationIndex()]->Get_AnimationName()).c_str());
                }
            }

            ImGui::EndListBox();
        }
        IMGUI_SAME_LINE;

        ImGui::BeginGroup();
        if (ImGui::ArrowButton("##Swap_Animation_Up", ImGuiDir_Up))
        {
            pModelCom->Swap_Animation(pModelCom->Get_CurrAnimationIndex(), pModelCom->Get_CurrAnimationIndex() - 1);
        }
        IMGUI_SAME_LINE;
        if (ImGui::ArrowButton("##Swap_Animation_Down", ImGuiDir_Down))
        {
            pModelCom->Swap_Animation(pModelCom->Get_CurrAnimationIndex(), pModelCom->Get_CurrAnimationIndex() + 1);
        }

        

        
        
        ImGui::InputText("##Animation_Input_Name", szAnimationName, 255);
        if(ImGui::Button("Rename"))
        {
            wstring NewAnimationName = CUtils::ToWString(string(szAnimationName));
            if (NewAnimationName.size() > 0)
                Animations[pModelCom->Get_CurrAnimationIndex()]->Set_AnimationName(NewAnimationName);
        }

        if (KEY_TAP(KEY::ENTER) && ImGui::IsWindowFocused())
        {
            wstring NewAnimationName = CUtils::ToWString(string(szAnimationName));
            if (NewAnimationName.size() > 0)
                Animations[pModelCom->Get_CurrAnimationIndex()]->Set_AnimationName(NewAnimationName);
        }

        if (ImGui::Button("Delete") || (KEY_TAP(KEY::DEL) && ImGui::IsWindowFocused()))
           pModelCom->Delete_Animation(pModelCom->Get_CurrAnimationIndex());

        ImGui::EndGroup();

        // Animation Slider
        CAnimation* pCurrAnimation = Animations[pModelCom->Get_CurrAnimationIndex()];

        _float fPlayTime = pCurrAnimation->Get_PlayTime();
        if (ImGui::SliderFloat("##Animation_PlayTime", &fPlayTime, 0.f, pCurrAnimation->Get_Duration()))
        {
            pCurrAnimation->Set_AnimationPlayTime(m_pDummy->Get_TransformCom(), fPlayTime, fTimeDelta);
        }


        if (ImGui::ArrowButton("##Play_AnimationButton", ImGuiDir_Right))
            pCurrAnimation->Set_Pause(false);

        IMGUI_SAME_LINE;

        if (ImGui::Button("||"))
            pCurrAnimation->Set_Pause(true);

        IMGUI_SAME_LINE;
        if (ImGui::Button("Sort"))
        {
            vector<class CAnimation*>& Animations = pModelCom->Get_Animations();
            sort(Animations.begin(), Animations.end(), [&](CAnimation* pSrcAnimation, CAnimation* pDestAnimation) {
                return pSrcAnimation->Get_AnimationName() < pDestAnimation->Get_AnimationName();
            });
        }

        _float fSpeed = pCurrAnimation->Get_AnimationSpeed();
        ImGui::Text("Speed");
        IMGUI_SAME_LINE;
        if (ImGui::DragFloat("##AnimationSpeed", &fSpeed, 0.01f, 0.f, 100.f))
        {
            pCurrAnimation->Set_AnimationSpeed(fSpeed);
        }

        IMGUI_NEW_LINE;

        _bool bRootAnimation = pCurrAnimation->Is_RootAnimation();
        ImGui::Text("Root_Animation");
        IMGUI_SAME_LINE;
        ImGui::Checkbox("##IsRootAnimation", &bRootAnimation);
        pCurrAnimation->Set_RootAnimation(bRootAnimation);

        _bool bLoop = pCurrAnimation->Is_Loop();
        ImGui::Text("Loop");
        IMGUI_SAME_LINE;
        ImGui::Checkbox("##IsLoop", &bLoop);
        pCurrAnimation->Set_Loop(bLoop);
    }
    ImGui::End();
    
}
#pragma endregion

void CImGui_Manager::Tick_Effect_Tool(_float fTimeDelta)
{

    ImGui::Begin("Effect");



    static char szEffectModelName[MAX_PATH] = "Temp";
    if (ImGui::BeginListBox("##Effect_Model_List", ImVec2(200.f, 200.f)))
    {
        for (size_t i = 0; i < m_EffectsModelFiles.size(); ++i)
        {
            if (ImGui::Selectable(szEffectModelName, CUtils::ToWString(szEffectModelName) == m_EffectsModelFiles[i]))
            {
                strcpy_s(szEffectModelName, CUtils::ToString(m_EffectsModelFiles[i]).c_str());
            }
        }
        ImGui::EndListBox();
    }



    const char* szEffectTypes[] = { "EFFECT_TEXTURE", "EFFECT_MESH" };

    static const char* szEffectType;
    static _int iSelecetedEffectType = -1;
    if (ImGui::BeginCombo("##Generate_EffectType", szEffectType))
    {
        for (int n = 0; n < IM_ARRAYSIZE(szEffectTypes); n++)
        {
            bool is_selected = (szEffectType == szEffectTypes[n]); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(szEffectTypes[n], is_selected))
            {
                szEffectType = szEffectTypes[n];
                iSelecetedEffectType = n;
            }

        }
        ImGui::EndCombo();
    }


    if (ImGui::Button("Generate_Effect"))
    {
        if (iSelecetedEffectType < 0)
        {
            MSG_BOX("이펙트 타입을 설정하세요.");
            ImGui::End();
            return;
        }

        if (iSelecetedEffectType == CEffect::EFFECT_TYPE::EFFECT_MESH
            && 0 == strcmp(szEffectModelName, ""))
        {
            MSG_BOX("모델 이펙트는 프로토 타입을 선택 해야합니다.");
            ImGui::End();
            return;
        }

        if (nullptr != m_pPrevEffect)
            Safe_Release(m_pPrevEffect);

        CEffect::EFFECT_DESC tEffectDesc;
        if (iSelecetedEffectType == CEffect::EFFECT_TYPE::EFFECT_MESH)
            m_pPrevEffect = CEffect::Create(m_pDevice, m_pContext, L"Prev_Effect", CEffect::EFFECT_TYPE(iSelecetedEffectType), CUtils::ToWString(szEffectModelName), tEffectDesc);
        else
            m_pPrevEffect = CEffect::Create(m_pDevice, m_pContext, L"Prev_Effect", CEffect::EFFECT_TYPE(iSelecetedEffectType), L"", tEffectDesc);


        if (nullptr == m_pPrevEffect)
        {
            MSG_BOX("이펙트 생성 실패.");
            ImGui::End();
            return;
        }
        else
        {
            if (FAILED(m_pPrevEffect->Initialize(nullptr)))
            {
                MSG_BOX("이펙트 초기화 실패.");
                Safe_Release(m_pPrevEffect);
                ImGui::End();
                return;
            }
        }
    }



    if (nullptr != m_pPrevEffect)
    {
        CEffect::EFFECT_DESC tEffectDesc = m_pPrevEffect->Get_EffectDesc();
        CTransform* pTransform = m_pPrevEffect->Get_TransformCom();

        CTexture* pDiffuseTexture = m_pPrevEffect->Get_DiffuseTexture();
        CTexture* pAlphaTexture = m_pPrevEffect->Get_DiffuseTexture();

        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Texture");
        ImGui::Text("============================================");
        {
            ImGui::Text("Diffuse Texture");
            if (ImGui::BeginListBox("##Effect_DiffuseTexture_List", ImVec2(200.f, 300.f)))
            {
                for (size_t i = 0; i < pDiffuseTexture->Get_TextureCount(); ++i)
                {
                    if (ImGui::ImageButton(pDiffuseTexture->Get_Srv(i), ImVec2(150.f, 150.f)))
                    {
                        tEffectDesc.iDiffuseTextureIndex = i;
                    }
                }
                ImGui::EndListBox();
            }

            IMGUI_SAME_LINE;
            if (ImGui::Button("Reset_Diffuse"))
            {
                tEffectDesc.iDiffuseTextureIndex = -1;
            }

        }

        IMGUI_NEW_LINE;
        {
            ImGui::Text("Alpha Texture");
            if (ImGui::BeginListBox("##Effect_AlphaTexture_List", ImVec2(200.f, 300.f)))
            {
                for (size_t i = 0; i < pAlphaTexture->Get_TextureCount(); ++i)
                {
                    if (ImGui::ImageButton(pAlphaTexture->Get_Srv(i), ImVec2(150.f, 150.f)))
                    {
                        tEffectDesc.iAlphaTextureIndex = i;
                    }
                }
                ImGui::EndListBox();
            }

            IMGUI_SAME_LINE;
            if (ImGui::Button("Reset_Alpha"))
            {
                tEffectDesc.iAlphaTextureIndex = -1;
            }
        }



        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Texture & Color");
        ImGui::Text("============================================");

        IMGUI_NEW_LINE;
        ImGui::Text("ARGB");
        ImGui::DragFloat3("##AddtiveDiffuseColor", (_float*)&tEffectDesc.fAdditiveDiffuseColor, 0.01f, 0.f, 1.f);
        ImGui::DragFloat("##AddtiveAlpha", &tEffectDesc.fAlpha, 0.01f, 0.f, 1.f);


        IMGUI_NEW_LINE;
        ImGui::Text("UV_Speed : ");
        ImGui::DragFloat("##Effect_UV_Speed", &tEffectDesc.fIndexSpeed, 0.01f, 0.f, 100.f);

        
        IMGUI_NEW_LINE;
        ImGui::Text("Max_Count_X : ");
        ImGui::DragFloat("##Max_CountX", &tEffectDesc.fMaxCountX, 1.f, 1.f, 100.f);

        IMGUI_NEW_LINE;
        ImGui::Text("Max_Count_Y : ");
        ImGui::DragFloat("##Max_CountY", &tEffectDesc.fMaxCountY, 1.f, 1.f, 100.f);


        
        



        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Scale");
        ImGui::Text("============================================");

        _float3 vScale = pTransform->Get_Scale();
        ImGui::DragFloat3("##Effect_Scale", (_float*)&vScale, 0.01f, 0.01f, 100.f);
        if(vScale.x >= 0.01f && vScale.y >= 0.01f && vScale.z >= 0.01f)
            pTransform->Set_Sclae(vScale);


        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Rotation");
        ImGui::Text("============================================");



        
        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Movement");
        ImGui::Text("============================================");

        if(ImGui::Button("ResetTransform"))
        {
            if (nullptr != pTransform)
            {
                _float3 vScale = pTransform->Get_Scale();
                pTransform->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x);
                pTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y);
                pTransform->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z);
                pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
            }
        }

        IMGUI_NEW_LINE;

        ImGui::Text("Move_Speed : ");
        IMGUI_SAME_LINE;
        ImGui::DragFloat("##Effect_Move_Speed", &tEffectDesc.fMoveSpeed, 0.f, 0.01f, 100.f);

        ImGui::Text("Move_Direction : ");
        IMGUI_SAME_LINE;
        ImGui::DragFloat3("##Effect_Move_Direction", (_float*)&tEffectDesc.vMoveDir, 0.f, 0.01f, 100.f);

        ImGui::Text("Turn_Speed : ");
        IMGUI_SAME_LINE;
        ImGui::DragFloat("##Effect_Turn_Speed", &tEffectDesc.fTurnSpeed, 0.f, 0.01f, 100.f);

        ImGui::Text("Turn_Direction : ");
        IMGUI_SAME_LINE;
        ImGui::DragFloat3("##Effect_Turn_Direction", (_float*)&tEffectDesc.vTurnDir, 0.f, 0.01f, 100.f);


        
        m_pPrevEffect->Set_EffectDesc(tEffectDesc);


        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Save & Load");
        ImGui::Text("============================================");

        static char szExportEffectFolder[MAX_PATH] = "";
        static char szExportEffectName[MAX_PATH] = "";

        ImGui::Text("Effect_ExportSubFolder");
        IMGUI_SAME_LINE;
        ImGui::InputText("##EffectExportSubFolder", szExportEffectFolder, MAX_PATH);

        ImGui::Text("Effect_Name");
        IMGUI_SAME_LINE;
        ImGui::InputText("##EffectExportName", szExportEffectName, MAX_PATH);
        if (ImGui::Button("Save_Effect"))
        {
            if (0 != strcmp(szExportEffectName, ""))
            {
                char szFullPath[MAX_PATH] = "../Bin/Export/Effect/";

                strcat_s(szFullPath, szExportEffectFolder);
                strcat_s(szFullPath, "/");
                strcat_s(szFullPath, szExportEffectName);
                strcat_s(szFullPath, ".effect");

                Save_Effect(CUtils::ToWString(szFullPath));
            }
            else
            {
                MSG_BOX("이름을 입력해주세요.");
            }
        }

        m_pPrevEffect->Tick(fTimeDelta);
        m_pPrevEffect->LateTick(fTimeDelta);
    }


    ImGui::End();
    
}

void CImGui_Manager::Tick_Map_Tool(_float fTimeDelta)
{
    ImGui::Begin("Map");

    static char pMapFileName[MAX_PATH] = "Temp";

    ImGui::Text("Map_Name");
    IMGUI_SAME_LINE;
    ImGui::InputText("##Map_FileName", pMapFileName, MAX_PATH);
    if (ImGui::Button("Save_Map"))
        Save_Map_Data(CUtils::ToWString(pMapFileName));

    IMGUI_SAME_LINE;

    if (ImGui::Button("Load_Map"))
        Load_Map_Data(CUtils::ToWString(pMapFileName));


    IMGUI_NEW_LINE;
    static char pSelectedObjectName[MAX_PATH] = "Temp";
    static _bool bSelected = false;
    static LAYER_TYPE eSelectedLayer = LAYER_TYPE::LAYER_END;
    static string strPrototypeName;

    

    for (_uint i = 0; i < LAYER_TYPE::LAYER_END; ++i)
    {
        if (i == LAYER_TYPE::LAYER_CAMERA
            || i == LAYER_TYPE::LAYER_TERRAIN
            || i == LAYER_TYPE::LAYER_BACKGROUND
            || i == LAYER_TYPE::LAYER_SKYBOX
            || i == LAYER_TYPE::LAYER_UI)
            continue;

        const map<const wstring, CGameObject*>& PrototypeObjects = GAME_INSTANCE->Find_Prototype_GameObjects(i);

        char szListBoxTag[MAX_PATH] = "##Object_List";
        strcat_s(szListBoxTag, to_string(i).c_str());

        
        if (ImGui::CollapsingHeader(STR_LAYER_NAME[i]))
        {
            if (ImGui::BeginListBox(szListBoxTag))
            {
                for (auto& iter : PrototypeObjects)
                {
                    strPrototypeName = CUtils::ToString(iter.first);
                    if (ImGui::Selectable(strPrototypeName.c_str(), !strcmp(pSelectedObjectName, strPrototypeName.c_str())))
                    {
                        m_pPrevObject = iter.second->Clone(nullptr);

                        strcpy_s(pSelectedObjectName, strPrototypeName.c_str());
                        eSelectedLayer = LAYER_TYPE(i);
                    }
                }
                ImGui::EndListBox();
            }
        }
    }


    if (nullptr != m_pPrevObject && nullptr != m_pTerrain)
    {
        PickingGroundObj();


        if (KEY_TAP(KEY::LBTN))
        {
            CGameObject* pCloneObject = m_pPrevObject->Clone(nullptr);
            if (nullptr == pCloneObject)
            {
                MSG_BOX("Clone_Failed.");
                Safe_Release(m_pPrevObject);
                m_pPrevObject = nullptr;
            }
            CTransform* pPrevObjTransform = dynamic_cast<CTransform*>(m_pPrevObject->Get_Component(L"Com_Transform"));
            CTransform* pObjectTransform = dynamic_cast<CTransform*>(pCloneObject->Get_Component(L"Com_Transform"));
            if (pObjectTransform == nullptr)
            {
                MSG_BOX("Get_TransformCom Failed.");
                Safe_Release(m_pPrevObject);
                m_pPrevObject = nullptr;
            }

            pObjectTransform->Set_State(CTransform::STATE::STATE_RIGHT, pPrevObjTransform->Get_State(CTransform::STATE::STATE_RIGHT));
            pObjectTransform->Set_State(CTransform::STATE::STATE_UP, pPrevObjTransform->Get_State(CTransform::STATE::STATE_UP));
            pObjectTransform->Set_State(CTransform::STATE::STATE_LOOK, pPrevObjTransform->Get_State(CTransform::STATE::STATE_LOOK));
            pObjectTransform->Set_State(CTransform::STATE::STATE_POSITION, pPrevObjTransform->Get_State(CTransform::STATE_POSITION));

            if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TOOL, eSelectedLayer, pCloneObject)))
            {
                MSG_BOX("Add_GameObject Failed.");
                Safe_Release(m_pPrevObject);
                Safe_Release(pCloneObject);
                m_pPrevObject = nullptr;
            }
        }

        if (KEY_TAP(KEY::OPEN_SQUARE_BRACKET))
        {
            CTransform* pTransform = dynamic_cast<CTransform*>(m_pPrevObject->Get_Component(L"Com_Transform"));
            _float3 vScale = pTransform->Get_Scale();
            vScale.x = clamp(vScale.x -= 1.f, 1.f, 999.f);
            vScale.y = clamp(vScale.y -= 1.f, 1.f, 999.f);
            vScale.z = clamp(vScale.z -= 1.f, 1.f, 999.f);
            pTransform->Set_Scale(XMLoadFloat3(&vScale));
        }

        if (KEY_TAP(KEY::CLOSE_SQUARE_BRACKET))
        {
            CTransform* pTransform = dynamic_cast<CTransform*>(m_pPrevObject->Get_Component(L"Com_Transform"));
            _float3 vScale = pTransform->Get_Scale();
            vScale.x = clamp(vScale.x += 1.f, 1.f, 999.f);
            vScale.y = clamp(vScale.y += 1.f, 1.f, 999.f);
            vScale.z = clamp(vScale.z += 1.f, 1.f, 999.f);
            pTransform->Set_Scale(XMLoadFloat3(&vScale));
        }

        if (KEY_TAP(KEY::R))
        {
            CTransform* pTransform = dynamic_cast<CTransform*>(m_pPrevObject->Get_Component(L"Com_Transform"));
            pTransform->Rotation_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));
        }

        m_pPrevObject->Tick(fTimeDelta);
        m_pPrevObject->LateTick(fTimeDelta);

        if (KEY_TAP(KEY::RBTN))
        {
            Safe_Release(m_pPrevObject);
            m_pPrevObject = nullptr;
        }
    }
    


    ImGui::End();
}

void CImGui_Manager::Tick_Terrain_Tool(_float fTimeDelta)
{
    ImGui::Begin("Terrain");
    ImGui::Text("Terrain Tool");
    if (nullptr != m_pTerrain)
    {
        static _bool bDrawGrid = true;
        ImGui::Text("Draw_Grid");
        IMGUI_SAME_LINE;
        ImGui::Checkbox("##DrawGrid", &bDrawGrid);
        m_pTerrain->Set_DrawGrid(bDrawGrid);
        

        IMGUI_NEW_LINE;
        CTransform* pTransform = m_pTerrain->Get_TransformCom();

#pragma region Position
        // Postion
        _float3 vPos;
        XMStoreFloat3(&vPos, pTransform->Get_State(CTransform::STATE_POSITION));

        ImGui::Text("Position");
        ImGui::DragFloat3("##Position", (_float*)&vPos, 0.1f, -999.f, 999.f, "%.3f");

        pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
#pragma endregion

        IMGUI_NEW_LINE;
#pragma region Scale
        // Scale
        _float3 vScale = pTransform->Get_Scale();

        ImGui::Text("Scale");
        ImGui::DragFloat3("##Scale", (_float*)&vScale, 0.01f, 0.01f, 100.f);

        if (vScale.x >= 0.01f
            && vScale.y >= 0.01f
            && vScale.z >= 0.01f)
            pTransform->Set_Scale(XMLoadFloat3(&vScale));
#pragma endregion

    }
    ImGui::End();
}

HRESULT CImGui_Manager::Load_Map_Data(const wstring& strMapFileName)
{
    wstring strMapFilePath = L"../Bin/Data/Map/" + strMapFileName + L"/" + strMapFileName + L".map";

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
            || i == LAYER_TYPE::LAYER_EFFECT)
            continue;

        GI->Clear_Layer(LEVEL_TOOL, i);

        // 2. ObjectCount
        _uint iObjectCount = File->Read<_uint>();

        for (_uint j = 0; j < iObjectCount; ++j)
        {
            // 3. Object_Prototype_Tag
            wstring strPrototypeTag = CUtils::ToWString(File->Read<string>());
            wstring strObjectTag = CUtils::ToWString(File->Read<string>());

            CGameObject* pObj = nullptr;
            if (FAILED(GI->Add_GameObject(LEVEL_TOOL, i, strPrototypeTag, nullptr, &pObj)))
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

            CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Get_Component(L"Com_Transform"));
            if(nullptr == pTransform)
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
        }
    }
    MSG_BOX("Map_Loaded.");
    return S_OK;
}

HRESULT CImGui_Manager::Save_Map_Data(const wstring& strMapFileName)
{
    wstring strMapFilePath = L"../Bin/Data/Map/" + strMapFileName + L"/" + strMapFileName + L".map";

    auto path = filesystem::path(strMapFilePath);
    filesystem::create_directories(path.parent_path());


    shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
    File->Open(strMapFilePath, FileMode::Write);
    for (_uint i = 0; i < LAYER_TYPE::LAYER_END; ++i)
    {
        if (i == LAYER_TYPE::LAYER_CAMERA
            || i == LAYER_TYPE::LAYER_TERRAIN
            || i == LAYER_TYPE::LAYER_BACKGROUND
            || i == LAYER_TYPE::LAYER_SKYBOX
            || i == LAYER_TYPE::LAYER_UI
            || i == LAYER_TYPE::LAYER_PLAYER
            || i == LAYER_TYPE::LAYER_PROJECTILE
            || i == LAYER_TYPE::LAYER_EFFECT)
            continue;

        // 2. ObjectCount
        list<CGameObject*>& GameObjects = GI->Find_GameObjects(LEVEL_TOOL, i);
        File->Write<_uint>(GameObjects.size());

        for (auto& Object : GameObjects)
        {
            CTransform* pTransform = dynamic_cast<CTransform*>(Object->Get_Component(L"Com_Transform"));
            if (nullptr == pTransform)
            {
                MSG_BOX("Find_Transform_Failed.");
                return E_FAIL;
            }

            // 3. Object_Prototype_Tag
            File->Write<string>(CUtils::ToString(Object->Get_PrototypeTag()));

            // 4. Object_Tag
            File->Write<string>(CUtils::ToString(Object->Get_ObjectTag()));

            // 5. Obejct States
            _float4 vRight, vUp, vLook, vPos;

            XMStoreFloat4(&vRight, pTransform->Get_State(CTransform::STATE_RIGHT));
            XMStoreFloat4(&vUp, pTransform->Get_State(CTransform::STATE_UP));
            XMStoreFloat4(&vLook, pTransform->Get_State(CTransform::STATE_LOOK));
            XMStoreFloat4(&vPos, pTransform->Get_State(CTransform::STATE_POSITION));

            File->Write<_float4>(vRight);
            File->Write<_float4>(vUp);
            File->Write<_float4>(vLook);
            File->Write<_float4>(vPos);
        }

    }

    MSG_BOX("Map_Saved.");
    return S_OK;
}

HRESULT CImGui_Manager::Save_Effect(const wstring& strFullPath)
{
    auto path = filesystem::path(strFullPath);
    if (filesystem::exists(path))
    {
        MSG_BOX("File is Exist. Press Other Name.");
        return E_FAIL;
    }
        
    

    filesystem::create_directories(path.parent_path());

    _tchar strFileName[MAX_PATH];
    _wsplitpath_s(strFullPath.c_str(), nullptr, 0, nullptr, 0, strFileName, MAX_PATH, nullptr, 0);


    shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
    File->Open(strFullPath, FileMode::Write);

    File->Write<CEffect::EFFECT_TYPE>(m_pPrevEffect->Get_EffectType());
    
    const CEffect::EFFECT_DESC& tDesc = m_pPrevEffect->Get_EffectDesc();
    File->Write<_float>(tDesc.fTurnSpeed);
    File->Write<_float>(tDesc.fMoveSpeed);
    File->Write<_float>(tDesc.fMaxCountX);
    File->Write<_float>(tDesc.fMaxCountY);
    File->Write<_float>(tDesc.fAlpha);
    File->Write<_float>(tDesc.fIndexSpeed);
    File->Write<_float3>(tDesc.fAdditiveDiffuseColor);
    File->Write<_float3>(tDesc.vMoveDir);
    File->Write<_float3>(tDesc.vTurnDir);


    File->Write<_bool>(m_pPrevEffect->Is_Loop());
    File->Write<_bool>(m_pPrevEffect->Is_Increment());

    File->Write<_float3>(m_pPrevEffect->Get_TransformCom()->Get_Scale());
 

    MSG_BOX("Effect Saved.");
    return S_OK;
}

HRESULT CImGui_Manager::Load_Effect(const wstring& strFullPath)
{
    auto path = filesystem::path(strFullPath);
    filesystem::create_directories(path.parent_path());

    _tchar strFileName[MAX_PATH];
    _wsplitpath_s(strFullPath.c_str(), nullptr, 0, nullptr, 0, strFileName, MAX_PATH, nullptr, 0);


    shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
    File->Open(strFullPath, FileMode::Write);

    File->Write<CEffect::EFFECT_TYPE>(m_pPrevEffect->Get_EffectType());

    const CEffect::EFFECT_DESC& tDesc = m_pPrevEffect->Get_EffectDesc();
    File->Write<_float>(tDesc.fTurnSpeed);
    File->Write<_float>(tDesc.fMoveSpeed);
    File->Write<_float>(tDesc.fMaxCountX);
    File->Write<_float>(tDesc.fMaxCountY);
    File->Write<_float>(tDesc.fAlpha);
    File->Write<_float>(tDesc.fIndexSpeed);
    File->Write<_float3>(tDesc.fAdditiveDiffuseColor);
    File->Write<_float3>(tDesc.vMoveDir);
    File->Write<_float3>(tDesc.vTurnDir);


    File->Write<_bool>(m_pPrevEffect->Is_Loop());
    File->Write<_bool>(m_pPrevEffect->Is_Increment());

    File->Write<_float3>(m_pPrevEffect->Get_TransformCom()->Get_Scale());

    return S_OK;
}

HRESULT CImGui_Manager::Load_EffectsModelPath(const wstring& strEffectPath)
{

    for (auto& p : std::filesystem::directory_iterator(strEffectPath))
    {
        if (p.is_directory())
            Load_EffectsModelPath(p.path());

        wstring strFullPath = CUtils::PathToWString(p.path().wstring());
        _tchar strFileName[MAX_PATH];
        _tchar strFolderName[MAX_PATH];
        _tchar strExt[MAX_PATH];

        _wsplitpath_s(strFullPath.c_str(), nullptr, 0, strFolderName, MAX_PATH, strFileName, MAX_PATH, strExt, MAX_PATH);


        if (0 == lstrcmp(TEXT(".fbx"), strExt) || 0 == lstrcmp(TEXT(".mesh"), strExt))
        {
            if(FAILED(GI->Import_Model_Data(LEVEL_STATIC, 
                wstring(L"Prototype_Model_") + strFileName, 
                CModel::TYPE::TYPE_NONANIM, 
                strFolderName, 
                wstring(strFileName) + strExt, nullptr)))
                return E_FAIL;

            m_EffectsModelFiles.push_back(wstring(L"Prototype_Model_") + strFileName);
        }
    }

    return S_OK;
}

void CImGui_Manager::PickingTerrainObj()
{
    _float4 vHitPos;
    if (CPicking_Manager::GetInstance()->Is_Picking(m_pTerrain->Get_TransformCom(), m_pTerrain->Get_TerrainBufferCom(), true, &vHitPos))
    {
        CTransform* pTransform = dynamic_cast<CTransform*>(m_pPrevObject->Get_Component(L"Com_Transform"));
        if (nullptr != pTransform)
            pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vHitPos));

        return;
    }

}


void CImGui_Manager::PickingGroundObj()
{
    _float4 vHitPos;
    list<CGameObject*>& PickingObjects = GI->Find_GameObjects(LEVEL_TOOL, LAYER_TYPE::LAYER_GROUND);
    for (auto& PickingObject : PickingObjects)
    {
        CGround* pGround = dynamic_cast<CGround*>(PickingObject);
        if (nullptr == pGround)
            continue;

        CTransform* pTransform = pGround->Get_TransformCom();
        if (nullptr == pTransform)
            continue;


        const vector<CMesh*>& Meshes = pGround->Get_ModelCom()->Get_Meshes();
        for (auto& pMesh : Meshes)
        {
            if (CPicking_Manager::GetInstance()->Is_Picking(pTransform, pMesh, false, &vHitPos))
            {
                CTransform* pTransform = dynamic_cast<CTransform*>(m_pPrevObject->Get_Component(L"Com_Transform"));
                if (nullptr != pTransform)
                    pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vHitPos));

                return;
            }
        }
    }

    PickingTerrainObj();
}


void CImGui_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);


    m_pTarget = nullptr;
    m_pDummy = nullptr;
    m_pTerrain = nullptr;
    

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

}

