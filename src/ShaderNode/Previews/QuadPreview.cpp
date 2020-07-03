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

	for (std::size_t y = 0; y < 128; ++y)
	{
		for (std::size_t x = 0; x < 128; ++x)
			uv(x, y) = Nz::Vector4f(x / 128.f, y / 128.f, 0.f, 1.f);
	}

	return uv;
}
