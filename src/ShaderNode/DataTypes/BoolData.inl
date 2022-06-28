#include <ShaderNode/DataTypes/BoolData.hpp>

inline BoolData::BoolData() :
preview(1, 1)
{
	preview(0, 0) = nzsl::Vector4f32(1.f, 1.f, 1.f, 0.f);
}
