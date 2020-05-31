#include <ShaderNode/DataTypes/VecData.hpp>

inline VecData::VecData(std::size_t ComponentCount) :
componentCount(ComponentCount),
preview(64, 64, QImage::Format_RGBA8888)
{
	preview.fill(QColor::fromRgb(255, 255, 255, 0));
}

inline QtNodes::NodeDataType VecData::type() const
{
	return Type();
}

inline QtNodes::NodeDataType VecData::Type()
{
	return { "vector", "Vector" };
}
