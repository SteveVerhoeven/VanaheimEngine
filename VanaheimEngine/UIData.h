#pragma once
#include "VanaheimPCH.h"

struct InspectorVariable
{
	virtual ~InspectorVariable()
	{}

	std::string name{};
	DirectX::XMFLOAT2 varRange{};
};
struct STInspectorVariable final : InspectorVariable
{
	size_t* value{};
};
struct FInspectorVariable final : InspectorVariable
{
	float* value{};
};
struct BInspectorVariable final : InspectorVariable
{
	bool* value{};
};
