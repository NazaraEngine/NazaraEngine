#include <ShaderGraph.hpp>
#include <DataModels/SampleTexture.hpp>
#include <DataModels/VecValue.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

SampleTexture::SampleTexture(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_layout = new QVBoxLayout;

	m_textureSelection = new QComboBox;
	m_textureSelection->setStyleSheet("background-color: rgba(255,255,255,255)");

	m_layout->addWidget(m_textureSelection);

	m_pixmap = QPixmap(64, 64);
	m_pixmap.fill();

	m_pixmapLabel = new QLabel;
	m_pixmapLabel->setPixmap(m_pixmap);

	m_layout->addWidget(m_pixmapLabel);

	m_widget = new QWidget;
	m_widget->setStyleSheet("background-color: rgba(0,0,0,0)");
	m_widget->setLayout(m_layout);

	m_onTextureListUpdate.Connect(GetGraph().OnTextureListUpdate, [&](ShaderGraph*) { UpdateTextureList(); });
	UpdateTextureList();
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
	ComputePreview(m_pixmap);
	m_pixmapLabel->setPixmap(m_pixmap);
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
	pixmap.fill();
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

	return std::make_shared<Vec4Data>(Nz::Vector4f::Zero());
}
