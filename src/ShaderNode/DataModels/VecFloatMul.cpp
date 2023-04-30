#include <ShaderNode/DataModels/VecFloatMul.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <NZSL/Ast/Nodes.hpp>

VecFloatMul::VecFloatMul(ShaderGraph& graph) :
ShaderNode(graph)
{
	UpdateOutput();
}

nzsl::Ast::NodePtr VecFloatMul::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 2);
	assert(outputIndex == 0);

	return nzsl::ShaderBuilder::Binary(nzsl::Ast::BinaryType::Multiply, std::move(expressions[0]), std::move(expressions[1]));
}

QString VecFloatMul::caption() const
{
	static QString caption = "Float/vector multiplication";
	return caption;
}

QString VecFloatMul::name() const
{
	static QString name = "vecfloat_mul";
	return name;
}

QtNodes::NodeDataType VecFloatMul::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::None:
			break;

		case QtNodes::PortType::In:
		{
			assert(portIndex == 0 || portIndex == 1);
			switch (portIndex)
			{
				case 0: return FloatData::Type();
				case 1: return VecData::Type();
			}
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return VecData::Type();
		}
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

unsigned int VecFloatMul::nPorts(QtNodes::PortType portType) const
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

std::shared_ptr<QtNodes::NodeData> VecFloatMul::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

void VecFloatMul::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0 || index == 1);

	switch (index)
	{
		case 0:
		{
			if (value && value->type().id == FloatData::Type().id)
			{
				assert(dynamic_cast<FloatData*>(value.get()) != nullptr);
				m_lhs = std::static_pointer_cast<FloatData>(value);
			}
			else
				m_lhs.reset();

			break;
		}

		case 1:
		{
			if (value && value->type().id == VecData::Type().id)
			{
				assert(dynamic_cast<VecData*>(value.get()) != nullptr);
				m_rhs = std::static_pointer_cast<VecData>(value);
			}
			else
				m_rhs.reset();

			break;
		}

		default:
			assert(false);
			throw std::runtime_error("Invalid PortType");
	}

	UpdateOutput();
}

QtNodes::NodeValidationState VecFloatMul::validationState() const
{
	if (!m_lhs || !m_rhs)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString VecFloatMul::validationMessage() const
{
	if (!m_lhs || !m_rhs)
		return "Missing operands";

	return QString();
}

bool VecFloatMul::ComputePreview(QPixmap& pixmap)
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

void VecFloatMul::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output = std::make_shared<VecData>(4);
		m_output->preview = PreviewValues(1, 1);
		m_output->preview.Fill(nzsl::Vector4f32(0.f, 0.f, 0.f, 0.f));
		return;
	}

	m_output = std::make_shared<VecData>(m_rhs->componentCount);

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
		output[i] = left[i] * right[i];

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}
