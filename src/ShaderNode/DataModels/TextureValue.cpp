#include <ShaderNode/DataModels/TextureValue.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/TextureData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <QtWidgets/QFormLayout>

TextureValue::TextureValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_onTextureListUpdateSlot.Connect(GetGraph().OnTextureListUpdate, [&](ShaderGraph*) { OnTextureListUpdate(); });
	m_onTexturePreviewUpdateSlot.Connect(GetGraph().OnTexturePreviewUpdate, [&](ShaderGraph*, std::size_t textureIndex)
	{
		if (m_currentTextureIndex == textureIndex)
			UpdatePreview();
	});

	if (graph.GetTextureCount() > 0)
	{
		auto& firstInput = graph.GetTexture(0);
		m_currentTextureIndex = 0;
		m_currentTextureText = firstInput.name;
	}

	DisableCustomVariableName();
	EnablePreview(true);
}

unsigned int TextureValue::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 0;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

void TextureValue::OnTextureListUpdate()
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

bool TextureValue::ComputePreview(QPixmap& pixmap)
{
	if (!m_currentTextureIndex)
		return false;

	const ShaderGraph& graph = GetGraph();
	const auto& textureEntry = graph.GetTexture(*m_currentTextureIndex);

	pixmap = QPixmap::fromImage(textureEntry.preview);
	return true;
}

void TextureValue::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QComboBox* textureSelection = new QComboBox;
	connect(textureSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentTextureIndex = static_cast<std::size_t>(index);
		else
			m_currentTextureIndex.reset();

		UpdatePreview();

		Q_EMIT dataUpdated(0);
	});

	for (const auto& textureEntry : GetGraph().GetTextures())
		textureSelection->addItem(QString::fromStdString(textureEntry.name));

	layout->addRow(tr("Texture"), textureSelection);
}

Nz::ShaderAst::ExpressionPtr TextureValue::GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	if (!m_currentTextureIndex)
		throw std::runtime_error("invalid inputs");

	assert(count == 0);

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

	return Nz::ShaderBuilder::Uniform(textureEntry.name, expression);
}

auto TextureValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	return Vec4Data::Type();
}

std::shared_ptr<QtNodes::NodeData> TextureValue::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	if (!m_currentTextureIndex)
		return nullptr;

	const ShaderGraph& graph = GetGraph();
	const auto& textureEntry = graph.GetTexture(*m_currentTextureIndex);

	std::shared_ptr<TextureData> textureData;

	switch (textureEntry.type)
	{
		case TextureType::Sampler2D:
			textureData = std::make_shared<Texture2Data>();
			break;
	}

	assert(textureData);

	textureData->preview = textureEntry.preview;

	return textureData;
}
