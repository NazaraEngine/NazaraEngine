#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <DataModels/VecValue.hpp>
#include <array>
#include <tuple>

template<typename Data>
VecValue<Data>::VecValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	constexpr std::array<char, 4> componentName = { 'X', 'Y', 'Z', 'W' };
	static_assert(ComponentCount <= componentName.size());

	m_layout = new QFormLayout;
	for (std::size_t i = 0; i < ComponentCount; ++i)
	{
		m_spinboxes[i] = new QDoubleSpinBox;
		m_spinboxes[i]->setDecimals(6);
		m_spinboxes[i]->setValue(1.0);
		m_spinboxes[i]->setStyleSheet("background-color: rgba(255,255,255,255)");
		connect(m_spinboxes[i], qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double)
		{
			UpdatePreview();

			Q_EMIT dataUpdated(0);
		});

		m_layout->addRow(QString::fromUtf8(&componentName[i], 1), m_spinboxes[i]);
	}

	m_pixmap = QPixmap(32, 32);
	m_pixmap.fill();

	m_pixmapLabel = new QLabel;
	m_pixmapLabel->setPixmap(m_pixmap);

	m_layout->addWidget(m_pixmapLabel);

	m_widget = new QWidget;
	m_widget->setStyleSheet("background-color: rgba(0,0,0,0)");
	m_widget->setLayout(m_layout);

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
QWidget* VecValue<Data>::embeddedWidget()
{
	return m_widget;
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
Nz::ShaderAst::ExpressionPtr VecValue<Data>::GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	assert(count == 0);

	return Nz::ShaderBuilder::Constant(ToVector());
}

template<typename Data>
QColor VecValue<Data>::ToColor() const
{
	std::array<float, 4> values = { 0.f, 0.f, 0.f, 1.f };

	for (std::size_t i = 0; i < ComponentCount; ++i)
		values[i] = std::clamp(float(m_spinboxes[i]->value()), 0.f, 1.f);

	return QColor::fromRgbF(values[0], values[1], values[2], values[3]);
}

template<typename Data>
auto VecValue<Data>::ToVector() const -> VecType<ComponentCount>
{
	std::array<float, ComponentCount> values;

	for (std::size_t i = 0; i < ComponentCount; ++i)
		values[i] = std::clamp(float(m_spinboxes[i]->value()), 0.f, 1.f);

	return std::apply([](auto... values)
	{
		return VecType<ComponentCount>(values...);
	}, values);
}

template<typename Data>
void VecValue<Data>::UpdatePreview()
{
	m_pixmap.fill(ToColor());
	m_pixmapLabel->setPixmap(m_pixmap);
}
