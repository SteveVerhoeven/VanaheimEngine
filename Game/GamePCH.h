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
#include "Locator.h"

#include "DebugLogger.h"

#include "ResourceManager.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "InputManager.h"
#include "GeneratorManager.h"

#include "NoiseGenerator.h"
#include "ImageGenerator.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "CameraComponent.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "LineComponent.h"
#include "NameComponent.h"
#include "TerrainGeneratorComponent.h"
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
/* ******* */
/* LOGGING */
/* ******* */
/** Function to log a message to the visual studio console (IF created) and to the engine console */
// @param: errorLevel - The level of message that is sent to the consoles (ErrorLevel::...)
// @param: message    - The message to sent to the consoles
#define LOG( errorLevel, message )	\
Locator::GetDebugLoggerService()->Log(errorLevel, message);
#define LOG_INFO( message )	\
Locator::GetDebugLoggerService()->Log(ErrorLevel::LOG_INFO, message);
#define LOG_WARNING( message )	\
Locator::GetDebugLoggerService()->Log(ErrorLevel::LOG_WARNING, message);
#define LOG_ERROR( message )	\
Locator::GetDebugLoggerService()->Log(ErrorLevel::LOG_ERROR, message);
#define LOG_FATAL( message )	\
Locator::GetDebugLoggerService()->Log(ErrorLevel::LOG_FATAL, message);
#define LOG_HRESULT( hr, function, file, line )	\
Locator::GetDebugLoggerService()->LogHRESULT(hr, function, file, line);
#pragma endregion