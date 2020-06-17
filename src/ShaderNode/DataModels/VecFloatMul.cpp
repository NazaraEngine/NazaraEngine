#include <ShaderNode/DataModels/VecFloatMul.hpp>
#include <Nazara/Renderer/ShaderNodes.hpp>

VecFloatMul::VecFloatMul(ShaderGraph& graph) :
ShaderNode(graph)
{
	UpdateOutput();
}

Nz::ShaderNodes::ExpressionPtr VecFloatMul::GetExpression(Nz::ShaderNodes::ExpressionPtr* expressions, std::size_t count) const
{
	assert(count == 2);
	using namespace Nz::ShaderNodes;
	return BinaryOp::Build(BinaryType::Multiply, expressions[0], expressions[1]);
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
			if (value)
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
			if (value)
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

	pixmap = QPixmap::fromImage(m_output->preview);
	return true;
}

void VecFloatMul::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output = std::make_shared<VecData>(4);
		m_output->preview = QImage(1, 1, QImage::Format_RGBA8888);
		m_output->preview.fill(QColor::fromRgb(0, 0, 0, 0));
		return;
	}

	m_output = std::make_shared<VecData>(m_rhs->componentCount);

	const QImage& leftPreview = m_lhs->preview;
	const QImage& rightPreview = m_rhs->preview;
	int maxWidth = std::max(leftPreview.width(), rightPreview.width());
	int maxHeight = std::max(leftPreview.height(), rightPreview.height());

	// Exploit COW
	QImage leftResized = leftPreview;
	if (leftResized.width() != maxWidth || leftResized.height() != maxHeight)
		leftResized = leftResized.scaled(maxWidth, maxHeight);

	QImage rightResized = rightPreview;
	if (rightResized.width() != maxWidth || rightResized.height() != maxHeight)
		rightResized = rightResized.scaled(maxWidth, maxHeight);

	m_output->preview = QImage(maxWidth, maxHeight, QImage::Format_RGBA8888);

	const uchar* left = leftResized.constBits();
	const uchar* right = rightPreview.constBits();
	uchar* output = m_output->preview.bits();

	std::size_t pixelCount = maxWidth * maxHeight * 4;
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		output[i] = static_cast<std::uint8_t>(lValue * rValue / 255);
	}

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}
