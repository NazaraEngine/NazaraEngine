#include <ShaderNode/DataModels/VecBinOp.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>

template<Nz::ShaderNodes::BinaryType BinOp>
VecBinOp<BinOp>::VecBinOp(ShaderGraph& graph) :
ShaderNode(graph)
{
	UpdateOutput();
}

template<Nz::ShaderNodes::BinaryType BinOp>
Nz::ShaderNodes::ExpressionPtr VecBinOp<BinOp>::GetExpression(Nz::ShaderNodes::ExpressionPtr* expressions, std::size_t count) const
{
	assert(count == 2);
	using BuilderType = typename Nz::ShaderBuilder::template BinOpBuilder<BinOp>;
	constexpr BuilderType builder;
	return builder(expressions[0], expressions[1]);
}

template<Nz::ShaderNodes::BinaryType BinOp>
QtNodes::NodeDataType VecBinOp<BinOp>::dataType(QtNodes::PortType /*portType*/, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0 || portIndex == 1);

	return VecData::Type();
}

template<Nz::ShaderNodes::BinaryType BinOp>
unsigned int VecBinOp<BinOp>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 2;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<Nz::ShaderNodes::BinaryType BinOp>
std::shared_ptr<QtNodes::NodeData> VecBinOp<BinOp>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

template<Nz::ShaderNodes::BinaryType BinOp>
void VecBinOp<BinOp>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0 || index == 1);

	std::shared_ptr<VecData> castedValue;
	if (value)
	{
		assert(dynamic_cast<VecData*>(value.get()) != nullptr);

		castedValue = std::static_pointer_cast<VecData>(value);
	}

	if (index == 0)
		m_lhs = std::move(castedValue);
	else
		m_rhs = std::move(castedValue);

	UpdateOutput();
}

template<Nz::ShaderNodes::BinaryType BinOp>
QtNodes::NodeValidationState VecBinOp<BinOp>::validationState() const
{
	if (!m_lhs || !m_rhs)
		return QtNodes::NodeValidationState::Error;

	if (m_lhs->componentCount != m_rhs->componentCount)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

template<Nz::ShaderNodes::BinaryType BinOp>
QString VecBinOp<BinOp>::validationMessage() const
{
	if (!m_lhs || !m_rhs)
		return "Missing operands";

	if (m_lhs->componentCount != m_rhs->componentCount)
		return "Incompatible components count (left has " + QString::number(m_lhs->componentCount) + ", right has " + QString::number(m_rhs->componentCount) + ")";

	return QString();
}

template<Nz::ShaderNodes::BinaryType BinOp>
bool VecBinOp<BinOp>::ComputePreview(QPixmap& pixmap)
{
	if (!m_lhs || !m_rhs)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

template<Nz::ShaderNodes::BinaryType BinOp>
void VecBinOp<BinOp>::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output = std::make_shared<VecData>(4);
		m_output->preview = PreviewValues(1, 1);
		m_output->preview.Fill(Nz::Vector4f::Zero());
		return;
	}

	m_output = std::make_shared<VecData>(m_lhs->componentCount);

	const PreviewValues& leftPreview = m_lhs->preview;
	const PreviewValues& rightPreview = m_rhs->preview;
	std::size_t maxWidth = std::max(leftPreview.GetWidth(), rightPreview.GetWidth());
	std::size_t maxHeight = std::max(leftPreview.GetHeight(), rightPreview.GetHeight());

	// FIXME: Prevent useless copy
	PreviewValues leftResized = leftPreview;
	if (leftResized.GetWidth() != maxWidth || leftResized.GetHeight() != maxHeight)
		leftResized = leftResized.Resized(maxWidth, maxHeight);

	PreviewValues rightResized = rightPreview;
	if (rightResized.GetWidth() != maxWidth || rightResized.GetHeight() != maxHeight)
		rightResized = rightResized.Resized(maxWidth, maxHeight);

	m_output->preview = PreviewValues(maxWidth, maxHeight);
	ApplyOp(leftResized.GetData(), rightResized.GetData(), m_output->preview.GetData(), maxWidth * maxHeight);

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}
