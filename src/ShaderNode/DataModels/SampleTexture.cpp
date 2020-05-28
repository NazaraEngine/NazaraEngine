#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/SampleTexture.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

SampleTexture::SampleTexture(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_output = std::make_shared<Vec4Data>();

	UpdateOutput();
}

unsigned int SampleTexture::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 2;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

void SampleTexture::UpdateOutput()
{
	QImage& output = m_output->preview;

	if (!m_texture || !m_uv)
	{
		output = QImage(1, 1, QImage::Format_RGBA8888);
		output.fill(QColor::fromRgb(0, 0, 0, 0));
		return;
	}

	const QImage& texturePreview = m_texture->preview;

	int textureWidth = texturePreview.width();
	int textureHeight = texturePreview.height();

	const QImage& uv = m_uv->preview;

	int uvWidth = uv.width();
	int uvHeight = uv.height();

	output = QImage(uvWidth, uvHeight, QImage::Format_RGBA8888);

	std::uint8_t* outputPtr = output.bits();
	const std::uint8_t* uvPtr = uv.constBits();
	const std::uint8_t* texturePtr = texturePreview.constBits();
	for (int y = 0; y < uvHeight; ++y)
	{
		for (int x = 0; x < uvWidth; ++x)
		{
			float u = float(uvPtr[0]) / 255;
			float v = float(uvPtr[1]) / 255;

			int texX = std::clamp(int(u * textureWidth), 0, textureWidth - 1);
			int texY = std::clamp(int(v * textureHeight), 0, textureHeight - 1);
			int texPixel = (texY * textureWidth + texX) * 4;

			*outputPtr++ = texturePtr[texPixel + 0];
			*outputPtr++ = texturePtr[texPixel + 1];
			*outputPtr++ = texturePtr[texPixel + 2];
			*outputPtr++ = texturePtr[texPixel + 3];
			uvPtr += 4;
		}
	}

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}

bool SampleTexture::ComputePreview(QPixmap& pixmap)
{
	if (!m_texture || !m_uv)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview);
	return true;
}

Nz::ShaderAst::ExpressionPtr SampleTexture::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	if (!m_texture || !m_uv)
		throw std::runtime_error("invalid inputs");

	assert(count == 2);

	return Nz::ShaderBuilder::Sample2D(expressions[0], expressions[1]);
}

auto SampleTexture::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			switch (portIndex)
			{
				case 0: return Texture2Data::Type();
				case 1: return Vec2Data::Type();
			}

			assert(false);
			throw std::runtime_error("invalid port index");
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return Vec4Data::Type();
		}

		default:
			assert(false);
			throw std::runtime_error("invalid PortType");
	}
}

QString SampleTexture::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			switch (portIndex)
			{
				case 0: return tr("Texture");
				case 1: return tr("UV");
			}

			assert(false);
			throw std::runtime_error("invalid port index");
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return tr("Sample");
		}

		default:
			assert(false);
			throw std::runtime_error("Invalid PortType");
	}
}

bool SampleTexture::portCaptionVisible(QtNodes::PortType /*portType*/, QtNodes::PortIndex /*portIndex*/) const
{
	return true;
}

std::shared_ptr<QtNodes::NodeData> SampleTexture::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	return m_output;
}

void SampleTexture::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	switch (index)
	{
		case 0:
		{
			if (value)
			{
				assert(dynamic_cast<Texture2Data*>(value.get()) != nullptr);

				m_texture = std::static_pointer_cast<Texture2Data>(value);
			}
			else
				m_texture.reset();

			break;
		}

		case 1:
		{
			if (value)
			{
				assert(dynamic_cast<Vec2Data*>(value.get()) != nullptr);

				m_uv = std::static_pointer_cast<Vec2Data>(value);
			}
			else
				m_uv.reset();

			break;
		}

		default:
			assert(false);
			throw std::runtime_error("Invalid PortType");
	}

	UpdateOutput();
}
