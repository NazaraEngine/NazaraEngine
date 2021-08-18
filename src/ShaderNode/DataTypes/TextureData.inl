#include <ShaderNode/DataTypes/TextureData.hpp>

inline TextureData::TextureData() :
preview(64, 64)
{
	preview.Fill(Nz::Vector4f(1.f, 1.f, 1.f, 0.f));
}
