
#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#ifndef WinSock_Include_h__
#define WinSock_Include_h__

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#endif

#include "PxPhysics.h"
#include "PxPhysicsAPI.h"
using namespace physx;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <FX11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/SimpleMath.h>



#include <Assimp/scene.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>

#define DIRECTINPUT_VERSION 0x0800

//#ifdef _DEBUG
//#pragma comment(lib, "ServerCore.lib")
//#pragma comment(lib, "libprotobufd.lib")
//#else
//#pragma comment(lib, "ServerCore.lib")
//#pragma comment(lib, "libprotobuf.lib")
//#endif
//
//
//#include "CoreExports.h"
//#include "Enum.pb.h"
//
//using GameSessionRef = shared_ptr<class GameSession>;
//using PlayerRef = shared_ptr<class Player>;
#include <mutex>
#include <future>




#include <dinput.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <typeinfo>
#include <array>
#include <process.h>



#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Enum.h"



//#ifdef _DEBUG
//
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
////#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") // 디버그 모드에서 콘솔창 출력.
//
//#ifndef DBG_NEW 
//
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//
//#endif
//
//#endif // _DEBUG

using namespace std;
using namespace Engine;


