#include "stdafx.h"
#include "ImGui_Manager.h"
#include "GameObject.h"
#include "Component_Manager.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Dummy.h"
#include "Terrain.h"
#include "Key_Manager.h"
#include "Utils.h"
#include "Mesh_Effect.h"
#include "Camera_Free.h"

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

    ImGui_ImplWin32_Init(hWnd);
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

    ImGui::BeginTabBar("");
    ImGui::Checkbox("Demo_Window", &m_bShowDemo);

    IMGUI_NEW_LINE;

    ImGui::Checkbox("Model Editor", &m_bShowModelWindow);
    ImGui::Checkbox("Animation", &m_bShowModelWindow);
    ImGui::Checkbox("Effect", &m_bShowEffectWindow);
    ImGui::Checkbox("Map", &m_bShowMapWindow);
    ImGui::Checkbox("Terrain", &m_bShowTerrainWindow);

    if (m_bShowDemo)
        ImGui::ShowDemoWindow(&m_bShowDemo);

    Tick_Hierachy(fTimeDelta);
    Tick_Inspector(fTimeDelta);

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

#pragma region Rotaion_TODO
        // Rotation
        /*_float3 fRotation;

        _float4 vRight, vUp, vLook;
        XMStoreFloat4(&vRight, pTransform->Get_State(CTransform::STATE::STATE_RIGHT));
        XMStoreFloat4(&vUp, pTransform->Get_State(CTransform::STATE::STATE_UP));
        XMStoreFloat4(&vLook, pTransform->Get_State(CTransform::STATE::STATE_LOOK));


        ImGui::Text("Rotaion");
        if (ImGui::DragFloat3("##Rotation_x", (_float*)&vRight, 0.1f, 0.f, 360.f))
        {
            pTransform->Turn
        }*/

        /*pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fRotaionX));
        pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRotaionY));
        pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRotaionZ));*/
        // IMGUI_NEW_LINE;
#pragma endregion

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
        CTransform* pTransform = dynamic_cast<CTransform*>(m_pDummy->Get_Component(L"Com_Transform"));
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

    #pragma region Rotaion_TODO
        // Rotation
        /*_float3 fRotation;

        _float4 vRight, vUp, vLook;
        XMStoreFloat4(&vRight, pTransform->Get_State(CTransform::STATE::STATE_RIGHT));
        XMStoreFloat4(&vUp, pTransform->Get_State(CTransform::STATE::STATE_UP));
        XMStoreFloat4(&vLook, pTransform->Get_State(CTransform::STATE::STATE_LOOK));


        ImGui::Text("Rotaion");
        if (ImGui::DragFloat3("##Rotation_x", (_float*)&vRight, 0.1f, 0.f, 360.f))
        {
            pTransform->Turn
        }*/

        /*pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fRotaionX));
        pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRotaionY));
        pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRotaionZ));*/
        // IMGUI_NEW_LINE;
    #pragma endregion

    #pragma region Scale
        // Scale
        _float3 vScale = pTransform->Get_Scale();

        ImGui::Text("Scale");
        ImGui::DragFloat3("##Scale", (_float*)&vScale, 0.01f, 0.01f, 100.f);

        if(vScale.x >= 0.01f
            && vScale.y >= 0.01f
            && vScale.z >= 0.01f)
            pTransform->Set_Scale(XMLoadFloat3(&vScale));
    #pragma endregion
        

        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;

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


        static char szExportFolderName[MAX_PATH];
        ImGui::Text("Export_Folder_Name");
        ImGui::InputText("##ModelExportFolder", szExportFolderName, MAX_PATH);
        


        const char* items[] = { "NON_ANIM", "ANIM"};
        static const char* szCurrent = NULL;
        if (ImGui::BeginCombo("##ModelType", szCurrent))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (szCurrent == items[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(items[n], is_selected))
                {
                    szCurrent = items[n];
                    m_iSelectedModelType = n;
                }
                    
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Import"))
        {
            m_pDummy->Ready_ModelCom(m_iSelectedModelType, m_strFilePath, m_strFileName);
        }

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

        static char szAllObjectExportFolderName[MAX_PATH] = "";
        ImGui::Text("Export_All_Object_To_SubFolder");
        ImGui::InputText("##All_ModelExportFolder", szAllObjectExportFolderName, MAX_PATH);


        const char* szArrModelTypes[] = { "NON_ANIM", "ANIM" };
        static const char* szAllObjectModelType;
        static _int iSelectedModelType = -1;
        if (ImGui::BeginCombo("##AllObject_ModelType", szAllObjectModelType))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (szAllObjectModelType == items[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(items[n], is_selected))
                {
                    szAllObjectModelType = szArrModelTypes[n];
                    iSelectedModelType = n;
                }

            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Export_All"))
        {
            if (0 != strcmp(szAllObjectExportFolderName, "") && iSelectedModelType != -1)
            {
                if (FAILED(GAME_INSTANCE->Export_Model_Data_FromPath(iSelectedModelType, CUtils::ToWString(szAllObjectExportFolderName))))
                    MSG_BOX("Export Failed.");
                else
                    MSG_BOX("Export Complete!");
            }
            else
            {
                MSG_BOX("폴더 경로를 입력하세요.");
            }
        }

        ImGui::EndChild();
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

        if (ImGui::ArrowButton("##Play_AnimationButton", ImGuiDir_Right))
            pCurrAnimation->Set_Pause(false);

        IMGUI_SAME_LINE;

        if (ImGui::Button("||"))
            pCurrAnimation->Set_Pause(true);


    }
    ImGui::End();
    
}
#pragma endregion

void CImGui_Manager::Tick_Effect_Tool(_float fTimeDelta)
{
    ImGui::Begin("Effect");
    if (m_pPrevEffect != nullptr)
    {
        m_pPrevEffect->Tick(fTimeDelta);
        m_pPrevEffect->LateTick(fTimeDelta);

        const CEffect::EFFECT_DESC& tDesc = m_pPrevEffect->Get_EffectDesc();
        
    }

    static char szEffectName[MAX_PATH] = "";
    static char szEffectFolderPath[MAX_PATH] = "";
    static char szEffectFileName[MAX_PATH] = "";
    
    ImGui::Text("Effect_Name");
    ImGui::InputText("##EffectName", szEffectName, MAX_PATH);
    IMGUI_NEW_LINE;
    ImGui::Text("Effect_FolderPath");
    ImGui::InputText("##EffectFolderPath", szEffectFolderPath, MAX_PATH);
    IMGUI_NEW_LINE;
    ImGui::Text("Effect_FileName");
    ImGui::InputText("##EffectFileName", szEffectFileName, MAX_PATH);

    if(ImGui::Button("Generate_Effect"))
    {
        Safe_Release(m_pPrevEffect);

        CEffect::EFFECT_DESC tDesc;
        ZeroMemory(&tDesc, sizeof(CEffect::EFFECT_DESC));

        CMesh_Effect* pMeshEffect = CMesh_Effect::Create(m_pDevice, m_pContext, 
            CUtils::ToWString(szEffectName), 
            L"Mesh_Effect", 
            CUtils::ToWString(szEffectFolderPath), 
            CUtils::ToWString(szEffectFileName), 
            tDesc);

        pMeshEffect->Initialize(nullptr);

        m_pPrevEffect = pMeshEffect;
    }

    if (nullptr != m_pPrevEffect)
    {
        CEffect::EFFECT_DESC tEffectDesc = m_pPrevEffect->Get_EffectDesc();
        
        ImGui::Text("SpeedU : ");
        IMGUI_SAME_LINE;
        ImGui::DragFloat("##EffectUSpeed", &tEffectDesc.fUSpeed, 0.01f, 0.f, 100.f);


        ImGui::Text("SpeedV : ");
        IMGUI_SAME_LINE;
        ImGui::DragFloat("##EffectVSpeed", &tEffectDesc.fVSpeed, 0.01f, 0.f, 100.f);

        //ImGui::Text("Texture Select : ");
        //IMGUI_SAME_LINE;
        //ImGui::DragInt("##EffectTextureIndex", (int*)&tEffectDesc.iTextureIndex, 1.f, 0, m_pPrevEffect->Get_Texture()->Get_TextureCount() - 1);

        m_pPrevEffect->Set_EffectDesc(tEffectDesc);

    }
    ImGui::End();
}

void CImGui_Manager::Tick_Map_Tool(_float fTimeDelta)
{
    ImGui::Begin("Map");

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
                        bSelected = true;
                    }
                }
                ImGui::EndListBox();
            }
        }
    }

    if (bSelected)
    {
        if (ImGui::Button("Create"))
        {
            wstring strPrototypeName = CUtils::ToWString(pSelectedObjectName);
            if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TOOL, eSelectedLayer, strPrototypeName, nullptr)))
            {
                MSG_BOX("Create Failed.");
            }
        }
    }


    if (m_pPrevObject && nullptr != m_pTerrain)
    {
        _float4 vHitPos;
        if (m_pTerrain->Is_Picking(&vHitPos))
        {
            CTransform* pTransform = dynamic_cast<CTransform*>(m_pPrevObject->Get_Component(L"Com_Transform"));
            if (nullptr != pTransform)
                pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vHitPos));

            if (KEY_TAP(KEY::LBTN))
            {
                CGameObject* pCloneObject = m_pPrevObject->Clone(nullptr);
                if (nullptr == pCloneObject)
                {
                    MSG_BOX("Clone_Failed.");
                    Safe_Release(m_pPrevObject);
                    m_pPrevObject = nullptr;
                }
                CTransform* pObjectTransform = dynamic_cast<CTransform*>(pCloneObject->Get_Component(L"Com_Transform"));
                if (pObjectTransform == nullptr)
                {
                    MSG_BOX("Get_TransformCom Failed.");
                    Safe_Release(m_pPrevObject);
                    m_pPrevObject = nullptr;
                }

                pObjectTransform->Set_State(CTransform::STATE::STATE_RIGHT, pTransform->Get_State(CTransform::STATE::STATE_RIGHT));
                pObjectTransform->Set_State(CTransform::STATE::STATE_UP, pTransform->Get_State(CTransform::STATE::STATE_UP));
                pObjectTransform->Set_State(CTransform::STATE::STATE_LOOK, pTransform->Get_State(CTransform::STATE::STATE_LOOK));
                pObjectTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vHitPos));

                if (FAILED(GAME_INSTANCE->Add_GameObject(LEVEL_TOOL, eSelectedLayer, pCloneObject)))
                {
                    MSG_BOX("Add_GameObject Failed.");
                    Safe_Release(m_pPrevObject);
                    Safe_Release(pCloneObject);
                    m_pPrevObject = nullptr;
                }
            }
        }

        m_pPrevObject->Tick(fTimeDelta);
        m_pPrevObject->LateTick(fTimeDelta);

        if (KEY_TAP(KEY::RBTN))
        {
            Safe_Release(m_pPrevObject);
            m_pPrevObject = nullptr;
        }

        if (KEY_TAP(KEY::R))
        {
            CTransform* pTransform = dynamic_cast<CTransform*>(m_pPrevObject->Get_Component(L"Com_Transform"));
            pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
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
