#include <ShaderNode/Previews/QuadPreview.hpp>
#include <cassert>

PreviewValues QuadPreview::GetPreview(InputRole role, std::size_t roleIndex) const
{
	if (role != InputRole::TexCoord)
	{
		PreviewValues dummy(1, 1);
		dummy(0, 0) = Nz::Vector4f::Zero();

		return dummy;
	}

	PreviewValues uv(128, 128); 

	float invWidth = 1.f / uv.GetWidth();
	float invHeight = 1.f / uv.GetHeight();

	for (std::size_t y = 0; y < uv.GetHeight(); ++y)
	{
		for (std::size_t x = 0; x < uv.GetWidth(); ++x)
			uv(x, y) = Nz::Vector4f(x * invWidth, y * invHeight, 0.f, 1.f);
	}

	return uv;
}
