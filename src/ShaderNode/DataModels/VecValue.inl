#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <ShaderNode/DataModels/VecValue.hpp>
#include <array>
#include <tuple>

template<typename Data>
VecValue<Data>::VecValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	static_assert(ComponentCount <= s_vectorComponents.size());

	std::array<float, ComponentCount> defaultValues;

	for (std::size_t i = 0; i < ComponentCount; ++i)
		defaultValues[i] = (i == 3) ? 1.f : 0.f;

	m_value.Set(defaultValues.data());

	UpdatePreview();
}

template<typename Data>
QString VecValue<Data>::caption() const
{
	static QString caption = Data::Type().name + " constant";
	return caption;
}

template<typename Data>
QString VecValue<Data>::name() const
{
	static QString name = Data::Type().id + "Value";
	return name;
}

template<typename Data>
QtNodes::NodeDataType VecValue<Data>::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	return Data::Type();
}

template<typename Data>
unsigned int VecValue<Data>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 0;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<typename Data>
std::shared_ptr<QtNodes::NodeData> VecValue<Data>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	auto out = std::make_shared<Data>();
	out->preview = QImage(1, 1, QImage::Format_RGBA8888);
	out->preview.fill(ToColor());

	return out;
}

template<typename Data>
void VecValue<Data>::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	for (std::size_t i = 0; i < ComponentCount; ++i)
	{
		QDoubleSpinBox* spinbox = new QDoubleSpinBox;
		spinbox->setDecimals(6);
		spinbox->setValue(m_value[i]);

		connect(spinbox, qOverload<double>(&QDoubleSpinBox::valueChanged), [=](double)
		{
			m_value[i] = spinbox->value();
			Q_EMIT dataUpdated(0);

			UpdatePreview();
		});

		layout->addRow(QString::fromUtf8(&s_vectorComponents[i], 1), spinbox);
	}
}

template<typename Data>
Nz::ShaderAst::ExpressionPtr VecValue<Data>::GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	assert(count == 0);

	return Nz::ShaderBuilder::Constant(m_value);
}

template<typename Data>
bool VecValue<Data>::ComputePreview(QPixmap& pixmap)
{
	pixmap.fill(ToColor());
	return true;
}

template<typename Data>
QColor VecValue<Data>::ToColor() const
{
	std::array<float, 4> values = { 0.f, 0.f, 0.f, 1.f };

	for (std::size_t i = 0; i < ComponentCount; ++i)
		values[i] = std::clamp(m_value[i], 0.f, 1.f);

	return QColor::fromRgbF(values[0], values[1], values[2], values[3]);
}
