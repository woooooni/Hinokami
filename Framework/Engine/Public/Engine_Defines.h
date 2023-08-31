#pragma once

#pragma warning (disable : 4251)
#include <d3d11.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <vector> /* �迭 */
#include <list> /* ����ũ�帮��Ʈ */
#include <map> /* ����Ʈ�� */
#include <unordered_map> /* �ؽ����̺� */
#include <algorithm>

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


using namespace Engine;



using namespace std;