#include <ShaderNode/DataTypes/TextureData.hpp>

inline TextureData::TextureData() :
preview(64, 64)
{
	preview.Fill(nzsl::Vector4f32(1.f, 1.f, 1.f, 0.f));
}
