// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0501

// Windows Header Files:
#include <windows.h>
#include <winUser.h>
#include <mmsystem.h>

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <crtdbg.h>

// C++ STL
#include <array>
#include <string>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <thread>
#include <atomic>

// DirectX
#include <d3d9.h>
#include <D3DX9.h>

typedef LPDIRECT3DDEVICE9 LPD3DDEVICE;
typedef IDirect3DTexture9 ID3DTexture;
typedef LPDIRECT3DTEXTURE9 LPD3DTEXTURE;
typedef D3DMATERIAL9 D3DMATERIAL;
typedef LPDIRECT3DVERTEXBUFFER9 LPD3DVERTEXBUFFER;

#define ZZ_MEM_OFF // not use zz_mem

#include "zz_interface.h"

// #pragma warning(default : 4786)
#pragma warning(disable : 4651)

#include "rose/common/game_config.h"
#include "rose/common/log.h"

#include "interface/it_mgr.h"
#include "io_terrain.h"

#include "util\CVector.h"
#include "common\DataTYPE.h"

#include "util\CD3DUtil.h"
#include "util\classSTR.h"
#include "util\classLUA.h"
#include "util\classHASH.h"
#include "util\classUTIL.h"
#include "util\SLLIST.h"
#include "util\DLLIST.h"
#include "util\CFileLIST.h"

#include "util\CGameSTR.h"
#include "util\CGameSTB.h"

#include "CApplication.h"

#ifndef SAFE_DELETE
    #define SAFE_DELETE(p)  \
        {                   \
            if (p) {        \
                delete (p); \
                (p) = NULL; \
            }               \
        }
#endif

#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY(p) \
        {                        \
            if (p) {             \
                delete[](p);     \
                (p) = NULL;      \
            }                    \
        }
#endif

#ifndef SAFE_RELEASE
    #define SAFE_RELEASE(p)     \
        {                       \
            if (p) {            \
                (p)->Release(); \
                (p) = NULL;     \
            }                   \
        }
#endif

#ifndef GET_WHEEL_DELTA_WPARAM
    #define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#endif
