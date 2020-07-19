#include <ShaderNode/DataModels/BufferField.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <QtWidgets/QFormLayout>
#include <iostream>

BufferField::BufferField(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_onBufferListUpdateSlot.Connect(GetGraph().OnBufferListUpdate, [&](ShaderGraph*) { UpdateCurrentBufferIndex(); });
	m_onBufferUpdateSlot.Connect(GetGraph().OnBufferUpdate, [&](ShaderGraph*, std::size_t bufferIndex)
	{
		if (m_currentBufferIndex == bufferIndex)
		{
			UpdatePreview();
			Q_EMIT dataUpdated(0);
		}
	});

	if (graph.GetBufferCount() > 0)
	{
		m_currentBufferIndex = 0;
		UpdateBufferText();
	}

	DisableCustomVariableName();
	UpdatePreview();
}

unsigned int BufferField::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 0;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

bool BufferField::ComputePreview(QPixmap& pixmap)
{
	return false;

	/*if (!m_currentBufferIndex)
		return false;

	const ShaderGraph& graph = GetGraph();
	const auto& inputEntry = graph.GetBuffer(*m_currentBufferIndex);
	const auto& preview = graph.GetPreviewModel();

	pixmap = QPixmap::fromImage(preview.GetPreview(inputEntry.role, inputEntry.roleIndex).GenerateImage());
	return true;*/
}

void BufferField::UpdateCurrentBufferIndex()
{
	Nz::CallOnExit resetIfNotFound([&]
	{
		m_currentBufferIndex.reset();
		m_currentBufferText.clear();
		m_currentFieldIndex.reset();
		m_currentFieldText.clear();
	});

	if (m_currentBufferText.empty())
		return;

	std::size_t bufferIndex = 0;
	for (const auto& bufferEntry : GetGraph().GetBuffers())
	{
		if (bufferEntry.name == m_currentBufferText)
		{
			m_currentBufferIndex = bufferIndex;
			resetIfNotFound.Reset();
			break;
		}

		bufferIndex++;
	}
}

void BufferField::UpdateCurrentFieldIndex()
{
	Nz::CallOnExit resetIfNotFound([&]
	{
		m_currentFieldIndex.reset();
		m_currentFieldText.clear();
	});

	if (m_currentFieldText.empty())
		return;

	if (!m_currentFieldIndex)
		m_currentFieldIndex.emplace();

	CurrentField& currentField = *m_currentFieldIndex;
	currentField.nestedFields.clear();

	const ShaderGraph& graph = GetGraph();
	auto& buffer = graph.GetBuffer(*m_currentBufferIndex);

	std::function<bool(std::size_t structIndex, const std::string& prefix)> FetchField;
	FetchField = [&](std::size_t structIndex, const std::string& prefix) -> bool
	{
		const auto& s = graph.GetStruct(structIndex);
		for (auto it = s.members.begin(); it != s.members.end(); ++it)
		{
			const auto& member = *it;

			bool found = std::visit([&](auto&& arg) -> bool
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, PrimitiveType>)
				{
					if (prefix + member.name == m_currentFieldText)
					{
						currentField.finalFieldIndex = std::distance(s.members.begin(), it);
						return true;
					}
					else
						return false;
				}
				else if constexpr (std::is_same_v<T, std::size_t>)
				{
					currentField.nestedFields.push_back(std::distance(s.members.begin(), it));
					bool found = FetchField(arg, prefix + member.name + ".");
					if (!found)
					{
						currentField.nestedFields.pop_back();
						return false;
					}

					return true;
				}
				else
					static_assert(Nz::AlwaysFalse<T>::value, "non-exhaustive visitor");
			},
			member.type);

			if (found)
				return true;
		}

		return false;
	};

	if (FetchField(buffer.structIndex, ""))
		resetIfNotFound.Reset();
}

void BufferField::PopulateField(QComboBox* fieldList, std::size_t structIndex, const std::string& prefix)
{
	const auto& s = GetGraph().GetStruct(structIndex);
	for (const auto& member : s.members)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, PrimitiveType>)
				fieldList->addItem(QString::fromStdString(prefix + member.name));
			else if constexpr (std::is_same_v<T, std::size_t>)
				PopulateField(fieldList, arg, prefix + member.name + ".");
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		},
		member.type);
	}
}

const ShaderGraph::StructMemberEntry& BufferField::RetrieveNestedMember() const
{
	const ShaderGraph& graph = GetGraph();
	auto& buffer = graph.GetBuffer(*m_currentBufferIndex);

	assert(m_currentFieldIndex);
	const CurrentField& currentField = *m_currentFieldIndex;

	const ShaderGraph::StructEntry* structEntry = &graph.GetStruct(buffer.structIndex);
	for (std::size_t nestedIndex : currentField.nestedFields)
	{
		assert(nestedIndex < structEntry->members.size());
		const auto& memberEntry = structEntry->members[nestedIndex];
		assert(std::holds_alternative<std::size_t>(memberEntry.type));

		std::size_t nestedStructIndex = std::get<std::size_t>(memberEntry.type);
		structEntry = &graph.GetStruct(nestedStructIndex);
	}

	return structEntry->members[currentField.finalFieldIndex];
}

void BufferField::UpdateBufferText()
{
	if (m_currentBufferIndex)
	{
		auto& buffer = GetGraph().GetBuffer(*m_currentBufferIndex);
		m_currentBufferText = buffer.name;
	}
	else
		m_currentBufferText.clear();
}

void BufferField::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QComboBox* fieldSelection = new QComboBox;
	connect(fieldSelection, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index)
	{
		if (index >= 0)
			m_currentFieldText = fieldSelection->itemText(index).toStdString();
		else
			m_currentFieldText.clear();

		UpdateCurrentFieldIndex();
		UpdatePreview();

		Q_EMIT dataUpdated(0);
	});

	QComboBox* bufferSelection = new QComboBox;
	for (const auto& inputEntry : GetGraph().GetBuffers())
		bufferSelection->addItem(QString::fromStdString(inputEntry.name));

	connect(bufferSelection, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index)
	{
		fieldSelection->clear();
		fieldSelection->setCurrentIndex(-1);

		if (index >= 0)
		{
			m_currentBufferIndex = static_cast<std::size_t>(index);

			const ShaderGraph& graph = GetGraph();
			const auto& buffer = graph.GetBuffer(*m_currentBufferIndex);
			PopulateField(fieldSelection, buffer.structIndex);
		}
		else
			m_currentBufferIndex.reset();

		UpdateBufferText();
	});

	if (m_currentBufferIndex)
	{
		int index = int(*m_currentBufferIndex);
		QString currentFieldText = QString::fromStdString(m_currentFieldText);

		bufferSelection->setCurrentIndex(-1);
		bufferSelection->setCurrentIndex(index);

		fieldSelection->setCurrentText(currentFieldText);
	}
	else
	{
		bufferSelection->setCurrentIndex(-1);
		fieldSelection->setCurrentIndex(-1);
	}

	layout->addRow(tr("Buffer"), bufferSelection);
	layout->addRow(tr("Field"), fieldSelection);
}

Nz::ShaderNodes::ExpressionPtr BufferField::GetExpression(Nz::ShaderNodes::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	assert(count == 0);

	if (!m_currentBufferIndex)
		throw std::runtime_error("no buffer");

	const ShaderGraph& graph = GetGraph();

	const auto& bufferEntry = graph.GetBuffer(*m_currentBufferIndex);
	const auto& structEntry = graph.GetStruct(bufferEntry.structIndex);

	Nz::ShaderNodes::VariablePtr varPtr;
	switch (bufferEntry.type)
	{
		case BufferType::UniformBufferObject:
			varPtr = Nz::ShaderBuilder::Uniform(bufferEntry.name, structEntry.name);
			break;
	}

	assert(varPtr);

	assert(m_currentFieldIndex);
	const CurrentField& currentField = *m_currentFieldIndex;

	Nz::ShaderNodes::ExpressionPtr sourceExpr = Nz::ShaderBuilder::Identifier(varPtr);

	const ShaderGraph::StructEntry* sourceStruct = &structEntry;
	for (std::size_t nestedIndex : currentField.nestedFields)
	{
		assert(nestedIndex < sourceStruct->members.size());
		const auto& memberEntry = sourceStruct->members[nestedIndex];
		assert(std::holds_alternative<std::size_t>(memberEntry.type));

		std::size_t nestedStructIndex = std::get<std::size_t>(memberEntry.type);
		sourceStruct = &graph.GetStruct(nestedStructIndex);

		sourceExpr = Nz::ShaderBuilder::AccessMember(std::move(sourceExpr), 0, graph.ToShaderExpressionType(memberEntry.type));
	}

	assert(currentField.finalFieldIndex < sourceStruct->members.size());
	const auto& memberEntry = sourceStruct->members[currentField.finalFieldIndex];
	assert(std::holds_alternative<PrimitiveType>(memberEntry.type));

	return Nz::ShaderBuilder::AccessMember(std::move(sourceExpr), 0, graph.ToShaderExpressionType(std::get<PrimitiveType>(memberEntry.type)));
}

auto BufferField::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	if (!m_currentBufferIndex || !m_currentFieldIndex)
		return VecData::Type();

	const auto& member = RetrieveNestedMember();
	assert(std::holds_alternative<PrimitiveType>(member.type));

	switch (std::get<PrimitiveType>(member.type))
	{
		case PrimitiveType::Bool:
			return BoolData::Type();

		case PrimitiveType::Float1:
			return FloatData::Type();

		case PrimitiveType::Float2:
		case PrimitiveType::Float3:
		case PrimitiveType::Float4:
			return VecData::Type();
	}

	assert(false);
	throw std::runtime_error("Unhandled primitive type");
}

std::shared_ptr<QtNodes::NodeData> BufferField::outData(QtNodes::PortIndex port)
{
	if (!m_currentBufferIndex)
		return nullptr;

	assert(port == 0);

	if (!m_currentBufferIndex || !m_currentFieldIndex)
		return {};
	
	const auto& member = RetrieveNestedMember();
	assert(std::holds_alternative<PrimitiveType>(member.type));

	switch (std::get<PrimitiveType>(member.type))
	{
		case PrimitiveType::Bool:   return std::make_shared<BoolData>();
		case PrimitiveType::Float1: return std::make_shared<FloatData>();
		case PrimitiveType::Float2: return std::make_shared<VecData>(2);
		case PrimitiveType::Float3: return std::make_shared<VecData>(3);
		case PrimitiveType::Float4: return std::make_shared<VecData>(4);
	}

	assert(false);
	throw std::runtime_error("Unhandled primitive type");
}

QtNodes::NodeValidationState BufferField::validationState() const
{
	if (!m_currentBufferIndex)
		return QtNodes::NodeValidationState::Error;

	if (!m_currentFieldIndex)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString BufferField::validationMessage() const
{
	if (!m_currentBufferIndex)
		return "No input selected";

	if (!m_currentFieldIndex)
		return "No field selected";

	return QString();
}

void BufferField::restore(const QJsonObject& data)
{
	m_currentBufferText = data["buffer"].toString().toStdString();
	m_currentFieldText = data["field"].toString().toStdString();
	UpdateCurrentBufferIndex();

	ShaderNode::restore(data);
}

QJsonObject BufferField::save() const
{
	QJsonObject data = ShaderNode::save();
	data["buffer"] = QString::fromStdString(m_currentBufferText);
	data["field"] = QString::fromStdString(m_currentFieldText);

	return data;
}
