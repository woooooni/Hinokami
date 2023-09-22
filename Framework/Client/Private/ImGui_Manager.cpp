#include "stdafx.h"
#include "ImGui_Manager.h"
#include "GameObject.h"
#include "Component_Manager.h"
#include "Animation.h"
#include "GameInstance.h"
#include "Dummy.h"
#include "Terrain.h"

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

    ImGui::Checkbox("Model", &m_bShowModelWindow);
    ImGui::Checkbox("Animation", &m_bShowAnimationWindow);
    ImGui::Checkbox("Effect", &m_bShowEffectWindow);
    ImGui::Checkbox("Map", &m_bShowMapWindow);
    ImGui::Checkbox("Terrain", &m_bShowTerrainWindow);

    if (m_bShowDemo)
        ImGui::ShowDemoWindow(&m_bShowDemo);

    if (m_bShowModelWindow)
        Tick_Model_Tool(fTimeDelta);
    if(m_bShowAnimationWindow)
        Tick_Animation_Tool(fTimeDelta);
    if(m_bShowEffectWindow)
        Tick_Effect_Tool(fTimeDelta);
    if(m_bShowMapWindow)
        Tick_Map_Tool(fTimeDelta);
    if(m_bShowTerrainWindow)
        Tick_Terrain_Tool(fTimeDelta);
    
    ImGui::EndTabBar();
    ImGui::End();
    
}

void CImGui_Manager::Tick_Model_Tool(_float fTimeDelta)
{
    
    ImGui::Begin("Model");
    if (nullptr != m_pDummy)
    {
        ImGui::Text("Name : ");
        IMGUI_SAME_LINE;
        ImGui::Text(CGameInstance::GetInstance()->wstring_to_string(m_pDummy->Get_ObjectTag()).c_str());


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
        ImGui::DragFloat3("##Position", (_float*)&vPos, 0.1f, -999.f, 999.f, "%.3f");

        pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
    #pragma endregion

        IMGUI_NEW_LINE;

    #pragma region Rotaion_TODO
        //// Rotation
        //_vector vRight = pTransform->Get_State(CTransform::STATE::STATE_RIGHT);
        //_vector vUp = pTransform->Get_State(CTransform::STATE::STATE_UP);
        //_vector vLook = pTransform->Get_State(CTransform::STATE::STATE_LOOK);

        //
        //float fRotaionX = XMConvertToDegrees(XMVectorGetX(XMVector4Dot(XMVector4Normalize(vRight), XMVectorSet(1.f, 0.f, 0.f, 0.f))));
        //float fRotaionY = XMConvertToDegrees(XMVectorGetX(XMVector4Dot(XMVector4Normalize(vUp), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
        //float fRotaionZ = XMConvertToDegrees(XMVectorGetX(XMVector4Dot(XMVector4Normalize(vLook), XMVectorSet(0.f, 0.f, 1.f, 0.f))));

        //ImGui::Text("Rotaion");

        //ImGui::Text("X : ");
        //IMGUI_SAME_LINE;
        //ImGui::DragFloat("##Rotation_x", &fRotaionX, 0.1f, 0.f, 360.f);

        //ImGui::Text("Y : ");
        //IMGUI_SAME_LINE;
        //ImGui::DragFloat("##Rotation_y", &fRotaionY, 0.1f, 0.f, 360.f);

        //ImGui::Text("Z : ");
        //IMGUI_SAME_LINE;
        //ImGui::DragFloat("##Rotation_z", &fRotaionZ, 0.1f, 0.f, 360.f);

        ///*pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fRotaionX));
        //pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRotaionY));
        //pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRotaionZ));*/
    #pragma endregion

        IMGUI_NEW_LINE;

    #pragma region Scale
        // Scale
        _float3 vScale = pTransform->Get_Scale();

        ImGui::Text("Scale");
        ImGui::DragFloat3("##Scale", (_float*)&vScale, 0.1f, 0.1f, 100.f);

        if(vScale.x >= 0.1f 
            && vScale.y >= 0.1f 
            && vScale.z >= 0.1f)
            pTransform->Set_Scale(XMLoadFloat3(&vScale));
    #pragma endregion
        

        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;

        char szFilePath[MAX_PATH];
        char szFileName[MAX_PATH];
        

        sprintf_s(szFilePath, CGameInstance::GetInstance()->wstring_to_string(m_strFilePath).c_str());
        sprintf_s(szFileName, CGameInstance::GetInstance()->wstring_to_string(m_strFileName).c_str());
        


        ImGui::Text("Path");
        IMGUI_SAME_LINE;
        if (ImGui::InputText("##ModelPathText", szFilePath, MAX_PATH))
        {
            m_strFilePath = wstring(CGameInstance::GetInstance()->string_to_wstring(string(szFilePath)));
        }
        
        ImGui::Text("File");
        IMGUI_SAME_LINE;
        if (ImGui::InputText("##ModelFileText", szFileName, MAX_PATH))
        {
            m_strFileName = wstring(CGameInstance::GetInstance()->string_to_wstring(string(szFileName)));
        }

        
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
            m_pDummy->Export_Model();
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


        


        // AnimationList
        if (ImGui::BeginListBox("##Animation_List"))
        {
            for(size_t i = 0; i< Animations.size(); ++i)
            {
                string AnimationName = CGameInstance::GetInstance()->wstring_to_string(Animations[i]->Get_AnimationName());
                if (ImGui::Selectable(AnimationName.c_str(), i == pModelCom->Get_CurrAnimationIndex()))
                {
                    pModelCom->Set_AnimIndex(i);
                    sprintf_s(szAnimationName, CGameInstance::GetInstance()->wstring_to_string(Animations[pModelCom->Get_CurrAnimationIndex()]->Get_AnimationName()).c_str());
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
            wstring NewAnimationName = CGameInstance::GetInstance()->string_to_wstring(string(szAnimationName));
            if (NewAnimationName.size() > 0)
                Animations[pModelCom->Get_CurrAnimationIndex()]->Set_AnimationName(NewAnimationName);
        }

        if (KEY_TAP(KEY::ENTER) && ImGui::IsWindowFocused())
        {
            wstring NewAnimationName = CGameInstance::GetInstance()->string_to_wstring(string(szAnimationName));
            if (NewAnimationName.size() > 0)
                Animations[pModelCom->Get_CurrAnimationIndex()]->Set_AnimationName(NewAnimationName);
        }

        if (ImGui::Button("Delete"))
        {
            pModelCom->Delete_ModelAnimation(pModelCom->Get_CurrAnimationIndex());
        }

        if (KEY_TAP(KEY::DEL) && ImGui::IsWindowFocused())
        {
            pModelCom->Delete_ModelAnimation(pModelCom->Get_CurrAnimationIndex());
        }
        ImGui::EndGroup();

        // Animation Slider
        CAnimation* pCurrAnimation = Animations[pModelCom->Get_CurrAnimationIndex()];

        _float fDuration = pCurrAnimation->Get_Duration();
        _float fCurrFrame = pCurrAnimation->Get_KeyDesc().iCurrFrame;
        if (ImGui::SliderFloat("##Animation_Slider", &fCurrFrame, 0.f, fDuration, "%.2f"))
        {
            pCurrAnimation->Set_Pause(true);
            pCurrAnimation->Set_CurrFrame(fCurrFrame);
        }
        else
        {
            pCurrAnimation->Set_Pause(false);
        }

        if (ImGui::ArrowButton("##Play_AnimationButton", ImGuiDir_Right))
        {
            pCurrAnimation->Set_Pause(false);
        }

        IMGUI_SAME_LINE;

        if (ImGui::Button("||"))
        {
            pCurrAnimation->Set_Pause(true);
        }


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
    ImGui::Text("Map Tool");
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
        ImGui::DragFloat3("##Scale", (_float*)&vScale, 0.1f, 0.1f, 100.f);

        if (vScale.x >= 0.1f
            && vScale.y >= 0.1f
            && vScale.z >= 0.1f)
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
