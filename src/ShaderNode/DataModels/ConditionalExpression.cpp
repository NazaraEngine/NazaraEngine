#include <ShaderNode/DataModels/ConditionalExpression.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/Matrix4Data.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <iostream>
#include <sstream>

ConditionalExpression::ConditionalExpression(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_onOptionListUpdateSlot.Connect(GetGraph().OnOptionListUpdate, [&](ShaderGraph*) { OnOptionListUpdate(); });
	m_onOptionUpdateSlot.Connect(GetGraph().OnOptionUpdate, [&](ShaderGraph*, std::size_t optionIndex)
	{
		if (m_currentOptionIndex == optionIndex)
		{
			UpdatePreview();
			Q_EMIT dataUpdated(0);
		}
	});

	if (graph.GetOptionCount() > 0)
	{
		m_currentOptionIndex = 0;
		UpdateOptionText();
	}

	EnablePreview();
	SetPreviewSize({ 128, 128 });
	UpdatePreview();
}

nzsl::Ast::NodePtr ConditionalExpression::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 2);
	assert(outputIndex == 0);

	if (!m_currentOptionIndex)
		throw std::runtime_error("no option");

	const ShaderGraph& graph = GetGraph();

	const auto& optionEntry = graph.GetOption(*m_currentOptionIndex);
	return nzsl::ShaderBuilder::ConditionalExpression(nzsl::ShaderBuilder::Identifier(optionEntry.name), std::move(expressions[0]), std::move(expressions[1]));
}

QString ConditionalExpression::caption() const
{
	return "ConditionalExpression (" + QString::fromStdString(m_currentOptionText) + ")";
}

QString ConditionalExpression::name() const
{
	return "ConditionalExpression";
}

unsigned int ConditionalExpression::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::None:
			break;

		case QtNodes::PortType::In:  return 2;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

void ConditionalExpression::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);
	
	QComboBox* optionSelection = new QComboBox;
	for (const auto& optionEntry : GetGraph().GetOptions())
		optionSelection->addItem(QString::fromStdString(optionEntry.name));

	if (m_currentOptionIndex)
		optionSelection->setCurrentIndex(int(*m_currentOptionIndex));
	else
		optionSelection->setCurrentIndex(-1);
	
	connect(optionSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentOptionIndex = static_cast<std::size_t>(index);
		else
			m_currentOptionIndex.reset();

		UpdateOptionText();
		UpdatePreview();

		Q_EMIT dataUpdated(0);
	});

	layout->addRow(tr("Option"), optionSelection);
}

auto ConditionalExpression::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			switch (portIndex)
			{
				case 0:
				{
					if (!m_truePath && !m_falsePath)
						return VecData::Type();

					return (m_truePath) ? m_truePath->type() : m_falsePath->type();
				}

				case 1:
				{
					if (!m_truePath && !m_falsePath)
						return VecData::Type();

					return (m_falsePath) ? m_falsePath->type() : m_truePath->type();
				}

				default:
					break;
			}
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);

			if (!m_truePath && !m_falsePath)
				return VecData::Type();

			return (m_truePath) ? m_truePath->type() : m_falsePath->type();
		}

		default:
			break;
	}

	return VecData::Type();
}

QString ConditionalExpression::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			switch (portIndex)
			{
				case 0:
					return "True expression";

				case 1:
					return "False expression";

				default:
					break;
			}
		}

		default:
			break;
	}

	return QString{};
}

bool ConditionalExpression::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex /*portIndex*/) const
{
	return portType == QtNodes::PortType::In;
}

std::shared_ptr<QtNodes::NodeData> ConditionalExpression::outData(QtNodes::PortIndex port)
{
	if (!m_currentOptionIndex)
		return nullptr;

	assert(port == 0);
	return (GetGraph().IsOptionEnabled(*m_currentOptionIndex)) ? m_truePath : m_falsePath;
}

void ConditionalExpression::restore(const QJsonObject& data)
{
	m_currentOptionText = data["option_name"].toString().toStdString();
	OnOptionListUpdate();

	ShaderNode::restore(data);
}

QJsonObject ConditionalExpression::save() const
{
	QJsonObject data = ShaderNode::save();
	data["option_name"] = QString::fromStdString(m_currentOptionText);

	return data;
}

void ConditionalExpression::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0 || index == 1);

	if (index == 0)
		m_truePath = std::move(value);
	else
		m_falsePath = std::move(value);

	UpdatePreview();
}

QtNodes::NodeValidationState ConditionalExpression::validationState() const
{
	if (!m_truePath || !m_falsePath)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString ConditionalExpression::validationMessage() const
{
	if (!m_currentOptionIndex)
		return "Invalid option";

	if (!m_truePath || !m_falsePath)
		return "Missing input";

	return QString();
}

bool ConditionalExpression::ComputePreview(QPixmap& pixmap)
{
	if (!m_currentOptionIndex)
		return false;

	auto input = outData(0);
	if (!input || input->type().id != VecData::Type().id)
		return false;

	assert(dynamic_cast<VecData*>(input.get()) != nullptr);
	const VecData& data = static_cast<const VecData&>(*input);

	pixmap = QPixmap::fromImage(data.preview.GenerateImage());
	return true;
}

void ConditionalExpression::OnOptionListUpdate()
{
	m_currentOptionIndex.reset();

	std::size_t optionIndex = 0;
	for (const auto& optionEntry : GetGraph().GetOptions())
	{
		if (optionEntry.name == m_currentOptionText)
		{
			m_currentOptionIndex = optionIndex;
			break;
		}

		optionIndex++;
	}
}

void ConditionalExpression::UpdateOptionText()
{
	if (m_currentOptionIndex)
	{
		auto& option = GetGraph().GetOption(*m_currentOptionIndex);
		m_currentOptionText = option.name;
	}
	else
		m_currentOptionText.clear();
}
