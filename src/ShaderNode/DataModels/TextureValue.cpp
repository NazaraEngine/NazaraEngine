#include <ShaderNode/DataModels/TextureValue.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/TextureData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <QtWidgets/QFormLayout>

TextureValue::TextureValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_onTextureListUpdateSlot.Connect(GetGraph().OnTextureListUpdate, [&](ShaderGraph*) { OnTextureListUpdate(); });

	auto HandleTextureUpdate = [&](ShaderGraph*, std::size_t textureIndex)
	{
		if (m_currentTextureIndex == textureIndex)
		{
			UpdatePreview();
			Q_EMIT dataUpdated(0);
		}
	};

	m_onTexturePreviewUpdateSlot.Connect(GetGraph().OnTexturePreviewUpdate, HandleTextureUpdate);
	m_onTextureUpdateSlot.Connect(GetGraph().OnTextureUpdate, HandleTextureUpdate);

	if (graph.GetTextureCount() > 0)
	{
		m_currentTextureIndex = 0;
		UpdateTexture();
	}

	DisableCustomVariableName();
	EnablePreview(true);
}

unsigned int TextureValue::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::None:
			break;

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

void TextureValue::UpdateTexture()
{
	if (m_currentTextureIndex)
	{
		auto& texture = GetGraph().GetTexture(*m_currentTextureIndex);
		m_currentTextureText = texture.name;
	}
	else
		m_currentTextureText.clear();
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
	for (const auto& textureEntry : GetGraph().GetTextures())
		textureSelection->addItem(QString::fromStdString(textureEntry.name));

	if (m_currentTextureIndex)
		textureSelection->setCurrentIndex(int(*m_currentTextureIndex));

	connect(textureSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentTextureIndex = static_cast<std::size_t>(index);
		else
			m_currentTextureIndex.reset();

		UpdateTexture();
		UpdatePreview();

		Q_EMIT dataUpdated(0);
	});

	layout->addRow(tr("Texture"), textureSelection);
}

nzsl::Ast::NodePtr TextureValue::BuildNode(nzsl::Ast::ExpressionPtr* /*expressions*/, std::size_t count, std::size_t outputIndex) const
{
	if (!m_currentTextureIndex)
		throw std::runtime_error("invalid texture input");

	assert(count == 0);
	assert(outputIndex == 0);

	const auto& textureEntry = GetGraph().GetTexture(*m_currentTextureIndex);
	return nzsl::ShaderBuilder::Identifier(textureEntry.name);
}

auto TextureValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	return Texture2Data::Type();
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

	const QImage& previewImage = textureEntry.preview;

	textureData->preview = PreviewValues(previewImage.width(), previewImage.height());
	for (std::size_t y = 0; y < textureData->preview.GetHeight(); ++y)
	{
		for (std::size_t x = 0; x < textureData->preview.GetWidth(); ++x)
		{
			QColor pixelColor = previewImage.pixelColor(int(x), int(y));

			textureData->preview(x, y) = nzsl::Vector4f32(pixelColor.redF(), pixelColor.greenF(), pixelColor.blueF(), pixelColor.alphaF());
		}
	}

	return textureData;
}

QString TextureValue::portCaption(QtNodes::PortType portType, QtNodes::PortIndex /*portIndex*/) const
{
	assert(portType == QtNodes::PortType::Out);

	if (!m_currentTextureIndex)
		return QString();

	const auto& textureEntry = GetGraph().GetTexture(*m_currentTextureIndex);
	return QString::fromStdString(textureEntry.name);
}

bool TextureValue::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex /*portIndex*/) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return false;
		case QtNodes::PortType::Out: return m_currentTextureIndex.has_value();

		default:
			break;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

QtNodes::NodeValidationState TextureValue::validationState() const
{
	if (!m_currentTextureIndex)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString TextureValue::validationMessage() const
{
	if (!m_currentTextureIndex)
		return "No texture selected";

	return QString();
}

void TextureValue::restore(const QJsonObject& data)
{
	m_currentTextureText = data["texture"].toString().toStdString();
	OnTextureListUpdate();

	ShaderNode::restore(data);
}

QJsonObject TextureValue::save() const
{
	QJsonObject data = ShaderNode::save();
	data["texture"] = QString::fromStdString(m_currentTextureText);

	return data;
}
