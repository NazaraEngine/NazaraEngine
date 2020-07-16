#include <ShaderNode/DataTypes/VecData.hpp>
#include <cassert>
#include <stdexcept>

Nz::ShaderNodes::BasicType VecData::GetExpressionType() const
{
	switch (componentCount)
	{
		case 2: return Nz::ShaderNodes::BasicType::Float2;
		case 3: return Nz::ShaderNodes::BasicType::Float3;
		case 4: return Nz::ShaderNodes::BasicType::Float4;
		default:
			break;
	}

	assert(false);
	throw std::runtime_error("invalid component count");
}
