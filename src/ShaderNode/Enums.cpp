#include <ShaderNode/Enums.hpp>
#include <cassert>

std::size_t GetComponentCount(PrimitiveType type)
{
	switch (type)
	{
		case PrimitiveType::Bool:   return 1;
		case PrimitiveType::Float1: return 1;
		case PrimitiveType::Float2: return 2;
		case PrimitiveType::Float3: return 3;
		case PrimitiveType::Float4: return 4;
		case PrimitiveType::Mat4x4: return 16;
	}

	assert(false);
	return 0;
}

const char* EnumToString(BufferType bufferType)
{
	switch (bufferType)
	{
		case BufferType::UniformBufferObject: return "UniformBufferObject";
	}

	assert(false);
	return "<Unhandled>";
}

const char* EnumToString(InputRole role)
{
	switch (role)
	{
		case InputRole::None:     return "None";
		case InputRole::Normal:   return "Normal";
		case InputRole::Position: return "Position";
		case InputRole::TexCoord: return "TexCoord";
	}

	assert(false);
	return "<Unhandled>";
}

const char* EnumToString(PrimitiveType input)
{
	switch (input)
	{
		case PrimitiveType::Bool:   return "Bool";
		case PrimitiveType::Float1: return "Float";
		case PrimitiveType::Float2: return "Float2";
		case PrimitiveType::Float3: return "Float3";
		case PrimitiveType::Float4: return "Float4";
		case PrimitiveType::Mat4x4: return "Mat4x4";
	}

	assert(false);
	return "<Unhandled>";
}

const char* EnumToString(ShaderType type)
{
	switch (type)
	{
		case ShaderType::NotSet:   return "NotSet";
		case ShaderType::Fragment: return "Fragment";
		case ShaderType::Vertex:   return "Vertex";
	}

	assert(false);
	return "<Unhandled>";
}

const char* EnumToString(TextureType textureType)
{
	switch (textureType)
	{
		case TextureType::Sampler2D: return "Sampler2D";
	}

	assert(false);
	return "<Unhandled>";
}
