
#ifndef ENGINE_FRAMEWORK_H
#define ENGINE_FRAMEWORK_H

#include <wrl/client.h>
#include <DirectXMath.h>
#include <minwinbase.h>
#include <assert.h>

//================================================================================
//----- Macros
//================================================================================

#define CHECK_NULL(x) assert(x!=nullptr)
#define CHECK_EQUALS(x,y) assert(x==y)
#define CHECK_NOT(x,y) assert(x!=y)
#define UNUSED_VAR(x) (void(x))
#define COMPTR(T, x) Microsoft::WRL::ComPtr<T> x;
#define RELEASECOM(x) if(x!=nullptr){x->Release();x=nullptr;}
#define CLEARMEM(x) ZeroMemory(&x, sizeof(x))

//================================================================================
//----- Typedefs
//================================================================================

typedef DirectX::XMFLOAT4 fVect;
typedef DirectX::XMFLOAT3 fVect3;
typedef DirectX::XMFLOAT2 fVect2;
typedef DirectX::XMMATRIX Matrix;

#endif // ENGINE_FRAMEWORK_H


