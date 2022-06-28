#include <ShaderNode/DataModels/BinOp.hpp>
#include <NZSL/ShaderBuilder.hpp>

template<typename DataType, nzsl::Ast::BinaryType Op>
BinOp<DataType, Op>::BinOp(ShaderGraph& graph) :
ShaderNode(graph)
{
	UpdateOutput();
}

template<typename DataType, nzsl::Ast::BinaryType Op>
nzsl::Ast::NodePtr BinOp<DataType, Op>::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 2);
	assert(outputIndex == 0);

	return nzsl::ShaderBuilder::Binary(Op, std::move(expressions[0]), std::move(expressions[1]));
}

template<typename DataType, nzsl::Ast::BinaryType Op>
QtNodes::NodeDataType BinOp<DataType, Op>::dataType(QtNodes::PortType /*portType*/, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0 || portIndex == 1);

	return DataType::Type();
}

template<typename DataType, nzsl::Ast::BinaryType Op>
unsigned int BinOp<DataType, Op>::nPorts(QtNodes::PortType portType) const
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

template<typename DataType, nzsl::Ast::BinaryType Op>
std::shared_ptr<QtNodes::NodeData> BinOp<DataType, Op>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

template<typename DataType, nzsl::Ast::BinaryType Op>
QString BinOp<DataType, Op>::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
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

template<typename DataType, nzsl::Ast::BinaryType Op>
bool BinOp<DataType, Op>::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0 || portIndex == 1);
	return portType == QtNodes::PortType::In || portType == QtNodes::PortType::Out;
}

template<typename DataType, nzsl::Ast::BinaryType Op>
void BinOp<DataType, Op>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
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

template<typename DataType, nzsl::Ast::BinaryType Op>
QtNodes::NodeValidationState BinOp<DataType, Op>::validationState() const
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

template<typename DataType, nzsl::Ast::BinaryType Op>
QString BinOp<DataType, Op>::validationMessage() const
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

template<typename DataType, nzsl::Ast::BinaryType Op>
bool BinOp<DataType, Op>::ComputePreview(QPixmap& pixmap)
{
	if (!m_lhs || !m_rhs)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

template<typename DataType, nzsl::Ast::BinaryType Op>
void BinOp<DataType, Op>::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		if constexpr (std::is_same_v<DataType, VecData>)
			m_output = std::make_shared<DataType>(4);
		else
			m_output = std::make_shared<DataType>();

		m_output->preview = PreviewValues(1, 1);
		m_output->preview.Fill(nzsl::Vector4f32(0.f, 0.f, 0.f, 0.f));
		return;
	}

	if constexpr (std::is_same_v<DataType, VecData>)
		m_output = std::make_shared<DataType>(m_lhs->componentCount);
	else
		m_output = std::make_shared<DataType>();

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
void BinAdd<DataType>::ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
		output[i] = left[i] + right[i];
}

template<typename DataType>
QString BinAdd<DataType>::GetOperationString() const
{
	return "+";
}

template<typename DataType>
void BinMul<DataType>::ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
		output[i] = left[i] * right[i];
}

template<typename DataType>
QString BinMul<DataType>::GetOperationString() const
{
	return "*";
}

template<typename DataType>
void BinSub<DataType>::ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
		output[i] = left[i] - right[i];
}

template<typename DataType>
QString BinSub<DataType>::GetOperationString() const
{
	return "-";
}

template<typename DataType>
void BinDiv<DataType>::ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
		output[i] = left[i] / right[i];
}

template<typename DataType>
QString BinDiv<DataType>::GetOperationString() const
{
	return "/";
}
