#pragma once

#pragma warning (disable : 4251)
#include <d3d11.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <vector> /* 배열 */
#include <list> /* 더블링크드리스트 */
#include <map> /* 이진트리 */
#include <unordered_map> /* 해시테이블 */
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