#pragma once

#ifndef NAZARA_SHADERNODES_ENUMS_HPP
#define NAZARA_SHADERNODES_ENUMS_HPP

#include <cstddef>
#include <optional>
#include <string>

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


template<typename T> std::optional<T> DecodeEnum(const std::string_view& str);
const char* EnumToString(InputRole role);
const char* EnumToString(InOutType input);
const char* EnumToString(TextureType textureType);
std::size_t GetComponentCount(InOutType type);

#include <ShaderNode/Enums.inl>

#endif
