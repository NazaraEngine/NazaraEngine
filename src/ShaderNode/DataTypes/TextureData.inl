#include <ShaderNode/DataTypes/TextureData.hpp>

inline TextureData::TextureData() :
preview(64, 64, QImage::Format_RGBA8888)
{
	preview.fill(QColor::fromRgb(255, 255, 255, 0));
}
