#include <ShaderNode/DataModels/VecData.hpp>

inline VecData::VecData() :
preview(64, 64, QImage::Format_RGBA8888)
{
	preview.fill(QColor::fromRgb(255, 255, 255, 0));
}
