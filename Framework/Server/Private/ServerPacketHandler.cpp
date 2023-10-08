#include "stdafx.h"
#include "ServerPacketHandler.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerObject_Manager.h"
#include "ServerObject.h"
#include "Model.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// Á÷Á¢ ÄÁÅÙÃ÷ ÀÛ¾÷ÀÚ

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return true;
}
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{


	return true;
}

bool Handle_S_ENTER_LEVEL(PacketSessionRef& session, Protocol::S_ENTER_LEVEL& pkt)
{
	if (pkt.ilevelid() >= LEVELID::LEVEL_END)
		return false;

	
	const map<uint32, vector<CServerObject*>>& MapObjects = CServerObject_Manager::GetInstance()->Get_AllObjects(pkt.ilevelid());
	for (auto& Map : MapObjects)
	{
		for (size_t i = 0; i < LAYER_TYPE::LAYER_END; ++i)
		{
			if (i == LAYER_TYPE::LAYER_CAMERA
				|| i == LAYER_TYPE::LAYER_SKYBOX
				|| i == LAYER_TYPE::LAYER_UI)
				continue;

			for (auto& Object : MapObjects.find(i)->second)
			{
				const CServerObject::OBJECT_INFO& tInfo = Object->Get_ObjectInfo();
				Protocol::C_CREATE_OBJECT SendPkt;

				ZeroMemory(&SendPkt, sizeof(Protocol::C_ENTER_LEVEL));

				SendPkt.set_iobjectid(tInfo.iObjectID);
				SendPkt.set_iobjecttype(tInfo.iObjectType);
				SendPkt.set_strname(tInfo.strName);

				SendPkt.set_strprototypename(tInfo.strPrototypeName);
				SendPkt.set_imodeltype(tInfo.iModelType);

				if (Object->Get_ObjectInfo().iModelType == CModel::TYPE::TYPE_ANIM)
				{
					SendPkt.set_ianimationindex(tInfo.iAnimationIndex);
					SendPkt.set_fanimationplaytime(tInfo.fAnimationPlayTime);
				}
				else
				{
					SendPkt.set_ianimationindex(0);
					SendPkt.set_fanimationplaytime(0.f);
				}
				


				auto matWorld = SendPkt.mutable_mworldmatrix();
				matWorld->Resize(16, 0.0f);

				memcpy(matWorld->mutable_data(), &tInfo.mWorldMatrix, sizeof(_float4x4));

				SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(SendPkt);
				CGameSessionManager::GetInstance()->BroadcastOthers(SendBuffer, session->GetSessionID());
			}
		}
	}


	
	return true;
}

bool Handle_S_EXIT_LEVEL(PacketSessionRef& session, Protocol::S_EXIT_LEVEL& pkt)
{
	Protocol::C_EXIT_LEVEL SendPkt; 
	SendPkt.set_bexit_ok(true);

	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(SendPkt);
	CGameSessionManager::GetInstance()->BroadcastOthers(SendBuffer, session->GetSessionID());
	

	return true;
}

bool Handle_S_CREATE_OBJECT(PacketSessionRef& session, Protocol::S_CREATE_OBJECT& pkt)
{

	CServerObject_Manager::GetInstance()->Add_GameObject(pkt);

	Protocol::C_CREATE_OBJECT CreatePkt;
	CreatePkt.set_iobjectid(pkt.iobjectid());
	CreatePkt.set_iobjecttype(pkt.iobjecttype());
	CreatePkt.set_strname(pkt.strname());

	CreatePkt.set_strprototypename(pkt.strprototypename());
	CreatePkt.set_imodeltype(pkt.imodeltype());
	CreatePkt.set_ianimationindex(pkt.ianimationindex());


	auto matWorld = CreatePkt.mutable_mworldmatrix();
	matWorld->Resize(16, 0.0f);

	memcpy(matWorld->mutable_data(), pkt.mutable_mworldmatrix()->mutable_data(), sizeof(_float4x4));

	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(CreatePkt);
	CGameSessionManager::GetInstance()->BroadcastOthers(SendBuffer, session->GetSessionID());

	return true;
}

bool Handle_S_DELETE_OBJECT(PacketSessionRef& session, Protocol::S_DELETE_OBJECT& pkt)
{
	return false;
}

bool Handle_S_WORLD_MATRIX(PacketSessionRef& session, Protocol::S_WORLD_MATRIX& pkt)
{
	return true;
}

bool Handle_S_CHANGE_ANIMATION(PacketSessionRef& session, Protocol::S_CHANGE_ANIMATION& pkt)
{
	return true;
}
