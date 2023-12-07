#include "Sound_Manager.h"
#include <io.h>

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
	: m_pSystem(nullptr)
{

}

CSound_Manager::~CSound_Manager()
{
	Free();
}

HRESULT CSound_Manager::Reserve_Manager()
{
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	
	Load_SoundFile("../../Client/Bin/Resources/Sound/Bgm/");
	Load_SoundFile("../../Client/Bin/Resources/Sound/Battle/");
	Load_SoundFile("../../Client/Bin/Resources/Sound/Foot/");
	Load_SoundFile("../../Client/Bin/Resources/Sound/Skill/");
	Load_SoundFile("../../Client/Bin/Resources/Sound/Slash/");
	Load_SoundFile("../../Client/Bin/Resources/Sound/UI/");
	Load_SoundFile("../../Client/Bin/Resources/Sound/Voice/");

	return S_OK;
}


void CSound_Manager::Play_Sound(TCHAR* pSoundKey, CHANNELID eID, _float fVolume, _bool bStop)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	if (true == bStop)
		Stop_Sound(eID);

	FMOD_BOOL bPlay = FALSE;

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID]);
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
	FMOD_System_Update(m_pSystem);
}


void CSound_Manager::Play_BGM(TCHAR* pSoundKey, _float fVolume, _bool bStop)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	if (true == bStop)
		Stop_Sound(SOUND_BGM);

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Stop_Sound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound_Manager::Stop_All()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSound_Manager::Set_ChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Load_SoundFile(const char* szSoundFilePath)
{
	_finddata_t fd;

	string Path = szSoundFilePath;
	string Exe = "*.*";
	string FullPath = Path + Exe;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst(FullPath.c_str(), &fd);


	
	if (handle == -1)
		return;

	int iResult = 0;

	char	szCurPath[128];
	strcpy_s(szCurPath, sizeof(char) * 128, Path.c_str());

	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);
		
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
