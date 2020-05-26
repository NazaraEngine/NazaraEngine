#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/SampleTexture.hpp>
#include <ShaderNode/DataModels/VecValue.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

SampleTexture::SampleTexture(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_output = std::make_shared<Vec4Data>();

	m_onTextureListUpdateSlot.Connect(GetGraph().OnTextureListUpdate, [&](ShaderGraph*) { OnTextureListUpdate(); });
	m_onTexturePreviewUpdateSlot.Connect(GetGraph().OnTexturePreviewUpdate, [&](ShaderGraph*, std::size_t textureIndex)
	{
		if (m_currentTextureIndex == textureIndex)
			UpdatePreview();
	});
}

unsigned int SampleTexture::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 1;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

void SampleTexture::OnTextureListUpdate()
{
	m_currentTextureIndex.reset();

	std::size_t inputIndex = 0;
	for (const auto& textureEntry : GetGraph().GetTextures())
	{
		if (textureEntry.name == m_currentTextureText)
		{
			m_currentTextureIndex = inputIndex;
			break;
		}

		inputIndex++;
	}
}

void SampleTexture::UpdateOutput()
{
	QImage& output = m_output->preview;

	if (!m_currentTextureIndex || !m_uv)
	{
		output = QImage(1, 1, QImage::Format_RGBA8888);
		output.fill(QColor::fromRgb(0, 0, 0, 0));
		return;
	}

	const auto& textureEntry = GetGraph().GetTexture(*m_currentTextureIndex);

	int textureWidth = textureEntry.preview.width();
	int textureHeight = textureEntry.preview.height();

	const QImage& uv = m_uv->preview;

	int uvWidth = uv.width();
	int uvHeight = uv.height();

	output = QImage(uvWidth, uvHeight, QImage::Format_RGBA8888);

	std::uint8_t* outputPtr = output.bits();
	const std::uint8_t* uvPtr = uv.constBits();
	const std::uint8_t* texturePtr = textureEntry.preview.constBits();
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
	if (!m_currentTextureIndex || !m_uv)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview);
	return true;
}

void SampleTexture::BuildNodeEdition(QVBoxLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QComboBox* textureSelection = new QComboBox;
	connect(textureSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentTextureIndex = static_cast<std::size_t>(index);
		else
			m_currentTextureIndex.reset();

		UpdateOutput();
	});

	for (const auto& textureEntry : GetGraph().GetTextures())
		textureSelection->addItem(QString::fromStdString(textureEntry.name));

	layout->addWidget(textureSelection);
}

Nz::ShaderAst::ExpressionPtr SampleTexture::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	if (!m_currentTextureIndex || !m_uv)
		throw std::runtime_error("invalid inputs");

	assert(count == 1);

	const auto& textureEntry = GetGraph().GetTexture(*m_currentTextureIndex);

	Nz::ShaderAst::ExpressionType expression = [&]
	{
		switch (textureEntry.type)
		{
			case TextureType::Sampler2D: return Nz::ShaderAst::ExpressionType::Sampler2D;
		}

		assert(false);
		throw std::runtime_error("Unhandled texture type");
	}();

	auto sampler = Nz::ShaderBuilder::Uniform(textureEntry.name, expression);

	return Nz::ShaderBuilder::Sample2D(sampler, expressions[0]);
}

auto SampleTexture::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			assert(portIndex == 0);
			return Vec2Data::Type();
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return Vec4Data::Type();
		}

		default:
			assert(false);
			throw std::runtime_error("Invalid PortType");
	}
}

QString SampleTexture::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			assert(portIndex == 0);
			return tr("UV");
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

	if (!m_currentTextureIndex)
		return nullptr;

	return m_output;
}

void SampleTexture::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0);

	if (value)
	{
		assert(dynamic_cast<Vec2Data*>(value.get()) != nullptr);

		m_uv = std::static_pointer_cast<Vec2Data>(value);
	}
	else
		m_uv.reset();

	UpdateOutput();
}
