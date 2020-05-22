#include <ShaderGraph.hpp>
#include <DataModels/SampleTexture.hpp>
#include <DataModels/VecValue.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

SampleTexture::SampleTexture(ShaderGraph& graph) :
ShaderNode(graph),
m_currentTextureIndex(0)
{
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
	const auto& textureEntry = GetGraph().GetTexture(m_currentTextureIndex);

	pixmap = QPixmap::fromImage(textureEntry.preview).scaled(128, 128, Qt::KeepAspectRatio);
}

Nz::ShaderAst::ExpressionPtr SampleTexture::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	assert(count == 1);

	auto sampler = Nz::ShaderBuilder::Uniform("Texture0", Nz::ShaderAst::ExpressionType::Sampler2D);

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

std::shared_ptr<QtNodes::NodeData> SampleTexture::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	const auto& textureEntry = GetGraph().GetTexture(m_currentTextureIndex);

	auto vecData = std::make_shared<Vec4Data>();
	vecData->preview = textureEntry.preview;

	return vecData;
}
