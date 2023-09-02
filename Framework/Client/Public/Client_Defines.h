#pragma once

/* Ŭ���̾�Ʈ�� �����ϴ� ��� Ŭ������ ��Ŭ����ؾ��� ������ ��´�. */
#include <process.h>

#include <Assimp/scene.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>

namespace Client
{
	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_LOADING, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HWND g_hWnd;

using namespace Client;
