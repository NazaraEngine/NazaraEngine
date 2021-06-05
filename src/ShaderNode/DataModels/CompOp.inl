#include <ShaderNode/DataModels/CompOp.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>

template<typename DataType, Nz::ShaderAst::BinaryType Op>
CompOp<DataType, Op>::CompOp(ShaderGraph& graph) :
ShaderNode(graph)
{
	UpdateOutput();
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
Nz::ShaderAst::NodePtr CompOp<DataType, Op>::BuildNode(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 2);
	assert(outputIndex == 0);

	return Nz::ShaderBuilder::Binary(Op, std::move(expressions[0]), std::move(expressions[1]));
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
QtNodes::NodeDataType CompOp<DataType, Op>::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{	
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			assert(portIndex == 0 || portIndex == 1);
			return DataType::Type();
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return BoolData::Type();
		}

		default: break;
	}

	assert(false);
	throw std::runtime_error("invalid port type");
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
unsigned int CompOp<DataType, Op>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 2;
		case QtNodes::PortType::Out: return 1;
		default: break;
	}

	assert(false);
	throw std::runtime_error("invalid port type");
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
std::shared_ptr<QtNodes::NodeData> CompOp<DataType, Op>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
QString CompOp<DataType, Op>::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			switch (portIndex)
			{
				case 0:
					return "A";

				case 1:
					return "B";

				default:
					break;
			}
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return "A " + GetOperationString() + " B";
		}

		default:
			break;
	}

	return QString{};
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
bool CompOp<DataType, Op>::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0 || portIndex == 1);
	return portType == QtNodes::PortType::In || portType == QtNodes::PortType::Out;
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
void CompOp<DataType, Op>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0 || index == 1);

	std::shared_ptr<DataType> castedValue;
	if (value && value->type().id == DataType::Type().id)
		castedValue = std::static_pointer_cast<DataType>(value);

	if (index == 0)
		m_lhs = std::move(castedValue);
	else
		m_rhs = std::move(castedValue);

	UpdateOutput();
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
QtNodes::NodeValidationState CompOp<DataType, Op>::validationState() const
{
	if (!m_lhs || !m_rhs)
		return QtNodes::NodeValidationState::Error;

	if constexpr (std::is_same_v<DataType, VecData>)
	{
		if (m_lhs->componentCount != m_rhs->componentCount)
			return QtNodes::NodeValidationState::Error;
	}

	return QtNodes::NodeValidationState::Valid;
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
QString CompOp<DataType, Op>::validationMessage() const
{
	if (!m_lhs || !m_rhs)
		return "Missing operands";

	if constexpr (std::is_same_v<DataType, VecData>)
	{
		if (m_lhs->componentCount != m_rhs->componentCount)
			return "Incompatible components count (left has " + QString::number(m_lhs->componentCount) + ", right has " + QString::number(m_rhs->componentCount) + ")";
	}

	return QString();
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
bool CompOp<DataType, Op>::ComputePreview(QPixmap& pixmap)
{
	if (!m_lhs || !m_rhs)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

template<typename DataType, Nz::ShaderAst::BinaryType Op>
void CompOp<DataType, Op>::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output = std::make_shared<BoolData>();
		m_output->preview = PreviewValues(1, 1);
		m_output->preview.Fill(Nz::Vector4f::Zero());
		return;
	}

	m_output = std::make_shared<BoolData>();

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

template<typename DataType>
void CompEq<DataType>::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		float r = (left[i] == right[i]) ? 1.f : 0.f;
		output[i] = Nz::Vector4f(r, r, r, r);
	}
}

template<typename DataType>
QString CompEq<DataType>::GetOperationString() const
{
	return "==";
}

template<typename DataType>
void CompGe<DataType>::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		float r = (left[i] >= right[i]) ? 1.f : 0.f;
		output[i] = Nz::Vector4f(r, r, r, r);
	}
}

template<typename DataType>
QString CompGe<DataType>::GetOperationString() const
{
	return ">=";
}

template<typename DataType>
void CompGt<DataType>::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		float r = (left[i] > right[i]) ? 1.f : 0.f;
		output[i] = Nz::Vector4f(r, r, r, r);
	}
}

template<typename DataType>
QString CompGt<DataType>::GetOperationString() const
{
	return ">";
}

template<typename DataType>
void CompLe<DataType>::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		float r = (left[i] >= right[i]) ? 1.f : 0.f;
		output[i] = Nz::Vector4f(r, r, r, r);
	}
}

template<typename DataType>
QString CompLe<DataType>::GetOperationString() const
{
	return "<=";
}

template<typename DataType>
void CompLt<DataType>::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		float r = (left[i] > right[i]) ? 1.f : 0.f;
		output[i] = Nz::Vector4f(r, r, r, r);
	}
}

template<typename DataType>
QString CompLt<DataType>::GetOperationString() const
{
	return "<";
}

template<typename DataType>
void CompNe<DataType>::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		float r = (left[i] != right[i]) ? 1.f : 0.f;
		output[i] = Nz::Vector4f(r, r, r, r);
	}
}

template<typename DataType>
QString CompNe<DataType>::GetOperationString() const
{
	return "!=";
}
