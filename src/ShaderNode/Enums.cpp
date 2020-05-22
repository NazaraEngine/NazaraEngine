#include <Enums.hpp>
#include <cassert>

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

const char* EnumToString(InputType input)
{
	switch (input)
	{
		case InputType::Bool:  return "Bool";
		case InputType::Float1: return "Float";
		case InputType::Float2: return "Float2";
		case InputType::Float3: return "Float3";
		case InputType::Float4: return "Float4";
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
