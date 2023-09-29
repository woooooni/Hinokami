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

    const list<CGameObject*>& GameObjects = GAME_INSTANCE->Find_GameObjects(LEVEL_TOOL, _uint(LAYER_TYPE::LAYER_BUILDING));


    if (ImGui::CollapsingHeader("Building"))
    {
        if (ImGui::BeginListBox("##BuildingObject"))
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

                if (ImGui::Selectable(ObjectTag.c_str(), TargetObjectTag.c_str()))
                {

                }
                iIdx++;
            }

            ImGui::EndListBox();
        }
    }
    
    ImGui::End();
}

void CImGui_Manager::Tick_Model_Tool(_float fTimeDelta)
{
    
    ImGui::Begin("Model Editor");
    if (nullptr != m_pDummy)
    {
        ImGui::Text("Name : ");
        IMGUI_SAME_LINE;
        ImGui::Text(CUtils::ToString(m_pDummy->Get_ObjectTag()).c_str());


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

        if(vScale.x >= 0.1f 
            && vScale.y >= 0.1f 
            && vScale.z >= 0.1f)
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
                    m_iModelType = n;
                }
                    
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Import"))
        {
            m_pDummy->Ready_ModelCom(m_iModelType, m_strFilePath, m_strFileName);
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
    ImGui::Text("Effect Tool");
    ImGui::End();
}

void CImGui_Manager::Tick_Map_Tool(_float fTimeDelta)
{
    ImGui::Begin("Map");
    const map<const wstring, CGameObject*>& PrototypeObjects = GAME_INSTANCE->Find_Prototype_GameObjects(LAYER_BUILDING);

    static char pSelectedObjectName[MAX_PATH] = "Temp";
    static _bool bSelected = false;
    if (ImGui::BeginListBox("##Object_List"))
    {
        for (auto& iter : PrototypeObjects)
        {
            string strPrototypeName = CUtils::ToString(iter.first);
            if (ImGui::Selectable(strPrototypeName.c_str(), !strcmp(pSelectedObjectName, strPrototypeName.c_str())))
            {
                strcpy_s(pSelectedObjectName, strPrototypeName.c_str());
                bSelected = true;
            }
        }
        ImGui::EndListBox();
    }
    if (bSelected)
    {
        if (ImGui::Button("Create"))
        {
            wstring strPrototypeName = CUtils::ToWString(pSelectedObjectName);
            GAME_INSTANCE->Add_GameObject(LEVEL_TOOL, LAYER_BUILDING, strPrototypeName, nullptr);
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
