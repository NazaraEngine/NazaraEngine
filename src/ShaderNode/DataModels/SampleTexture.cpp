#include <ShaderGraph.hpp>
#include <DataModels/SampleTexture.hpp>
#include <DataModels/VecValue.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

SampleTexture::SampleTexture(ShaderGraph& graph) :
ShaderNode(graph),
m_currentTextureIndex(0)
{
	m_output = std::make_shared<Vec4Data>();

	m_layout = new QVBoxLayout;

	m_textureSelection = new QComboBox;
	m_textureSelection->setStyleSheet("background-color: rgba(255,255,255,255)");
	connect(m_textureSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index < 0)
			return;

		m_currentTextureIndex = static_cast<std::size_t>(index);
		UpdatePreview();
	});

	m_layout->addWidget(m_textureSelection);

	m_pixmap = QPixmap(64, 64);
	m_pixmap.fill();

	m_pixmapLabel = new QLabel;
	m_pixmapLabel->setPixmap(m_pixmap);

	m_layout->addWidget(m_pixmapLabel);

	m_widget = new QWidget;
	m_widget->setStyleSheet("background-color: rgba(0,0,0,0)");
	m_widget->setLayout(m_layout);

	m_onTextureListUpdateSlot.Connect(GetGraph().OnTextureListUpdate, [&](ShaderGraph*) { UpdateTextureList(); });
	m_onTexturePreviewUpdateSlot.Connect(GetGraph().OnTexturePreviewUpdate, [&](ShaderGraph*, std::size_t textureIndex)
	{
		if (m_currentTextureIndex == textureIndex)
			UpdatePreview();
	});

	UpdateTextureList();
	UpdatePreview();
}

QWidget* SampleTexture::embeddedWidget()
{
	return m_widget;
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

void SampleTexture::UpdatePreview()
{
	if (m_textureSelection->count() == 0)
		return;

	ComputePreview(m_pixmap);
	m_pixmapLabel->setPixmap(m_pixmap);

	Q_EMIT dataUpdated(0);
}

void SampleTexture::UpdateTextureList()
{
	QString currentTexture = m_textureSelection->currentText();
	m_textureSelection->clear();

	for (const auto& textureEntry : GetGraph().GetTextures())
		m_textureSelection->addItem(QString::fromStdString(textureEntry.name));

	m_textureSelection->setCurrentText(currentTexture);
}

void SampleTexture::ComputePreview(QPixmap& pixmap) const
{
	if (!m_uv)
		return;

	const auto& textureEntry = GetGraph().GetTexture(m_currentTextureIndex);

	int textureWidth = textureEntry.preview.width();
	int textureHeight = textureEntry.preview.height();

	QImage& output = m_output->preview;
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

	pixmap = QPixmap::fromImage(output).scaled(128, 128, Qt::KeepAspectRatio);
}

Nz::ShaderAst::ExpressionPtr SampleTexture::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	assert(count == 1);

	const auto& textureEntry = GetGraph().GetTexture(m_currentTextureIndex);

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

	UpdatePreview();
}
