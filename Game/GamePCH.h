#pragma once
// ****************
// PCH file DevGame
// ****************

#pragma region Includes
// ----------------------------
// External includes
// ----------------------------
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>

// ----------------------------
// DirectX includes
// ----------------------------
// Reference: https://fx11.codeplex.com/
#include <../Libs/DX_Effects11/include/d3dx11effect.h>
#include <DirectXMath.h>

// ----------------------------
// Vanaheim includes
// ----------------------------
#include <Locator.h>
#include <DebugLogger.h>
#pragma endregion

#pragma region Macro
/* ***************** */
/* DELETING POINTERS */
/* ***************** */
#define DELETE_POINTER( x ) \
if (x != NULL)			   \
{						   \
    delete x;			   \
    x = nullptr;		   \
}
#define DELETE_POINTERS( x, size ) 	\
for (size_t i{}; i < size; ++i)		\
{									\
	DELETE_POINTER(x[i]);			\
}									\
x.clear();
/* ****************** */
/* DELETING RESOURCES */
/* ****************** */
/*#define DELETE_RESOURCE_VALID( x ) \
if (x->IsValid())           \
{                           \
    x->Release();           \
    x = nullptr;            \
} */  
#define DELETE_RESOURCE_VALID( x ) \
if (x->IsValid())				   \
{								   \
    DELETE_RESOURCE( x );		   \
} 
#define DELETE_RESOURCE( x ) \
if (x)                      \
{                           \
    x->Release();           \
    x = nullptr;            \
}
#pragma endregion