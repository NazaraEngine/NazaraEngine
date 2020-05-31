#include <ShaderNode/DataTypes/VecData.hpp>
#include <cassert>
#include <stdexcept>

Nz::ShaderAst::ExpressionType VecData::GetExpressionType() const
{
	switch (componentCount)
	{
		case 2: return Nz::ShaderAst::ExpressionType::Float2;
		case 3: return Nz::ShaderAst::ExpressionType::Float3;
		case 4: return Nz::ShaderAst::ExpressionType::Float4;
		default:
			break;
	}

	assert(false);
	throw std::runtime_error("invalid component count");
}
