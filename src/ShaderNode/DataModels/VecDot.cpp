#include <ShaderNode/DataModels/VecDot.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <NZSL/Ast/Nodes.hpp>

VecDot::VecDot(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_output = std::make_shared<FloatData>();
	UpdateOutput();
}

nzsl::Ast::NodePtr VecDot::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 2);
	assert(outputIndex == 0);

	std::vector<nzsl::Ast::ExpressionPtr> params;
	params.push_back(std::move(expressions[0]));
	params.push_back(std::move(expressions[1]));

	return nzsl::ShaderBuilder::Intrinsic(nzsl::Ast::IntrinsicType::DotProduct, std::move(params));
}

QString VecDot::caption() const
{
	static QString caption = "Vector dot";
	return caption;
}

QString VecDot::name() const
{
	static QString name = "vec_dot";
	return name;
}

QtNodes::NodeDataType VecDot::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::None:
			break;

		case QtNodes::PortType::In:
		{
			assert(portIndex == 0 || portIndex == 1);
			return VecData::Type();
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return FloatData::Type();
		}
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

unsigned int VecDot::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::None:
			break;

		case QtNodes::PortType::In:  return 2;
		case QtNodes::PortType::Out: return 1;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

std::shared_ptr<QtNodes::NodeData> VecDot::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

void VecDot::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0 || index == 1);

	std::shared_ptr<VecData> castedValue;
	if (value && value->type().id == VecData::Type().id)
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

QtNodes::NodeValidationState VecDot::validationState() const
{
	if (!m_lhs || !m_rhs)
		return QtNodes::NodeValidationState::Error;

	if (m_lhs->componentCount != m_rhs->componentCount)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString VecDot::validationMessage() const
{
	if (!m_lhs || !m_rhs)
		return "Missing operands";

	if (m_lhs->componentCount != m_rhs->componentCount)
		return "Incompatible components count (left has " + QString::number(m_lhs->componentCount) + ", right has " + QString::number(m_rhs->componentCount) + ")";

	return QString();
}
bool VecDot::ComputePreview(QPixmap& pixmap)
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

void VecDot::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output->preview = PreviewValues(1, 1);
		m_output->preview.Fill(nzsl::Vector4f32(0.f, 0.f, 0.f, 0.f));
		return;
	}

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

	const nzsl::Vector4f32* left = leftResized.GetData();
	const nzsl::Vector4f32* right = rightPreview.GetData();
	nzsl::Vector4f32* output = m_output->preview.GetData();

	std::size_t pixelCount = maxWidth * maxHeight;
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		float acc = 0.f;
		for (std::size_t j = 0; j < m_lhs->componentCount; ++j)
			acc += left[i][j] * right[i][j];

		for (std::size_t j = 0; j < 3; ++j)
			output[i][j] = acc;
		output[i][3] = 1.f; //< leave alpha at maximum
	}

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}
