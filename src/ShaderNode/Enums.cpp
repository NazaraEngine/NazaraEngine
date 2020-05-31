#include <ShaderNode/Enums.hpp>
#include <cassert>

std::size_t GetComponentCount(InOutType type)
{
	switch (type)
	{
		case InOutType::Bool:   return 1;
		case InOutType::Float1: return 1;
		case InOutType::Float2: return 2;
		case InOutType::Float3: return 3;
		case InOutType::Float4: return 4;
	}

	assert(false);
	return 0;
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

const char* EnumToString(InOutType input)
{
	switch (input)
	{
		case InOutType::Bool:   return "Bool";
		case InOutType::Float1: return "Float";
		case InOutType::Float2: return "Float2";
		case InOutType::Float3: return "Float3";
		case InOutType::Float4: return "Float4";
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
