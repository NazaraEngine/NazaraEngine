#include <ShaderNode/DataModels/Mat4BinOp.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>

template<Nz::ShaderAst::BinaryType Op>
Mat4BinOp<Op>::Mat4BinOp(ShaderGraph& graph) :
ShaderNode(graph)
{
	UpdateOutput();
}

template<Nz::ShaderAst::BinaryType Op>
Nz::ShaderAst::NodePtr Mat4BinOp<Op>::BuildNode(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 2);
	assert(outputIndex == 0);

	return Nz::ShaderBuilder::Binary(Op, std::move(expressions[0]), std::move(expressions[1]));
}

template<Nz::ShaderAst::BinaryType Op>
QtNodes::NodeDataType Mat4BinOp<Op>::dataType(QtNodes::PortType /*portType*/, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0 || portIndex == 1);

	return Matrix4Data::Type();
}

template<Nz::ShaderAst::BinaryType Op>
unsigned int Mat4BinOp<Op>::nPorts(QtNodes::PortType portType) const
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

template<Nz::ShaderAst::BinaryType Op>
std::shared_ptr<QtNodes::NodeData> Mat4BinOp<Op>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

template<Nz::ShaderAst::BinaryType Op>
void Mat4BinOp<Op>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0 || index == 1);

	std::shared_ptr<Matrix4Data> castedValue;
	if (value && value->type().id == Matrix4Data::Type().id)
	{
		assert(dynamic_cast<Matrix4Data*>(value.get()) != nullptr);
		castedValue = std::static_pointer_cast<Matrix4Data>(value);
	}

	if (index == 0)
		m_lhs = std::move(castedValue);
	else
		m_rhs = std::move(castedValue);

	UpdateOutput();
}

template<Nz::ShaderAst::BinaryType Op>
QtNodes::NodeValidationState Mat4BinOp<Op>::validationState() const
{
	if (!m_lhs || !m_rhs)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

template<Nz::ShaderAst::BinaryType Op>
QString Mat4BinOp<Op>::validationMessage() const
{
	if (!m_lhs || !m_rhs)
		return "Missing operands";

	return QString();
}

template<Nz::ShaderAst::BinaryType Op>
bool Mat4BinOp<Op>::ComputePreview(QPixmap& pixmap)
{
	if (!m_lhs || !m_rhs)
		return false;

	return false;

	//pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	//return true;
}

template<Nz::ShaderAst::BinaryType Op>
void Mat4BinOp<Op>::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output = std::make_shared<Matrix4Data>();
		return;
	}

	m_output = std::make_shared<Matrix4Data>();

	/*m_output = std::make_shared<VecData>(m_lhs->componentCount);

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
	ApplyOp(leftResized.GetData(), rightResized.GetData(), m_output->preview.GetData(), maxWidth * maxHeight);*/

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}
