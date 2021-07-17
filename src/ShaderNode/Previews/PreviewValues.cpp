#include <ShaderNode/Previews/PreviewValues.hpp>
#include <QtGui/QImage>

PreviewValues::PreviewValues() :
PreviewValues(0, 0)
{
}

PreviewValues::PreviewValues(std::size_t width, std::size_t height) :
m_height(height),
m_width(width)
{
	m_values.resize(m_width * m_height); //< RGBA
}

void PreviewValues::Fill(const Nz::Vector4f& value)
{
	std::fill(m_values.begin(), m_values.end(), value);
}

QImage PreviewValues::GenerateImage() const
{
	QImage preview(int(m_width), int(m_height), QImage::Format_RGBA8888);

	Nz::UInt8* ptr = preview.bits();

	const Nz::Vector4f* src = m_values.data();
	for (std::size_t i = 0; i < m_values.size(); ++i)
	{
		for (std::size_t y = 0; y < 4; ++y)
			*ptr++ = static_cast<Nz::UInt8>(std::clamp((*src)[y] * 0xFF, 0.f, 255.f));

		*src++;
	}

	return preview;
}

PreviewValues PreviewValues::Resized(std::size_t newWidth, std::size_t newHeight) const
{
	PreviewValues resizedPreview(newWidth, newHeight);

	float xStep = 1.f / newWidth;
	float yStep = 1.f / newHeight;

	for (std::size_t y = 0; y < newHeight; ++y)
	{
		for (std::size_t x = 0; x < newWidth; ++x)
			resizedPreview(x, y) = Sample(x * xStep, y * yStep);
	}

	return resizedPreview;
}

Nz::Vector4f PreviewValues::Sample(float u, float v) const
{
	// Bilinear filtering
	float x = std::clamp(u * m_width, 0.f, m_width - 1.f);
	float y = std::clamp(v * m_height, 0.f, m_height - 1.f);

	auto iX = static_cast<std::size_t>(x);
	auto iY = static_cast<std::size_t>(y);

	float dX = x - iX;
	float dY = y - iY;

	auto ColorAt = [&](std::size_t x, std::size_t y) -> Nz::Vector4f
	{
		x = std::min(x, m_width - 1);
		y = std::min(y, m_height - 1);

		return m_values[y * m_width + x];
	};

	Nz::Vector4f d00 = ColorAt(iX, iY);
	Nz::Vector4f d10 = ColorAt(iX + 1, iY);
	Nz::Vector4f d01 = ColorAt(iX, iY + 1);
	Nz::Vector4f d11 = ColorAt(iX + 1, iY + 1);

	return Nz::Lerp(Nz::Lerp(d00, d10, dX), Nz::Lerp(d01, d11, dX), dY);
}

Nz::Vector4f& PreviewValues::operator()(std::size_t x, std::size_t y)
{
	assert(x < m_width);
	assert(y < m_height);
	return m_values[y * m_width + x];
}

Nz::Vector4f PreviewValues::operator()(std::size_t x, std::size_t y) const
{
	assert(x < m_width);
	assert(y < m_height);
	return m_values[y * m_width + x];
}
