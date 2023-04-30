#include <ShaderNode/DataModels/Mat4VecMul.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <NZSL/Ast/Nodes.hpp>

Mat4VecMul::Mat4VecMul(ShaderGraph& graph) :
ShaderNode(graph)
{
	UpdateOutput();
}

nzsl::Ast::NodePtr Mat4VecMul::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 2);
	assert(outputIndex == 0);

	return nzsl::ShaderBuilder::Binary(nzsl::Ast::BinaryType::Multiply, std::move(expressions[0]), std::move(expressions[1]));
}

QString Mat4VecMul::caption() const
{
	static QString caption = "Mat4/Vec multiplication";
	return caption;
}

QString Mat4VecMul::name() const
{
	static QString name = "mat4vec_mul";
	return name;
}

QtNodes::NodeDataType Mat4VecMul::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
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
				case 0: return Matrix4Data::Type();
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

unsigned int Mat4VecMul::nPorts(QtNodes::PortType portType) const
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

std::shared_ptr<QtNodes::NodeData> Mat4VecMul::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

void Mat4VecMul::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0 || index == 1);

	switch (index)
	{
		case 0:
		{
			if (value && value->type().id == Matrix4Data::Type().id)
			{
				assert(dynamic_cast<Matrix4Data*>(value.get()) != nullptr);
				m_lhs = std::static_pointer_cast<Matrix4Data>(value);
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

QtNodes::NodeValidationState Mat4VecMul::validationState() const
{
	if (!m_lhs || !m_rhs)
		return QtNodes::NodeValidationState::Error;

	if (m_rhs->componentCount != 4)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString Mat4VecMul::validationMessage() const
{
	if (!m_lhs || !m_rhs)
		return "Missing operands";

	if (m_rhs->componentCount != 4)
		return QString("Expected vector with 4 components, got ") + QString::number(m_rhs->componentCount);

	return QString();
}

bool Mat4VecMul::ComputePreview(QPixmap& pixmap)
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

void Mat4VecMul::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output = std::make_shared<VecData>(4);
		m_output->preview = PreviewValues(1, 1);
		m_output->preview.Fill(nzsl::Vector4f32(0.f, 0.f, 0.f, 0.f));
		return;
	}

	m_output = std::make_shared<VecData>(4);
	m_output->preview = PreviewValues(1, 1);
	m_output->preview.Fill(nzsl::Vector4f32(0.f, 0.f, 0.f, 0.f));

	/*m_output = std::make_shared<VecData>(m_rhs->componentCount);

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

	UpdatePreview();*/
}
