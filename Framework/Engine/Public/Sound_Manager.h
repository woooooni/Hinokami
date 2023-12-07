#pragma once
#include "Base.h"
#include "Engine_Defines.h"
#include "fmod.h"
#include "fmod.hpp"
BEGIN(Engine)

class ENGINE_DLL CSound_Manager : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)	

private:
	explicit CSound_Manager();
	virtual  ~CSound_Manager();

public:
	HRESULT Reserve_Manager();

public:
	void Play_Sound(TCHAR* pSoundKey, CHANNELID eID, _float fVolume, _bool bStop = false);
	void Play_BGM(TCHAR* pSoundKey, _float fVolume, _bool bStop = false);
	void Stop_Sound(CHANNELID eID);
	void Stop_All();
	void Set_ChannelVolume(CHANNELID eID, float fVolume);

	FMOD_CHANNEL* Get_Channel(CHANNELID eID)
	{
		return m_pChannelArr[eID];
	}

private:
	void Load_SoundFile(const char* szSoundFilePath);

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;

public:
	virtual void Free() override;

};

END