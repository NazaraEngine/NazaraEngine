#include <ShaderNode/DataTypes/FloatData.hpp>

inline FloatData::FloatData() :
preview(1, 1)
{
	preview(0, 0) = Nz::Vector4f(1.f, 1.f, 1.f, 0.f);
}
