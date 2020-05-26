#include <ShaderNode/DataModels/VecBinOp.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
VecBinOp<Data, BinOp>::VecBinOp(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_output = std::make_shared<Data>();

	UpdateOutput();
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
Nz::ShaderAst::ExpressionPtr VecBinOp<Data, BinOp>::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	assert(count == 2);
	using BuilderType = typename Nz::ShaderBuilder::template BinOpBuilder<BinOp>;
	constexpr BuilderType builder;
	return builder(expressions[0], expressions[1]);
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
QtNodes::NodeDataType VecBinOp<Data, BinOp>::dataType(QtNodes::PortType /*portType*/, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0 || portIndex == 1);

	return Data::Type();
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
unsigned int VecBinOp<Data, BinOp>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 2;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
std::shared_ptr<QtNodes::NodeData> VecBinOp<Data, BinOp>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
void VecBinOp<Data, BinOp>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0 || index == 1);

	std::shared_ptr<Data> castedValue;
	if (value)
	{
		assert(dynamic_cast<Data*>(value.get()) != nullptr);

		castedValue = std::static_pointer_cast<Data>(value);
	}

	if (index == 0)
		m_lhs = std::move(castedValue);
	else
		m_rhs = std::move(castedValue);

	UpdateOutput();
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
bool VecBinOp<Data, BinOp>::ComputePreview(QPixmap& pixmap)
{
	if (!m_lhs || !m_rhs)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview);
	return true;
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
void VecBinOp<Data, BinOp>::UpdateOutput()
{
	if (!m_lhs || !m_rhs)
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
	ApplyOp(leftResized.constBits(), rightResized.constBits(), m_output->preview.bits(), maxWidth * maxHeight * 4);

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}

template<typename Data>
QString VecAdd<Data>::caption() const
{
	static QString caption = Data::Type().name + " addition";
	return caption;
}

template<typename Data>
QString VecAdd<Data>::name() const
{
	static QString name = Data::Type().name + "add";
	return name;
}

template<typename Data>
void VecAdd<Data>::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		output[i] = static_cast<std::uint8_t>(std::min(lValue + rValue, 255U));
	}
}

template<typename Data>
QString VecMul<Data>::caption() const
{
	static QString caption = Data::Type().name + " multiplication";
	return caption;
}

template<typename Data>
QString VecMul<Data>::name() const
{
	static QString name = Data::Type().name + "mul";
	return name;
}

template<typename Data>
void VecMul<Data>::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		output[i] = static_cast<std::uint8_t>(lValue * rValue / 255);
	}
}

template<typename Data>
QString VecSub<Data>::caption() const
{
	static QString caption = Data::Type().name + " subtraction";
	return caption;
}

template<typename Data>
QString VecSub<Data>::name() const
{
	static QString name = Data::Type().name + "sub";
	return name;
}

template<typename Data>
void VecSub<Data>::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		unsigned int sub = (lValue >= rValue) ? lValue - rValue : 0u;

		output[i] = static_cast<std::uint8_t>(sub);
	}
}
