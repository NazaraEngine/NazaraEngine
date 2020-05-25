#include <ShaderNode/Previews/QuadPreview.hpp>
#include <cassert>

QImage QuadPreview::GetImage(InputRole role, std::size_t roleIndex) const
{
	assert(role == InputRole::TexCoord);
	assert(roleIndex == 0);

	QImage uv(128, 128, QImage::Format_RGBA8888);

	std::uint8_t* content = uv.bits();
	for (std::size_t y = 0; y < 128; ++y)
	{
		for (std::size_t x = 0; x < 128; ++x)
		{
			*content++ = (x * 255) / 128;
			*content++ = (y * 255) / 128;
			*content++ = 0;
			*content++ = 255;
		}
	}

	return uv;
}
