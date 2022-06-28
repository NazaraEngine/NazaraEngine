#include <ShaderNode/DataTypes/VecData.hpp>

inline VecData::VecData(std::size_t ComponentCount) :
componentCount(ComponentCount),
preview(64, 64)
{
	preview.Fill(nzsl::Vector4f32(1.f, 1.f, 1.f, 0.f));
}

inline QtNodes::NodeDataType VecData::type() const
{
	return Type();
}

inline QtNodes::NodeDataType VecData::Type()
{
	return { "vector", "Vector" };
}
