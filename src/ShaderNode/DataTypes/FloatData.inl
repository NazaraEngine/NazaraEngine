#include <ShaderNode/DataTypes/FloatData.hpp>

inline FloatData::FloatData() :
preview(1, 1, QImage::Format_RGBA8888)
{
	preview.fill(QColor::fromRgb(255, 255, 255, 0));
}
