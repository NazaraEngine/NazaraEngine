#pragma once

#ifndef NAZARA_SHADERNODES_ENUMS_HPP
#define NAZARA_SHADERNODES_ENUMS_HPP

#include <cstddef>
#include <optional>
#include <string>

enum class BufferType
{
	UniformBufferObject,

	Max = UniformBufferObject
};

constexpr std::size_t BufferTypeCount = static_cast<std::size_t>(BufferType::Max) + 1;

enum class InputRole
{
	None,
	Normal,
	Position,
	TexCoord,

	Max = TexCoord
};

constexpr std::size_t InputRoleCount = static_cast<std::size_t>(InputRole::Max) + 1;

enum class PrimitiveType
{
	Bool,
	Float1,
	Float2,
	Float3,
	Float4,
	Mat4x4,

	Max = Mat4x4
};

constexpr std::size_t PrimitiveTypeCount = static_cast<std::size_t>(PrimitiveType::Max) + 1;

enum class TextureType
{
	Sampler2D,

	Max = Sampler2D
};

constexpr std::size_t TextureTypeCount = static_cast<std::size_t>(TextureType::Max) + 1;


template<typename T> std::optional<T> DecodeEnum(const std::string_view& str);
const char* EnumToString(BufferType bufferType);
const char* EnumToString(InputRole role);
const char* EnumToString(PrimitiveType input);
const char* EnumToString(TextureType textureType);
std::size_t GetComponentCount(PrimitiveType type);

#include <ShaderNode/Enums.inl>

#endif
