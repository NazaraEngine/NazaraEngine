#include <ShaderNode/DataModels/ConditionalExpression.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/Matrix4Data.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <iostream>
#include <sstream>

ConditionalExpression::ConditionalExpression(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_onConditionListUpdateSlot.Connect(GetGraph().OnConditionListUpdate, [&](ShaderGraph*) { OnConditionListUpdate(); });
	m_onConditionUpdateSlot.Connect(GetGraph().OnConditionUpdate, [&](ShaderGraph*, std::size_t conditionIndex)
	{
		if (m_currentConditionIndex == conditionIndex)
		{
			UpdatePreview();
			Q_EMIT dataUpdated(0);
		}
	});

	if (graph.GetConditionCount() > 0)
	{
		m_currentConditionIndex = 0;
		UpdateConditionText();
	}

	EnablePreview();
	SetPreviewSize({ 128, 128 });
	UpdatePreview();
}

Nz::ShaderNodes::NodePtr ConditionalExpression::BuildNode(Nz::ShaderNodes::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 2);
	assert(outputIndex == 0);

	if (!m_currentConditionIndex)
		throw std::runtime_error("no condition");

	const ShaderGraph& graph = GetGraph();

	const auto& conditionEntry = graph.GetCondition(*m_currentConditionIndex);
	return Nz::ShaderBuilder::ConditionalExpression(conditionEntry.name, expressions[0], expressions[1]);
}

QString ConditionalExpression::caption() const
{
	return "ConditionalExpression (" + QString::fromStdString(m_currentConditionText) + ")";
}

QString ConditionalExpression::name() const
{
	return "ConditionalExpression";
}

unsigned int ConditionalExpression::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 2;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

void ConditionalExpression::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);
	
	QComboBox* conditionSelection = new QComboBox;
	for (const auto& conditionEntry : GetGraph().GetConditions())
		conditionSelection->addItem(QString::fromStdString(conditionEntry.name));

	if (m_currentConditionIndex)
		conditionSelection->setCurrentIndex(int(*m_currentConditionIndex));
	else
		conditionSelection->setCurrentIndex(-1);
	
	connect(conditionSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentConditionIndex = static_cast<std::size_t>(index);
		else
			m_currentConditionIndex.reset();

		UpdateConditionText();
		UpdatePreview();

		Q_EMIT dataUpdated(0);
	});

	layout->addRow(tr("Condition"), conditionSelection);
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
	if (!m_currentConditionIndex)
		return nullptr;

	assert(port == 0);
	return (GetGraph().IsConditionEnabled(*m_currentConditionIndex)) ? m_truePath : m_falsePath;
}

void ConditionalExpression::restore(const QJsonObject& data)
{
	m_currentConditionText = data["condition_name"].toString().toStdString();
	OnConditionListUpdate();

	ShaderNode::restore(data);
}

QJsonObject ConditionalExpression::save() const
{
	QJsonObject data = ShaderNode::save();
	data["condition_name"] = QString::fromStdString(m_currentConditionText);

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
	if (!m_currentConditionIndex)
		return "Invalid condition";

	if (!m_truePath || !m_falsePath)
		return "Missing input";

	return QString();
}

bool ConditionalExpression::ComputePreview(QPixmap& pixmap)
{
	if (!m_currentConditionIndex)
		return false;

	auto input = outData(0);
	if (!input || input->type().id != VecData::Type().id)
		return false;

	assert(dynamic_cast<VecData*>(input.get()) != nullptr);
	const VecData& data = static_cast<const VecData&>(*input);

	pixmap = QPixmap::fromImage(data.preview.GenerateImage());
	return true;
}

void ConditionalExpression::OnConditionListUpdate()
{
	m_currentConditionIndex.reset();

	std::size_t conditionIndex = 0;
	for (const auto& conditionEntry : GetGraph().GetConditions())
	{
		if (conditionEntry.name == m_currentConditionText)
		{
			m_currentConditionIndex = conditionIndex;
			break;
		}

		conditionIndex++;
	}
}

void ConditionalExpression::UpdateConditionText()
{
	if (m_currentConditionIndex)
	{
		auto& condition = GetGraph().GetCondition(*m_currentConditionIndex);
		m_currentConditionText = condition.name;
	}
	else
		m_currentConditionText.clear();
}
