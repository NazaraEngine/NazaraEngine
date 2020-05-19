#include <DataModels/VecBinOp.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
VecBinOp<Data, BinOp>::VecBinOp(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_preview = QPixmap(64, 64);

	m_pixmapLabel = new QLabel;
	m_pixmapLabel->setPixmap(m_preview);
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
QWidget* VecBinOp<Data, BinOp>::embeddedWidget()
{
	return m_pixmapLabel;
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
		case QtNodes::PortType::In: return 2;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
std::shared_ptr<QtNodes::NodeData> VecBinOp<Data, BinOp>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return std::make_shared<Data>(GetValue());
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
void VecBinOp<Data, BinOp>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	std::shared_ptr<Data> castedValue;
	if (value)
	{
		assert(dynamic_cast<Data*>(value.get()) != nullptr);
		assert(index == 0 || index == 1);

		castedValue = std::static_pointer_cast<Data>(value);
	}

	if (index == 0)
		m_lhs = std::move(castedValue);
	else
		m_rhs = std::move(castedValue);

	UpdatePreview();

	Q_EMIT dataUpdated(0);
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
void VecBinOp<Data, BinOp>::UpdatePreview()
{
	InternalType value = GetValue();
	m_preview.fill(QColor::fromRgbF(value.x, value.y, value.z, value.w));
	m_pixmapLabel->setPixmap(m_preview);
}

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
auto VecBinOp<Data, BinOp>::GetValue() const -> InternalType
{
	if (!m_lhs || !m_rhs)
		return InternalType::Zero();

	return m_lhs->value * m_rhs->value;
}
