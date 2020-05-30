#pragma once

#ifndef NAZARA_SHADERNODES_ENUMS_HPP
#define NAZARA_SHADERNODES_ENUMS_HPP

#include <cstddef>

enum class InputRole
{
	None,
	Normal,
	Position,
	TexCoord,

	Max = TexCoord
};

constexpr std::size_t InputRoleCount = static_cast<std::size_t>(InputRole::Max) + 1;

enum class InOutType
{
	Bool,
	Float1,
	Float2,
	Float3,
	Float4,

	Max = Float4
};

constexpr std::size_t InOutTypeCount = static_cast<std::size_t>(InOutType::Max) + 1;

enum class TextureType
{
	Sampler2D,

	Max = Sampler2D
};

constexpr std::size_t TextureTypeCount = static_cast<std::size_t>(TextureType::Max) + 1;

const char* EnumToString(InputRole role);
const char* EnumToString(InOutType input);
const char* EnumToString(TextureType textureType);

#include <ShaderNode/Enums.inl>

#endif
