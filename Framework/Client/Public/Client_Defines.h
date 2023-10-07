#pragma once

/* 클라이언트에 존재하는 모든 클래스가 인클루드해야할 내용을 담는다. */
#include <process.h>
namespace Client
{
	enum LAYER_TYPE
	{
		LAYER_TERRAIN,
		LAYER_SKYBOX,
		LAYER_CAMERA,
		LAYER_BACKGROUND,
		LAYER_TREE,
		LAYER_PROP,
		LAYER_BUILDING,
		LAYER_PLAYER,
		LAYER_MONSTER,
		LAYER_PROJECTILE,
		LAYER_EFFECT,
		LAYER_UI,
		LAYER_END
	};
	enum OBJ_TYPE
	{
		OBJ_BACKGROUND,
		OBJ_BUILDING,
		OBJ_CAMERA,
		OBJ_CHARACTER,
		OBJ_PLAYER,
		OBJ_MONSTER,
		OBJ_PROP,
		OBJ_MESH_EFFECT,
		OBJ_TEXTURE_EFFECT,
		OBJ_TERRAIN,
		OBJ_DUMMY,
		OBJ_END
	};
	static const char* STR_LAYER_NAME[] = { "TERRAIN", "SKYBOX", "CAMERA", "BACKGROUND", "TREE", "PROP", "BUILDING", "PLAYER", "MONSTER", "PROJECTILE", "EFFECT", "UI" };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_TOOL, LEVEL_LOADING, LEVEL_DUMMY, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

#define SERVER_IP		L"192.168.0.105"
#define SERVER_PORT		7777

using namespace Client;
