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
#include "Particle.h"


#include <filesystem>
#include <fstream>
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
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Batang.ttc", 12.f, NULL, io.Fonts->GetGlyphRangesKorean());

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pEffect = new BasicEffect(m_pDevice);

    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCodes = nullptr;
    size_t			iLength = 0;

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
        return E_FAIL;

    return S_OK;
}

void CImGui_Manager::Tick(_float fTimeDelta)
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("CameraPosition");
    _float4 vCamPosition = GI->Get_CamPosition();
    ImGui::Text("x : ");
    IMGUI_SAME_LINE;
    ImGui::Text(to_string(vCamPosition.x).c_str());

    ImGui::Text("y : ");
    IMGUI_SAME_LINE;
    ImGui::Text(to_string(vCamPosition.y).c_str());

    ImGui::Text("z : ");
    IMGUI_SAME_LINE;
    ImGui::Text(to_string(vCamPosition.z).c_str());


    ImGui::End();
    Tick_Basic_Tool(fTimeDelta);
    Tick_Hierachy(fTimeDelta);
    Tick_Inspector(fTimeDelta);
    

    
   
    ImGui::EndFrame();
}


void CImGui_Manager::Render_ImGui()
{
    if (m_bShowNavigationWindow)    
        Draw_NaviPicking_Point();
    

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

    if (KEY_TAP(KEY::P))
    {
        list<CGameObject*>& Trees = GI->Find_GameObjects(LEVEL_TOOL, LAYER_TREE);
        auto iter = Trees.begin();
        while (iter != Trees.end())
        {
            CTransform* pTransform = (*iter)->Get_Component<CTransform>(L"Com_Transform");
            if (pTransform == nullptr)
                continue;

            _vector vPosition = pTransform->Get_Position();
            if (XMVectorGetX(vPosition) < -50.f || XMVectorGetX(vPosition) > 50.f)
            {
                iter = Trees.erase(iter);
                continue;
            }
            else
            {
                ++iter;
            }
                

        }
    }
    ImGui::BeginTabBar("##NoNameTabBar");
    ImGui::Checkbox("Demo_Window", &m_bShowDemo);

    IMGUI_NEW_LINE;

    ImGui::Checkbox("Model_Editor", &m_bShowModelWindow);
    ImGui::Checkbox("Animation", &m_bShowModelWindow);
    ImGui::Checkbox("Effect", &m_bShowEffectWindow);
    ImGui::Checkbox("Particle", &m_bShowParticleWindow);
    ImGui::Checkbox("Map", &m_bShowMapWindow);
    ImGui::Checkbox("Terrain", &m_bShowTerrainWindow);
    ImGui::Checkbox("Navigation", &m_bShowNavigationWindow);

    if (m_bShowDemo)
        ImGui::ShowDemoWindow(&m_bShowDemo);

    if (m_bShowNavigationWindow)
    {
        m_bShowDemo = false;
        m_bShowModelWindow = false;
        m_bShowEffectWindow = false;
        m_bShowParticleWindow = false;
        m_bShowMapWindow = false;
        m_bShowTerrainWindow = false;
        m_bShowNavigationWindow = true;
    }

    if (m_bShowNavigationWindow)
        Tick_NaviMesh_Tool(fTimeDelta);

    if (m_bShowModelWindow)
    {
        Tick_Model_Tool(fTimeDelta);
        Tick_Animation_Tool(fTimeDelta);
    }
        
    if(m_bShowEffectWindow)
        Tick_Effect_Tool(fTimeDelta);
    if (m_bShowParticleWindow)
        Tick_Particle_Tool(fTimeDelta);
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

            if (ImGui::BeginListBox(szListBoxLable, ImVec2(500, 0)))
            {
                for (auto& Object : GameObjects)
                {
                    string ObjectTag = CUtils::ToString(Object->Get_ObjectTag());
                    string TargetObjectTag;

                    if (m_pTarget)
                        TargetObjectTag = CUtils::ToString(m_pTarget->Get_ObjectTag());
                    else
                        TargetObjectTag = "";

                    char szTempName[MAX_PATH] = "##";
                    strcat_s(szTempName, ObjectTag.c_str());
                    if (Object->Is_NaviObject())
                    {
                        
                        
                        if (ImGui::ColorButton(szTempName, ImVec4(0.f, 1.f, 0.f, 1.f)))
                            Object->Set_NaviObject(false);
                    }
                    else
                    {
                        strcat_s(szTempName, ObjectTag.c_str());
                        if (ImGui::ColorButton(szTempName, ImVec4(1.f, 0.f, 0.f, 1.f)))
                            Object->Set_NaviObject(true);
                    }
                    IMGUI_SAME_LINE;

                    if (ImGui::Selectable(ObjectTag.c_str(), ObjectTag == TargetObjectTag, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(300, 15)))
                    {
                        m_pTarget = Object;
                        if (ImGui::IsMouseDoubleClicked(0))
                        {
                            CTransform* pCameraTransform = m_pCamera->Get_Component<CTransform>(L"Com_Transform");
                            CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");
                            
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

    /*if (KEY_TAP(KEY::F2))
    {
        list<CGameObject*>& GameObjects = GI->Find_GameObjects(LEVEL_TOOL, LAYER_TYPE::LAYER_BUILDING);
        for (auto& pGameObject : GameObjects)
        {
            if (pGameObject->Get_PrototypeTag() != L"Wall_43")
                continue;

            CTransform* pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform");
            if (nullptr == pTransform)
                continue;

            CCollider_AABB::AABB_COLLIDER_DESC tBoxDesc;
            ZeroMemory(&tBoxDesc, sizeof tBoxDesc);

            BoundingBox tBox;
            ZeroMemory(&tBox, sizeof tBox);
            tBox.Extents = { 4.f, 2.f, 0.52f };
            XMStoreFloat3(&tBox.Center, pTransform->Get_State(CTransform::STATE_POSITION));

            CModel* pModel = pGameObject->Get_Component<CModel>(L"Com_Model");
            if (pModel == nullptr)
                XMStoreFloat4x4(&tBoxDesc.ModePivotMatrix, XMMatrixIdentity());
            else
                XMStoreFloat4x4(&tBoxDesc.ModePivotMatrix, pModel->Get_PivotMatrix());

            tBoxDesc.tBox = tBox;
            tBoxDesc.pOwnerTransform = pTransform;
            tBoxDesc.vOffsetPosition = { 405.f, 204.f, 50.f };
            tBoxDesc.pNode = nullptr;

            pGameObject->Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::AABB, CCollider::DETECTION_TYPE::BOUNDARY, &tBoxDesc);
            pGameObject->Add_Collider(LEVEL_STATIC, CCollider::COLLIDER_TYPE::AABB, CCollider::DETECTION_TYPE::BODY, &tBoxDesc);
        }
    }

    if (KEY_TAP(KEY::F3))
    {
        list<CGameObject*>& GameObjects = GI->Find_GameObjects(LEVEL_TOOL, LAYER_TYPE::LAYER_BUILDING);
        for (auto& pGameObject : GameObjects)
        {
            if (pGameObject->Get_PrototypeTag() != L"Wall_43")
                continue;

            for (_uint i = 0; i < CCollider::DETECTION_TYPE::DETECTION_END; ++i)
            {
                vector<CCollider*>& Colliders = pGameObject->Get_Collider(i);

                auto iter = Colliders.begin();
                while (iter != Colliders.end())
                    iter = Colliders.erase(iter);
            }
            
            
        }
    }*/

    ImGui::Begin("Inspector");
    if (KEY_HOLD(KEY::CTRL) && KEY_TAP(KEY::LBTN))
    {
        for (_uint i = 0; i < LAYER_TYPE::LAYER_END; ++i)
        {
            if (i == LAYER_TYPE::LAYER_CAMERA
                || i == LAYER_TYPE::LAYER_TERRAIN
                || i == LAYER_TYPE::LAYER_BACKGROUND
                || i == LAYER_TYPE::LAYER_SKYBOX
                || i == LAYER_TYPE::LAYER_UI
                || i == LAYER_TYPE::LAYER_GROUND
                || i == LAYER_TYPE::LAYER_EFFECT
                || i == LAYER_TYPE::LAYER_GRASS)
                continue;

            list<CGameObject*>& GameObjects = GAME_INSTANCE->Find_GameObjects(LEVEL_TOOL, i);
            for (auto& Object : GameObjects)
            {
                CTransform* pTransform = Object->Get_Component<CTransform>(L"Com_Transform");
                CModel* pModel = Object->Get_Component<CModel>(L"Com_Model");
                if (pTransform == nullptr)
                    continue;

                if (pModel == nullptr)
                    continue;

                for (auto& pMesh : pModel->Get_Meshes())
                {
                    _float4 vPosition;
                    if (CPicking_Manager::GetInstance()->Is_Picking(pTransform, pMesh, false, &vPosition))
                    {
                        m_pTarget = Object;
                        break;
                    }
                }
            }

        }
    }
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

        CTransform* pTransform = m_pTarget->Get_Component<CTransform>(L"Com_Transform");
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
        _float3 vPrevRotation = pTransform->Get_Rotaion_Degree();
        _float3 vRotation = pTransform->Get_Rotaion_Degree();

        if (ImGui::DragFloat("##Object_RotationX", &vRotation.x, 0.01f, -360.f, 360.f))
        {
            _float fRadian = XMConvertToRadians(vRotation.x - vPrevRotation.x);
            pTransform->Rotation_Acc(XMVectorSet(1.f, 0.f, 0.f, 0.f), fRadian);
        }
        if (ImGui::DragFloat("##Object_RotationY", &vRotation.y, 0.01f, -360.f, 360.f))
        {
            _float fRadian = XMConvertToRadians(vRotation.y - vPrevRotation.y);
            pTransform->Rotation_Acc(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
        }
        if (ImGui::DragFloat("##Object_RotationZ", &vRotation.z, 0.01f, -360.f, 360.f))
        {
            _float fRadian = XMConvertToRadians(vRotation.z - vPrevRotation.z);
            pTransform->Rotation_Acc(XMVectorSet(0.f, 0.f, 1.f, 0.f), fRadian);
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

        IMGUI_NEW_LINE;

        ImGui::Text("Colliders");


        const char* szColliderTypes[] = { "Sphere", "Box" };
        static const char* szColliderType = NULL;
        static _int iSelectedColliderType = -1;

        if (ImGui::BeginCombo("##Collider_Type", szColliderType))
        {
            for (int n = 0; n < IM_ARRAYSIZE(szColliderTypes); n++)
            {
                bool is_selected = (szColliderType == szColliderTypes[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(szColliderTypes[n], is_selected))
                {
                    szColliderType = szColliderTypes[n];
                    iSelectedColliderType = n;
                }

            }

            ImGui::EndCombo();
        }



        const char* szDetectionTypes[] = { "BOUNDARY", "HEAD", "BODY", "WEAPON", "EFFECT"};
        static const char* szDetectionType = NULL;
        static _int iSelectedDetectionType = -1;

        if (ImGui::BeginCombo("##Detection_Type", szDetectionType))
        {
            for (int n = 0; n < IM_ARRAYSIZE(szDetectionTypes); n++)
            {
                bool is_selected = (szDetectionType == szDetectionTypes[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(szDetectionTypes[n], is_selected))
                {
                    szDetectionType = szDetectionTypes[n];
                    iSelectedDetectionType = n;
                }

            }

            ImGui::EndCombo();
        }

        
        if (ImGui::Button("Add_Collider"))
        {
            if(iSelectedColliderType == CCollider::COLLIDER_TYPE::AABB)
            {
                if (-1 != iSelectedDetectionType)
                {
                    CCollider_AABB::AABB_COLLIDER_DESC tBoxDesc;
                    ZeroMemory(&tBoxDesc, sizeof tBoxDesc);

                    BoundingBox tBox;
                    ZeroMemory(&tBox, sizeof tBox);
                    tBox.Extents = { 1.f, 1.f, 1.f };
                    XMStoreFloat3(&tBox.Center, pTransform->Get_State(CTransform::STATE_POSITION));

                    CModel* pModel = m_pTarget->Get_Component<CModel>(L"Com_Model");
                    if (pModel == nullptr)
                        XMStoreFloat4x4(&tBoxDesc.ModePivotMatrix, XMMatrixIdentity());
                    else
                        XMStoreFloat4x4(&tBoxDesc.ModePivotMatrix, pModel->Get_PivotMatrix());

                    tBoxDesc.tBox = tBox;
                    tBoxDesc.pOwnerTransform = pTransform;
                    tBoxDesc.vOffsetPosition = {};
                    tBoxDesc.pNode = nullptr;

                    m_pTarget->Add_Collider(LEVEL_STATIC, iSelectedColliderType, iSelectedDetectionType, &tBoxDesc);
                }
                
            }

            else if (iSelectedColliderType == CCollider::COLLIDER_TYPE::SPHERE)
            {
                if (-1 != iSelectedDetectionType)
                {
                    CCollider_Sphere::SPHERE_COLLIDER_DESC tSphereDesc;
                    ZeroMemory(&tSphereDesc, sizeof tSphereDesc);

                    BoundingSphere tSphere;
                    ZeroMemory(&tSphere, sizeof tSphere);
                    tSphere.Radius = 0.f;
                    XMStoreFloat3(&tSphere.Center, pTransform->Get_State(CTransform::STATE_POSITION));
                    
                    CModel* pModel = m_pTarget->Get_Component<CModel>(L"Com_Model");
                    if (pModel == nullptr)
                        XMStoreFloat4x4(&tSphereDesc.ModePivotMatrix, XMMatrixIdentity());
                    else
                        XMStoreFloat4x4(&tSphereDesc.ModePivotMatrix, pModel->Get_PivotMatrix());

                    tSphereDesc.tSphere = tSphere;
                    tSphereDesc.pOwnerTransform = pTransform;
                    tSphereDesc.vOffsetPosition = {};
                    tSphereDesc.pNode = nullptr;

                    m_pTarget->Add_Collider(LEVEL_STATIC, iSelectedColliderType, iSelectedDetectionType, &tSphereDesc);
                }
            }
            
        }
        
        _uint iColliderId = 0;
        for (_uint i = 0; i < CCollider::DETECTION_TYPE::DETECTION_END; ++i)
        {
            vector<CCollider*>& Colliders = m_pTarget->Get_Collider(i);
            auto iter = Colliders.begin();

            
            char szColliderDetectionType[MAX_PATH] = "===========";
            if (i == CCollider::DETECTION_TYPE::BOUNDARY)
                strcat_s(szColliderDetectionType, "BOUNDARY===========");
            else if(i == CCollider::DETECTION_TYPE::BODY)
                strcat_s(szColliderDetectionType, "BODY===========");
            else if(i == CCollider::DETECTION_TYPE::HEAD)
                strcat_s(szColliderDetectionType, "HEAD===========");
            else if(i == CCollider::DETECTION_TYPE::ATTACK)
                strcat_s(szColliderDetectionType, "ATTACK===========");

            ImGui::Text(szColliderDetectionType);
            while (iter != Colliders.end())
            {
                IMGUI_NEW_LINE;
                
                char szTagOffset[MAX_PATH] = "##";
                strcat_s(szTagOffset, to_string(++iColliderId).c_str());
                strcat_s(szTagOffset, "OffsetPosition");

                _float3 vOffset = (*iter)->Get_Offset();
                ImGui::Text("Collider_Offset");
                ImGui::DragFloat3(szTagOffset, (_float*)&vOffset, 1.f, -10000.f, 10000.f);
                (*iter)->Set_Offset(vOffset);

                if (ImGui::Button("Delete"))
                {
                    iter = Colliders.erase(iter);
                    continue;
                }

                if ((*iter)->Get_ColliderType() == CCollider::COLLIDER_TYPE::AABB)
                {
                    CCollider_AABB* pBoxCollider = dynamic_cast<CCollider_AABB*>((*iter));
                    if (nullptr != pBoxCollider)
                    {
                        BoundingBox tBox = pBoxCollider->Get_AABB_Box();
                        ImGui::Text("Extents : ");
                        IMGUI_SAME_LINE;
                        char szTagExtexts[MAX_PATH] = "";
                        strcpy_s(szTagExtexts, "##");
                        strcat_s(szTagExtexts, to_string(iColliderId).c_str());
                        strcat_s(szTagExtexts, "X");
                        ImGui::DragFloat(szTagExtexts, &tBox.Extents.x, 0.01f, 0.1f, 100.f);
                        strcat_s(szTagExtexts, "Y");
                        ImGui::DragFloat(szTagExtexts, &tBox.Extents.y, 0.01f, 0.1f, 100.f);
                        strcat_s(szTagExtexts, "Z");
                        ImGui::DragFloat(szTagExtexts, &tBox.Extents.z, 0.01f, 0.1f, 100.f);
                        pBoxCollider->Set_AABB_Box(tBox);
                    }

                }
                else if ((*iter)->Get_ColliderType() == CCollider::COLLIDER_TYPE::SPHERE)
                {
                    CCollider_Sphere* pShpereCollider = dynamic_cast<CCollider_Sphere*>((*iter));
                    if (nullptr != pShpereCollider)
                    {
                        BoundingSphere tSphere = pShpereCollider->Get_Sphere();
                        ImGui::Text("Radius : ");
                        IMGUI_SAME_LINE;
                        ImGui::DragFloat("##Collider_Radius", &tSphere.Radius, 0.01f, 0.1f, 1000.f);
                        pShpereCollider->Set_Sphere(tSphere);
                    }
                }

                ++iter;
            }
        }
        


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
        
        if(ImGui::Button("Export_Animation_Names"))
        {
            //쓰기 전용으로 파일을 오픈(파일이 없으면 생성)
            ofstream fout;
            fout.open("../Bin/Export/ModelAnimations.txt");

            for (auto& iter : Animations)
            {
                wstring strAnimationName = iter->Get_AnimationName();
                if (fout.is_open())
                {
                    fout.write(CUtils::ToString(strAnimationName).c_str(), strAnimationName.size());
                    fout.write("\n", sizeof(1));
                }
                    
            }
            fout.close();
            MSG_BOX("Export OK.");
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

        _float fAnimationProgress = pCurrAnimation->Get_AnimationProgress();

        ImGui::Text("Progress : ");
        IMGUI_SAME_LINE;
        
        ImGui::Text(to_string(fAnimationProgress).c_str());

        if (ImGui::ArrowButton("##Play_AnimationButton", ImGuiDir_Right))
        {
            if(fAnimationProgress >= 1.f)
                pModelCom->Set_AnimIndex(pModelCom->Get_CurrAnimationIndex());

            pCurrAnimation->Set_Pause(false);
        }
            

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

    static char szEffectModelName[MAX_PATH] = "";
    if (ImGui::BeginListBox("##Effect_Model_List", ImVec2(400.f, 200.f)))
    {
        for (size_t i = 0; i < m_EffectsModelFiles.size(); ++i)
        {
            if (ImGui::Selectable(CUtils::ToString(m_EffectsModelFiles[i]).c_str(), CUtils::ToWString(szEffectModelName) == m_EffectsModelFiles[i]))
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
        {
            m_pPrevEffect = CEffect::Create(m_pDevice, m_pContext, L"Prev_Effect", CEffect::EFFECT_TYPE(iSelecetedEffectType), CUtils::ToWString(szEffectModelName), tEffectDesc, true, true, false);

            m_pPrevEffect->Set_PrototypeTag(L"Mesh_Temp");
        }
            
        else
        {
             m_pPrevEffect = CEffect::Create(m_pDevice, m_pContext, L"Prev_Effect", CEffect::EFFECT_TYPE(iSelecetedEffectType), L"", tEffectDesc, true, true, false);
            m_pPrevEffect->Set_PrototypeTag(L"Texture_Temp");
        }
            


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
        CTexture* pAlphaTexture = m_pPrevEffect->Get_AlphaTexture();

        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Texture");
        ImGui::Text("============================================");
        {
            ImGui::Text("Diffuse Texture");
            if (ImGui::BeginListBox("##Effect_DiffuseTexture_List", ImVec2(500.f, 300.f)))
            {
                for (size_t i = 0; i < pDiffuseTexture->Get_TextureCount(); ++i)
                {
                    if(i % 3 != 0)
                        IMGUI_SAME_LINE;

                    if (ImGui::ImageButton(pDiffuseTexture->Get_Srv(i), ImVec2(150.f, 150.f)))
                    {
                        tEffectDesc.strDiffuseTetextureName = pDiffuseTexture->Get_Name(i);
                    }
                }
                ImGui::EndListBox();
            }

            IMGUI_SAME_LINE;
            if (ImGui::Button("Reset_Diffuse"))
            {
                tEffectDesc.strDiffuseTetextureName = L"";
            }

        }

        IMGUI_NEW_LINE;
        {
            ImGui::Text("Alpha Texture");
            if (ImGui::BeginListBox("##Effect_AlphaTexture_List", ImVec2(500.f, 300.f)))
            {
                for (size_t i = 0; i < pAlphaTexture->Get_TextureCount(); ++i)
                {
                    if (i % 3 != 0)
                        IMGUI_SAME_LINE;
                    if (ImGui::ImageButton(pAlphaTexture->Get_Srv(i), ImVec2(150.f, 150.f)))
                    {
                        tEffectDesc.strAlphaTexturName = pAlphaTexture->Get_Name(i);

                    }
                }
                ImGui::EndListBox();
            }

            IMGUI_SAME_LINE;
            if (ImGui::Button("Reset_Alpha"))
            {
                tEffectDesc.strAlphaTexturName = L"";

            }
        }



        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Texture & Color");
        ImGui::Text("============================================");

        IMGUI_NEW_LINE;
        ImGui::Text("ARGB");
        ImGui::DragFloat3("##AddtiveDiffuseColor", (_float*)&tEffectDesc.vAdditiveDiffuseColor, 0.01f, 0.f, 1.f);
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
        ImGui::Text("UV_Flow : ");
        ImGui::DragFloat2("##Effect_UV_Flow", (_float*)&tEffectDesc.vUVFlow, 0.01f, -100.f, 100.f);


        IMGUI_NEW_LINE;
        ImGui::Text("Dest_AlphaSpeed : ");
        ImGui::DragFloat("##Effect_DestAlphaSpeed", &tEffectDesc.fDestAlphaSpeed, 0.01f, 0.f, 100.f);

        if (ImGui::Button("Reset"))
        {
            tEffectDesc.fAlpha = 1.f;
            m_pPrevEffect->Reset_UV();
            pTransform->Set_WorldMatrix(XMMatrixIdentity());
        }
            

        IMGUI_NEW_LINE;
        ImGui::Text("Decrease Alpha Speed : ");
        

        ImGui::Text("============================================");
        ImGui::Text("Options");
        ImGui::Text("============================================");

        _int bCutUv = tEffectDesc.bCutUV;
        _bool bCutUvChecked = (bCutUv != -1);
        ImGui::Text("Cut UV");
        IMGUI_SAME_LINE;
        ImGui::Checkbox("##Effect_CutUV", &bCutUvChecked);

        if (bCutUvChecked == true)
            tEffectDesc.bCutUV = 1;
        else
            tEffectDesc.bCutUV = -1;

        


        ImGui::Text("Bill_Board ");
        IMGUI_SAME_LINE;
        ImGui::Checkbox("##Effect_BillBoard", &tEffectDesc.bBillboard);



        ImGui::Text("Loop ");
        IMGUI_SAME_LINE;
        _bool bLoop = m_pPrevEffect->Is_Loop();
        if (ImGui::Checkbox("##Effect_Loop", &bLoop))
        {
            m_pPrevEffect->Set_Loop(bLoop);
        }

        ImGui::Text("Increment ");
        IMGUI_SAME_LINE;
        _bool bIncrement = m_pPrevEffect->Is_Increment();
        if (ImGui::Checkbox("##Effect_Increment", &bIncrement))
        {
            m_pPrevEffect->Set_Increment(bIncrement);
        }

        ImGui::Text("Gravity ");
        IMGUI_SAME_LINE;
        _bool bGravity = m_pPrevEffect->Is_Gravity();
        if (ImGui::Checkbox("##Effect_Gravity", &bGravity))
        {
            m_pPrevEffect->Set_Gravity(bGravity);
        }

        ImGui::Text("Blur_Power");
        IMGUI_SAME_LINE;
        ImGui::DragFloat("##EffectBlurPower", (_float*)&tEffectDesc.fBlurPower, 0.01f, 0.01f, 100.f);

        ImGui::Text("Bloom_Power");
        IMGUI_SAME_LINE;
        ImGui::DragFloat3("##EffectBloomPower", (_float*)&tEffectDesc.vBloomPower, 0.01f, 0.01f, 100.f);

        IMGUI_NEW_LINE;
        ImGui::Text("Scale Dir");
        IMGUI_SAME_LINE;
        ImGui::DragFloat3("##EffectScaleDir", (_float*)&tEffectDesc.vScaleDir, 0.01f, -100.f, 100.f);

        IMGUI_NEW_LINE;
        ImGui::Text("Scale Speed");
        IMGUI_SAME_LINE;
        ImGui::DragFloat("##EffectScaleSpeed", (_float*)&tEffectDesc.fScaleSpeed, 0.01f, 0.f, 100.f);



        IMGUI_NEW_LINE;
        IMGUI_NEW_LINE;
        ImGui::Text("============================================");
        ImGui::Text("Offset");
        ImGui::Text("============================================");
        _vector vOffsetScale, vOffsetQuaternion, vOffsetPosition;
        XMMatrixDecompose(&vOffsetScale, &vOffsetQuaternion, &vOffsetPosition, XMLoadFloat4x4(&tEffectDesc.OffsetMatrix));

        _vector vOffsetRotation = XMLoadFloat3(&CUtils::ToEulerAngles(vOffsetQuaternion));


        _float3 fOffsetScale, fOffsetRoation, fOffsetPosition;
        
        XMStoreFloat3(&fOffsetScale, vOffsetScale);
        XMStoreFloat3(&fOffsetRoation, vOffsetRotation);
        XMStoreFloat3(&fOffsetPosition, vOffsetPosition);

        ImGui::Text("Offset_Scale : ");
        IMGUI_SAME_LINE;
        ImGui::DragFloat3("##Effect_OffsetScale", (_float*)&fOffsetScale, 0.01f, 0.f, 10.f);
        

        ImGui::Text("Offset_Rotation : ");
        IMGUI_SAME_LINE;
        fOffsetRoation.x = XMConvertToDegrees(fOffsetRoation.x);
        fOffsetRoation.y = XMConvertToDegrees(fOffsetRoation.y);
        fOffsetRoation.z = XMConvertToDegrees(fOffsetRoation.z);

        if (ImGui::DragFloat3("##Effect_OffsetRotation", (_float*)&fOffsetRoation, 0.1f, -360.f, 360.f))
        {
            if (fOffsetRoation.y == 180.f)
            {
                fOffsetRoation.x = 180.f;
                fOffsetRoation.y = 0.f;
                fOffsetRoation.z = 180.f;
            }
                
            fOffsetRoation.x = XMConvertToRadians(fOffsetRoation.x);
            fOffsetRoation.y = XMConvertToRadians(fOffsetRoation.y);
            fOffsetRoation.z = XMConvertToRadians(fOffsetRoation.z);

            
            vOffsetQuaternion = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&fOffsetRoation));
        }


        
        

        ImGui::Text("Offset_Position : ");
        IMGUI_SAME_LINE;
        ImGui::DragFloat3("##Effect_OffsetPosition", (_float*)&fOffsetPosition, 0.01f, -1000.f, 1000.f);

        
        XMStoreFloat4x4(&tEffectDesc.OffsetMatrix, 
            XMMatrixAffineTransformation(XMLoadFloat3(&fOffsetScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), vOffsetQuaternion, XMLoadFloat3(&fOffsetPosition)));

        
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

        static char szExportEffectName[MAX_PATH] = "";

        ImGui::Text("Effect_Name");
        IMGUI_SAME_LINE;
        ImGui::InputText("##EffectExportName", szExportEffectName, MAX_PATH);
        if (ImGui::Button("Save_Effect"))
        {
            if (0 != strcmp(szExportEffectName, ""))
            {
                char szFullPath[MAX_PATH] = "../Bin/Export/Effect/";

                strcat_s(szFullPath, szExportEffectName);
                strcat_s(szFullPath, ".effect");

                Save_Effect(CUtils::ToWString(szFullPath));
            }
            else
            {
                MSG_BOX("이름을 입력해주세요.");
            }
        }

        if (ImGui::Button("Load_Effect"))
        {
            if (0 != strcmp(szExportEffectName, ""))
            {
                char szFullPath[MAX_PATH] = "../Bin/Export/Effect/";

                strcat_s(szFullPath, szExportEffectName);
                strcat_s(szFullPath, ".effect");

                Load_Effect(CUtils::ToWString(szFullPath));
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
            || i == LAYER_TYPE::LAYER_UI
            || i == LAYER_TYPE::LAYER_TRAIL)
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
            CTransform* pPrevObjTransform = m_pPrevObject->Get_Component<CTransform>(L"Com_Transform");
            CTransform* pObjectTransform = pCloneObject->Get_Component<CTransform>(L"Com_Transform");
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
            CTransform* pTransform = m_pPrevObject->Get_Component<CTransform>(L"Com_Transform");
            _float3 vScale = pTransform->Get_Scale();
            vScale.x = clamp(vScale.x -= 1.f, 1.f, 999.f);
            vScale.y = clamp(vScale.y -= 1.f, 1.f, 999.f);
            vScale.z = clamp(vScale.z -= 1.f, 1.f, 999.f);
            pTransform->Set_Scale(XMLoadFloat3(&vScale));
        }

        if (KEY_TAP(KEY::CLOSE_SQUARE_BRACKET))
        {
            CTransform* pTransform = m_pPrevObject->Get_Component<CTransform>(L"Com_Transform");
            _float3 vScale = pTransform->Get_Scale();
            vScale.x = clamp(vScale.x += 1.f, 1.f, 999.f);
            vScale.y = clamp(vScale.y += 1.f, 1.f, 999.f);
            vScale.z = clamp(vScale.z += 1.f, 1.f, 999.f);
            pTransform->Set_Scale(XMLoadFloat3(&vScale));
        }

        if (KEY_TAP(KEY::R))
        {
            CTransform* pTransform = m_pPrevObject->Get_Component<CTransform>(L"Com_Transform");
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

void CImGui_Manager::Tick_NaviMesh_Tool(_float fTimeDeleta)
{
    ImGui::Begin("Navigation_Tool");
    static _bool bPickingMode = false;
    ImGui::Checkbox("Make Navi Triangle", &bPickingMode);

    if (bPickingMode == true)
        NaviPicking();

    static _float fGenCutDegree = 0.f;
    ImGui::Text("CutDegree");
    IMGUI_SAME_LINE;
    ImGui::DragFloat("##GenNaviDegree", &m_fGenerateRadian, 0.01f, 0.f, 3.141592f);

    static _float fMinGenScale = 0.f;
    static _float fMaxGenScale = 1000.f;

    ImGui::Text("TriangleScale_Min");
    IMGUI_SAME_LINE;
    ImGui::DragFloat("##GenNaviScaleMin", &fMinGenScale, 0.01f, 1.f, 100.f);

    ImGui::Text("TriangleScale_Max");
    IMGUI_SAME_LINE;
    ImGui::DragFloat("##GenNaviScaleMax", &fMaxGenScale, 0.01f, 1.f, 100.f);


    
    static _float fMinGenY = 0.f;
    static _float fMaxGenY = 1000.f;
    ImGui::Text("TriangleY_Min");
    IMGUI_SAME_LINE;
    ImGui::DragFloat("##GenNaviYMin", &fMinGenY, 0.01f, -100.f, 100.f);

    ImGui::Text("TriangleY_Max");
    IMGUI_SAME_LINE;
    ImGui::DragFloat("##GenNaviYMax", &fMaxGenY, 0.01f, -100.f, 100.f);

    m_fMinTriangleY = fMinGenY;
    m_fMaxTriangleY = fMaxGenY;

    m_fGenerateMinScale = fMinGenScale;
    m_fGenerateMaxScale = fMaxGenScale;

    if (ImGui::Button("Generate_Navigation"))
    {
        if (FAILED(NaviAutoGenerate()))        
            MSG_BOX("Generate Failed.");
    }

    if (ImGui::Button("Bake"))
    {
        if(FAILED(m_pTerrain->Get_NavigationCom()->SetUp_Neighbors()))
            MSG_BOX("Bake Failed.");
        else
            MSG_BOX("Bake Success");
    }


    IMGUI_NEW_LINE;
    static char szNaviSaveName[MAX_PATH] = "";
    ImGui::InputText("##NaviSaveLoadPath", szNaviSaveName, MAX_PATH);


    if (ImGui::Button("Save_Navi"))
    {
        if (FAILED(NaviSave(CUtils::ToWString(szNaviSaveName))))
            MSG_BOX("Save Navi Failed.");
    }

    if (ImGui::Button("Load_Navi"))
    {
        if (FAILED(NaviLoad(CUtils::ToWString(szNaviSaveName))))
            MSG_BOX("Load Navi Failed.");
        
    }
    
    
    ImGui::Text("m_vWorldPickedNaviPos Count");
    ImGui::Text(to_string(m_vWorldPickedNaviPos.size()).c_str());




    ImGui::End();
}

void CImGui_Manager::NaviPicking()
{
    if (KEY_HOLD(KEY::ALT))
    {
        if (true == CPicking_Manager::GetInstance()->Is_JustNaviPicking(m_pTerrain->Get_NavigationCom(), &m_vNaviPickingWorldPos))
        {
            if (KEY_TAP(KEY::LBTN))
            {
                m_vWorldPickedNaviPos.push_back(m_vNaviPickingWorldPos);

                if (m_vWorldPickedNaviPos.size() == 3)
                {
                    m_pTerrain->Get_NavigationCom()->Create_Cell(m_vWorldPickedNaviPos.data());
                    m_vWorldPickedNaviPos.clear();
                }
            }

            if (KEY_TAP(KEY::RBTN))
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(g_hWnd, &pt);

                _vector vMousePos = XMVectorSet(
                    _float(pt.x / (g_iWinSizeX * .5f) - 1.f),
                    _float(pt.y / (g_iWinSizeY * -.5f) + 1.f),
                    1.f, 1.f);

                _matrix ViewMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
                _matrix ProjMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);

                vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

                XMVECTOR vRayDir, vRayPosition;

                vRayPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                vRayDir = vMousePos - vRayPosition;


                vMousePos = XMVector3TransformCoord(vRayPosition, ViewMatrixInv);
                vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInv);

                m_vWorldPickedNaviPos.clear();
                m_pTerrain->Get_NavigationCom()->Delete_Cell(vRayDir, vMousePos);
            }
        }
        return;
    }

    for (_uint i = 0; i < LAYER_TYPE::LAYER_END; ++i)
    {
        list<CGameObject*>& GameObjects = GI->Find_GameObjects(LEVEL_TOOL, i);

        for (auto& pGameObject : GameObjects)
        {
            if (!pGameObject->Is_NaviObject())
                continue;


            CTransform* pTransform;
            pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform") == nullptr ? nullptr : pGameObject->Get_Component<CTransform>(L"Com_Transform");
            if (nullptr == pTransform)
                continue;

            CModel* pModel = pGameObject->Get_Component<CModel>(L"Com_Model");
            if (nullptr == pModel)
                continue;

            const vector<CMesh*>& Meshes = pModel->Get_Meshes();
            for (auto& pMesh : Meshes)
            {
                _float3 vLocalPos;
                if (KEY_HOLD(KEY::SHIFT))
                {
                    if (CPicking_Manager::GetInstance()->Is_NaviPicking(pTransform, pMesh, &m_vNaviPickingWorldPos, &vLocalPos))
                    {
                        if (KEY_HOLD(KEY::RBTN))
                        {
                            POINT pt;
                            GetCursorPos(&pt);
                            ScreenToClient(g_hWnd, &pt);

                            _vector vMousePos = XMVectorSet(
                                _float(pt.x / (g_iWinSizeX * .5f) - 1.f),
                                _float(pt.y / (g_iWinSizeY * -.5f) + 1.f),
                                1.f, 1.f);

                            _matrix ViewMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
                            _matrix ProjMatrixInv = GAME_INSTANCE->Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);

                            vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

                            XMVECTOR vRayDir, vRayPosition;

                            vRayPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                            vRayDir = vMousePos - vRayPosition;


                            vMousePos = XMVector3TransformCoord(vRayPosition, ViewMatrixInv);
                            vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInv);

                            m_vWorldPickedNaviPos.clear();
                            m_pTerrain->Get_NavigationCom()->Delete_Cell(vRayDir, vMousePos);
                        }

                        if (KEY_TAP(KEY::LBTN))
                        {
                            m_vWorldPickedNaviPos.push_back(m_vNaviPickingWorldPos);

                            if (m_vWorldPickedNaviPos.size() == 3)
                            {
                                m_pTerrain->Get_NavigationCom()->Create_Cell(m_vWorldPickedNaviPos.data());
                                m_vWorldPickedNaviPos.clear();
                            }
                        }
                        return;
                    }
                }
            }
        }
    }

}

HRESULT CImGui_Manager::Load_Map_Data(const wstring& strMapFileName)
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

            CTransform* pTransform = pObj->Get_Component<CTransform>(L"Com_Transform");
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
    m_pTarget = nullptr;
    m_pPrevObject = nullptr;
    MSG_BOX("Map_Loaded.");
    return S_OK;
}

HRESULT CImGui_Manager::Save_Map_Data(const wstring& strMapFileName)
{
    wstring strMapFilePath = L"../Bin/DataFiles/Map/" + strMapFileName + L"/" + strMapFileName + L".map";

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
            || i == LAYER_TYPE::LAYER_EFFECT
            || i == LAYER_TYPE::LAYER_TRAIL
            || i == LAYER_TYPE::LAYER_NPC)
            continue;

        // 2. ObjectCount
        list<CGameObject*>& GameObjects = GI->Find_GameObjects(LEVEL_TOOL, i);
        File->Write<_uint>(GameObjects.size());

        for (auto& Object : GameObjects)
        {
            CTransform* pTransform = Object->Get_Component<CTransform>(L"Com_Transform");
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


            for (_uint iCollider = 0; iCollider < CCollider::DETECTION_TYPE::DETECTION_END; iCollider++)
            {
                const vector<CCollider*>& ObjectCollider = Object->Get_Collider(iCollider);

                File->Write<_uint>(ObjectCollider.size());
                for (auto& pCollider : ObjectCollider)
                {
                    File->Write<_uint>(pCollider->Get_ColliderType());
                    File->Write<_float3>(pCollider->Get_Offset());
                    if (pCollider->Get_ColliderType() == CCollider::AABB)
                    {
                        File->Write<BoundingBox>(dynamic_cast<CCollider_AABB*>(pCollider)->Get_AABB_Box());
                    }
                    else if (pCollider->Get_ColliderType() == CCollider::SPHERE)
                    {
                        File->Write<BoundingSphere>(dynamic_cast<CCollider_Sphere*>(pCollider)->Get_Sphere());
                    }
                    else
                    {
                        return E_FAIL;
                    }
                }
            }
        }

    }

    MSG_BOX("Map_Saved.");
    return S_OK;
}

HRESULT CImGui_Manager::Save_Effect(const wstring& strFullPath)
{
    auto path = filesystem::path(strFullPath);
    //if (filesystem::exists(path))
    //{
    //    MSG_BOX("File is Exist. Press Other Name.");
    //    return E_FAIL;
    //}
        
    

    filesystem::create_directories(path.parent_path());

    _tchar strFileName[MAX_PATH];
    _wsplitpath_s(strFullPath.c_str(), nullptr, 0, nullptr, 0, strFileName, MAX_PATH, nullptr, 0);


    shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
    File->Open(strFullPath, FileMode::Write);

    File->Write<string>(CUtils::ToString(m_pPrevEffect->Get_EffectModelName()));
    File->Write<CEffect::EFFECT_TYPE>(m_pPrevEffect->Get_EffectType());
    
    CEffect::EFFECT_DESC EffectDesc = m_pPrevEffect->Get_EffectDesc();
    File->Write<string>(CUtils::ToString(EffectDesc.strDiffuseTetextureName));
    File->Write<string>(CUtils::ToString(EffectDesc.strAlphaTexturName));
    
    File->Write<_bool>(EffectDesc.bBillboard);
    File->Write<_int>(EffectDesc.bCutUV);
    
    File->Write<_float>(EffectDesc.fAlpha);
    File->Write<_float>(EffectDesc.fDestAlphaSpeed);
    File->Write<_float>(EffectDesc.fIndexSpeed);
    File->Write<_float>(EffectDesc.fMaxCountX);
    File->Write<_float>(EffectDesc.fMaxCountY);
    File->Write<_float>(EffectDesc.fMoveSpeed);
    File->Write<_float>(EffectDesc.fTurnSpeed);

    File->Write<_float>(EffectDesc.fBlurPower);
    File->Write<_float3>(EffectDesc.vBloomPower);
    File->Write<_float2>(EffectDesc.vUVFlow);

    File->Write<_float>(EffectDesc.fScaleSpeed);
    File->Write<_float3>(EffectDesc.vScaleDir);

    File->Write<_float3>(EffectDesc.vMoveDir);
    File->Write<_float3>(EffectDesc.vTurnDir);

    
    File->Write<_float4>(EffectDesc.vDiffuseColor);
    File->Write<_float4>(EffectDesc.vAdditiveDiffuseColor);
    File->Write<_float4x4>(EffectDesc.OffsetMatrix);
    

    File->Write<_bool>(m_pPrevEffect->Is_Loop());
    File->Write<_bool>(m_pPrevEffect->Is_Increment());
    File->Write<_bool>(m_pPrevEffect->Is_Gravity());
 

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
    File->Open(strFullPath, FileMode::Read);

    string strEffectModelName;
    strEffectModelName = File->Read<string>();

    CEffect::EFFECT_TYPE eType;
    eType = File->Read<CEffect::EFFECT_TYPE>();


    CEffect::EFFECT_DESC EffectDesc = {};
    EffectDesc.strDiffuseTetextureName = CUtils::ToWString(File->Read<string>());
    EffectDesc.strAlphaTexturName = CUtils::ToWString(File->Read<string>());

    EffectDesc.bBillboard = File->Read<_bool>();
    EffectDesc.bCutUV = File->Read<_int>();

    EffectDesc.fAlpha = File->Read<_float>();
    EffectDesc.fDestAlphaSpeed = File->Read<_float>();
    EffectDesc.fIndexSpeed = File->Read<_float>();
    EffectDesc.fMaxCountX = File->Read<_float>();
    EffectDesc.fMaxCountY = File->Read<_float>();
    EffectDesc.fMoveSpeed = File->Read<_float>();
    EffectDesc.fTurnSpeed = File->Read<_float>();

    EffectDesc.fBlurPower = File->Read<_float>();
    EffectDesc.vBloomPower = File->Read<_float3>();
    EffectDesc.vUVFlow = File->Read<_float2>();

    EffectDesc.fScaleSpeed = File->Read<_float>();
    EffectDesc.vScaleDir = File->Read<_float3>();

    EffectDesc.vMoveDir = File->Read<_float3>();
    EffectDesc.vTurnDir = File->Read<_float3>();

    EffectDesc.vDiffuseColor = File->Read<_float4>();
    EffectDesc.vAdditiveDiffuseColor = File->Read<_float4>();
    EffectDesc.OffsetMatrix = File->Read<_float4x4>();

    _bool bLoop, bIncrement, bGravity;
    bLoop = File->Read<_bool>();
    bIncrement = File->Read<_bool>();
    bGravity = File->Read<_bool>();
    
    Safe_Release(m_pPrevEffect);
    m_pPrevEffect = CEffect::Create(m_pDevice, m_pContext, L"Prev_Effect", eType, CUtils::ToWString(strEffectModelName), EffectDesc, bLoop, bIncrement, bGravity);

    if (nullptr == m_pPrevEffect)
    {
        MSG_BOX("Effect_Create 실패.");
        return E_FAIL;
    }
        

    if (FAILED(m_pPrevEffect->Initialize(nullptr)))
    {
        MSG_BOX("Effect_초기화 실패.");
        return E_FAIL;
    }
    
    m_pPrevEffect->Set_Loop(bLoop);
    m_pPrevEffect->Set_Increment(bIncrement);
    m_pPrevEffect->Set_Gravity(bGravity);

    return S_OK;
}

HRESULT CImGui_Manager::Save_Particle(const wstring& strFullPath)
{
    auto path = filesystem::path(strFullPath);
    filesystem::create_directories(path.parent_path());

    _tchar strFileName[MAX_PATH];
    _wsplitpath_s(strFullPath.c_str(), nullptr, 0, nullptr, 0, strFileName, MAX_PATH, nullptr, 0);


    shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
    File->Open(strFullPath, FileMode::Write);

    File->Write<string>(CUtils::ToString(strFileName));
    File->Write<string>(CUtils::ToString(m_pPrevParticle->Get_EffectPrototypeName()));
    File->Write<CParticle::PARTICLE_DESC>(m_pPrevParticle->Get_ParticleDesc());

    MSG_BOX("Particle Saved.");
    return S_OK;
}

HRESULT CImGui_Manager::Load_Particle(const wstring& strFullPath)
{
    auto path = filesystem::path(strFullPath);
    filesystem::create_directories(path.parent_path());

    _tchar strFileName[MAX_PATH];
    _wsplitpath_s(strFullPath.c_str(), nullptr, 0, nullptr, 0, strFileName, MAX_PATH, nullptr, 0);



    shared_ptr<CFileUtils> File = make_shared<CFileUtils>();
    File->Open(strFullPath, FileMode::Read);

    wstring strParticleName = CUtils::ToWString(File->Read<string>());
    wstring strEffectPrototypeEffectName = CUtils::ToWString(File->Read<string>());
    CParticle::PARTICLE_DESC ParticleDesc = File->Read<CParticle::PARTICLE_DESC>();

    Safe_Release(m_pPrevParticle);

    m_pPrevParticle = CParticle::Create(m_pDevice, m_pContext, strParticleName, strEffectPrototypeEffectName, ParticleDesc);
    if (nullptr == m_pPrevParticle)
    {
        MSG_BOX("Particle Generate Failed. : CImGui_Manager");
        return E_FAIL;
    }

    if (FAILED(m_pPrevParticle->Initialize(nullptr)))
    {
        MSG_BOX("Particle Initialize Failed. : CImGui_Manager");
        Safe_Release(m_pPrevParticle);
        return E_FAIL;
    }

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

            m_EffectsModelFiles.push_back(wstring(L"Prototype_Component_Model_") + strFileName);
        }
    }

    return S_OK;
}

void CImGui_Manager::PickingTerrainObj()
{
    _float4 vHitPos;
    if (CPicking_Manager::GetInstance()->Is_Picking(m_pTerrain->Get_TransformCom(), m_pTerrain->Get_TerrainBufferCom(), true, &vHitPos))
    {
        CTransform* pTransform = m_pPrevObject->Get_Component<CTransform>(L"Com_Transform");
        if (nullptr != pTransform)
            pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vHitPos));

        return;
    }

}


void CImGui_Manager::PickingGroundObj()
{
    _float4 vHitPos;

    

    for (_uint i = 0; i < LAYER_TYPE::LAYER_END; ++i)
    {
        list<CGameObject*>& GameObjects = GI->Find_GameObjects(LEVEL_TOOL, i);

        for (auto& pGameObject : GameObjects)
        {
            if (!pGameObject->Is_NaviObject())
                continue;

            CTransform* pTransform;
            pTransform = pGameObject->Get_Component<CTransform>(L"Com_Transform") == nullptr ? nullptr : pGameObject->Get_Component<CTransform>(L"Com_Transform");
            if (nullptr == pTransform)
                continue;

            CModel* pModel = pGameObject->Get_Component<CModel>(L"Com_Model");
            if (nullptr == pModel)
                continue;

            const vector<CMesh*>& Meshes = pModel->Get_Meshes();
            for (auto& pMesh : Meshes)
            {
                if (CPicking_Manager::GetInstance()->Is_Picking(pTransform, pMesh, false, &vHitPos))
                {
                    CTransform* pTransform = m_pPrevObject->Get_Component<CTransform>(L"Com_Transform");
                    vHitPos.x = floor(vHitPos.x);
                    vHitPos.y = floor(vHitPos.y);
                    vHitPos.z = floor(vHitPos.z);
                    if (nullptr != pTransform)
                        pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vHitPos));

                    return;
                }
            }
        }
    }

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
                CTransform* pTransform = m_pPrevObject->Get_Component<CTransform>(L"Com_Transform");
                if (nullptr != pTransform)
                    pTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat4(&vHitPos));

                return;
            }
        }
    }


    
    PickingTerrainObj();
}



void CImGui_Manager::Draw_NaviPicking_Point()
{
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(GAME_INSTANCE->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));


    m_pEffect->Apply(m_pContext);

    m_pContext->IASetInputLayout(m_pInputLayout);


    m_pBatch->Begin();
    DirectX::BoundingSphere tSphere;
    ZeroMemory(&tSphere, sizeof(DirectX::BoundingSphere));
    m_vNaviPickingWorldPos.y += 0.02f;
    XMStoreFloat3(&tSphere.Center, XMLoadFloat3(&m_vNaviPickingWorldPos));
    tSphere.Radius = 0.5f;
    DX::Draw(m_pBatch, tSphere, XMVectorSet(0.f, 1.f, 0.f, 1.f));

    for (size_t i = 0; i < m_vWorldPickedNaviPos.size(); ++i)
    {
        XMStoreFloat3(&tSphere.Center, XMLoadFloat3(&m_vWorldPickedNaviPos[i]));
        tSphere.Radius = 0.5f;
        DX::Draw(m_pBatch, tSphere, XMVectorSet(1.f, 0.f, 0.f, 1.f));
    }

    


    m_pBatch->End();
}


void CImGui_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);


    Safe_Release(m_pTarget);
    m_pTarget = nullptr;

    Safe_Release(m_pDummy);
    m_pDummy = nullptr;

    Safe_Release(m_pTerrain);
    m_pTerrain = nullptr;
    

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

}

HRESULT CImGui_Manager::NaviBake()
{

    return S_OK;
}

HRESULT CImGui_Manager::NaviAutoGenerate()
{
    CNavigation* pNavigation = m_pTerrain->Get_NavigationCom();

    if(nullptr == pNavigation || FAILED(pNavigation->Clear_Cells()))
        return E_FAIL;

    for (_uint i = 0; i < LAYER_TYPE::LAYER_END; ++i)
    {
        if (i == LAYER_TYPE::LAYER_CAMERA
            || i == LAYER_TYPE::LAYER_TERRAIN
            || i == LAYER_TYPE::LAYER_BACKGROUND
            || i == LAYER_TYPE::LAYER_SKYBOX
            || i == LAYER_TYPE::LAYER_UI)
            continue;

        list<CGameObject*> GameObjects = GI->Find_GameObjects(LEVEL_TOOL, i);
        for (auto& Object : GameObjects)
        {
            if (Object->Is_NaviObject())
            {
                CModel* pModelCom = Object->Get_Component<CModel>(L"Com_Model");
                CTransform* pTransformCom = Object->Get_Component<CTransform>(L"Com_Transform");
                if (nullptr == pModelCom)
                    continue;

                const vector<CMesh*>& Meshes = pModelCom->Get_Meshes();
                for (auto& pMesh : Meshes)
                {
                    const vector<_float3>& Vertices = pMesh->Get_VertexLocalPositions();
                    const vector<FACEINDICES32>& Indices = pMesh->Get_FaceIndices();

                    for (_uint i = 0; i < Indices.size(); ++i)
                    {
                        _vector v0, v1, v2;
                        _float3 fv0, fv1, fv2;
                        v0 = XMVectorSet(Vertices[Indices[i]._0].x, Vertices[Indices[i]._0].y, Vertices[Indices[i]._0].z, 1.f);
                        v1 = XMVectorSet(Vertices[Indices[i]._1].x, Vertices[Indices[i]._1].y, Vertices[Indices[i]._1].z, 1.f);
                        v2 = XMVectorSet(Vertices[Indices[i]._2].x, Vertices[Indices[i]._2].y, Vertices[Indices[i]._2].z, 1.f);

                        _vector vLineAB = XMVectorSetY(v1 - v0, 0.f);
                        _vector vLineAC = XMVectorSetY(v2 - v0, 0.f);

                        if (XMVectorGetY(XMVector3Cross(vLineAB, vLineAC)) < 0.f)
                        {
                            _vector vTemp = v1;
                            v1 = v2;
                            v2 = vTemp;
                        }


                        XMStoreFloat3(&fv0, v0);
                        XMStoreFloat3(&fv1, v1);
                        XMStoreFloat3(&fv2, v2);

                        _vector vNormal = XMVector3Normalize(XMVector3TransformNormal(XMVector3Normalize(XMVector3Cross(v1 - v0, v2 - v0)), pTransformCom->Get_WorldMatrix()));
                        _float fRadian = XMVectorGetX(XMVector3Dot(vNormal, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
                        _float fTriangleScale = fabs((fv0.x * fv1.z) + (fv1.x * fv2.z) + (fv2.x * fv0.z) - (fv1.x * fv0.z) - (fv2.x * fv1.z) - (fv0.x * fv2.z)) * 0.5f;

                        if (fRadian > m_fGenerateRadian 
                            && fTriangleScale >= m_fGenerateMinScale 
                            && fTriangleScale <= m_fGenerateMaxScale)
                        {
                            _float3  vPoints[3] = {};
                            XMStoreFloat3(&vPoints[0], XMVector3TransformCoord(v0, pTransformCom->Get_WorldMatrix()));
                            XMStoreFloat3(&vPoints[1], XMVector3TransformCoord(v1, pTransformCom->Get_WorldMatrix()));
                            XMStoreFloat3(&vPoints[2], XMVector3TransformCoord(v2, pTransformCom->Get_WorldMatrix()));

                            if (vPoints[0].y > m_fMinTriangleY
                                && vPoints[1].y > m_fMinTriangleY
                                && vPoints[2].y > m_fMinTriangleY
                                && vPoints[0].y < m_fMaxTriangleY
                                && vPoints[1].y < m_fMaxTriangleY
                                && vPoints[2].y < m_fMaxTriangleY)
                            {
                                m_pTerrain->Get_NavigationCom()->Create_Cell(vPoints);
                            }
                        }

                    }
                }
                
            }
        }
    }
    
    m_vWorldPickedNaviPos.clear();


    return S_OK;
}

HRESULT CImGui_Manager::NaviSave(const wstring& strNaviName)
{
    wstring strNaviPath = CUtils::ToWString("../Bin/DataFiles/Map/") + strNaviName + L"/" + strNaviName + L".nav";
    if (FAILED(m_pTerrain->Get_NavigationCom()->Save_NaviData(strNaviPath)))
        return E_FAIL;
    MSG_BOX("Save OK.");
    return S_OK;
}

HRESULT CImGui_Manager::NaviLoad(const wstring& strNaviName)
{
    wstring strNaviPath = CUtils::ToWString("../Bin/DataFiles/Map/") + strNaviName + L"/" + strNaviName + L".nav";
    if (FAILED(m_pTerrain->Get_NavigationCom()->Load_NaviData(strNaviPath)))
        return E_FAIL;

    MSG_BOX("Load OK.");
    return S_OK;
}

void CImGui_Manager::Tick_Particle_Tool(_float fTimeDelta)
{
    ImGui::Begin("Particle");

    static char szParticleEffectName[MAX_PATH] = "";
    ImGui::Text("Particle_Effect_Prototype_Name");
    ImGui::InputText("##ParticleEffectName", szParticleEffectName, MAX_PATH);

    const map<const wstring, CGameObject*>& Effects = GI->Find_Prototype_GameObjects(LAYER_TYPE::LAYER_EFFECT);


    if (ImGui::BeginListBox("##Prototype_Effects"))
    {

        for (auto& iter : Effects)
        {
            string strPrototypeEffectName = CUtils::ToString(iter.first);

            if (ImGui::Selectable(strPrototypeEffectName.c_str(), 0 == strcmp(strPrototypeEffectName.c_str(), szParticleEffectName)))
            {
                strcpy_s(szParticleEffectName, strPrototypeEffectName.c_str());
                if(nullptr != m_pPrevParticle)
                    m_pPrevParticle->Set_EffectPrototypeName(CUtils::ToWString(strPrototypeEffectName));
            }
                
        }
        ImGui::EndListBox();
    }

    
    if (ImGui::Button("Particle_Generate"))
    {
        if (nullptr != m_pPrevParticle)
            Safe_Release(m_pPrevParticle);


        CParticle::PARTICLE_DESC ParticleDesc;
        ZeroMemory(&ParticleDesc, sizeof(CParticle::PARTICLE_DESC));


        m_pPrevParticle = CParticle::Create(m_pDevice, m_pContext, L"Particle"
            , CUtils::ToWString(szParticleEffectName)
            , ParticleDesc);

        if (nullptr == m_pPrevParticle)
        {
            MSG_BOX("Particle Generate Failed.");
        }

        if (FAILED(m_pPrevParticle->Initialize(nullptr)))
        {
            MSG_BOX("Particle Initialize Failed.");
            Safe_Release(m_pPrevParticle);
        }
    }
    
    

    if (m_pPrevParticle != nullptr)
    {
        if (m_pPrevParticle->Is_Dead())
        {
            if (ImGui::Button("Play"))
            {
                if (FAILED(m_pPrevParticle->Ready_Effects()))
                    MSG_BOX("Play Failed.");

                m_pPrevParticle->Set_Dead(false);
            }
        }

        CParticle::PARTICLE_DESC ParticleDesc = m_pPrevParticle->Get_ParticleDesc();


        ImGui::Text("Effect_Count");
        IMGUI_SAME_LINE;
        ImGui::DragInt("##ParticleEffect_Count", (int*)&ParticleDesc.iNumEffectCount, 1.f, 1.f, 100.f);
        
        ImGui::Text("Particle Bill Board");
        IMGUI_SAME_LINE;
        ImGui::Checkbox("##ParticleBillBoard", &ParticleDesc.bBillboard);

        ImGui::Text("Particle LifeTime");
        IMGUI_SAME_LINE;
        ImGui::DragFloat("##Particle_LifeTime", (_float*)&ParticleDesc.fLifeTime, 0.01f, 0.f, 1000.f);
        


        ImGui::Text("Random Speed");
        IMGUI_SAME_LINE;
        ImGui::Checkbox("##ParticleRandom_Speed", &ParticleDesc.bRandomSpeed);

        ImGui::Text("Particle Speed");
        ImGui::DragFloat("##Particle_Speed", &ParticleDesc.fSpeed, 0.01f, 0.f, 1000.f);

        IMGUI_NEW_LINE;

        ImGui::Text("Particle BlurPower");
        ImGui::DragFloat("##Particle_BlurPower", &ParticleDesc.fBlurPower, 0.01f, 0.01f, 1000.f);
        
        IMGUI_NEW_LINE;
        

        ImGui::Text("Random Dir");
        IMGUI_SAME_LINE;
        ImGui::Checkbox("##Particle_Random_Dir", &ParticleDesc.bRandomDir);

        IMGUI_NEW_LINE;
        ImGui::Text("Direction");
        ImGui::DragFloat3("##Particle_Dir", (_float*)&ParticleDesc.vDir, 0.01f, 0.f, 1000.f);

        IMGUI_NEW_LINE;


        ImGui::Text("== RigidBody ==");
        ImGui::Checkbox("Gravity", &ParticleDesc.bRigidActive);

        IMGUI_NEW_LINE;
       
        ImGui::DragFloat3("##RigidBodyDir", (_float*)&ParticleDesc.vForceDir, 0.01f, 0.f, 1000.f);
        ImGui::Checkbox("Random ForceDir", &ParticleDesc.bRandomForceDir);


        ImGui::Text("== Force ==");
        ImGui::DragFloat("##RigidBodyForce", &ParticleDesc.fForce, 0.01f, 0.f, 1000.f);


        ImGui::Text("== Friction ==");
        ImGui::DragFloat("##RigidBodyFriction", &ParticleDesc.fFrictionScale, 0.01f, 0.f, 100.f);

        ImGui::Text("== Random Friction ==");
        ImGui::Checkbox("##Random Friction", &ParticleDesc.bRandomFriction);

        IMGUI_NEW_LINE;

        ImGui::Text("== Alpha Speed ==");
        ImGui::DragFloat("##Particle_Decrease_Alpha", &ParticleDesc.fDestAlphaSpeed, 0.01f, 0.f, 100.f);

        
        m_pPrevParticle->Set_ParticleDesc(ParticleDesc);



        IMGUI_NEW_LINE;
        static char szParticleFileName[MAX_PATH] = "";
        ImGui::Text("Particle File Name");
        ImGui::InputText("##Particle_Name", szParticleFileName, MAX_PATH);


        if (ImGui::Button("Save_Particle"))
        {
            if (0 == strcmp(szParticleFileName, ""))
            {
                MSG_BOX("파티클 파일 이름을 설정해주세요.");
            }
            else
            {
                Save_Particle(L"../Bin/Export/Particle/" + CUtils::ToWString(szParticleFileName) + L".particle");
            }
        }

        if (ImGui::Button("Load_Particle"))
        {
            if (0 == strcmp(szParticleFileName, ""))
            {
                MSG_BOX("파티클 파일 이름을 설정해주세요.");
            }
            else
            {
                Load_Particle(L"../Bin/Export/Particle/" + CUtils::ToWString(szParticleFileName) + L".particle");
            }
        }


        m_pPrevParticle->Tick(fTimeDelta);
        m_pPrevParticle->LateTick(fTimeDelta);
    }

    ImGui::End();
}

