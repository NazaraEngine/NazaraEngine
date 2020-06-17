#include <ShaderNode/DataModels/VecDot.hpp>
#include <Nazara/Renderer/ShaderNodes.hpp>

VecDot::VecDot(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_output = std::make_shared<FloatData>();
	UpdateOutput();
}

Nz::ShaderNodes::ExpressionPtr VecDot::GetExpression(Nz::ShaderNodes::ExpressionPtr* expressions, std::size_t count) const
{
	assert(count == 2);
	using namespace Nz::ShaderNodes;
	return IntrinsicCall::Build(IntrinsicType::DotProduct, { expressions[0], expressions[1] });
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

	pixmap = QPixmap::fromImage(m_output->preview);
	return true;
}

void VecDot::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output->preview = QImage(1, 1, QImage::Format_RGBA8888);
		m_output->preview.fill(QColor::fromRgb(0, 0, 0, 0));
		return;
	}

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

	std::size_t pixelCount = maxWidth * maxHeight;
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int acc = 0;
		for (std::size_t j = 0; j < m_lhs->componentCount; ++j)
			acc += left[j] * right[j] / 255;

		unsigned int result = static_cast<std::uint8_t>(std::min(acc, 255U));
		for (std::size_t j = 0; j < 3; ++j)
			*output++ = result;
		*output++ = 255; //< leave alpha at maximum

		left += 4;
		right += 4;
	}

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}
