#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <DataModels/VecValue.hpp>
#include <array>

template<std::size_t N, typename Data>
VecValue<N, Data>::VecValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	constexpr std::array<char, 4> componentName = { 'X', 'Y', 'Z', 'W' };
	static_assert(N <= componentName.size());

	m_layout = new QFormLayout;
	for (std::size_t i = 0; i < N; ++i)
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

template<std::size_t N, typename Data>
QtNodes::NodeDataType VecValue<N, Data>::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	return Data::Type();
}

template<std::size_t N, typename Data>
QWidget* VecValue<N, Data>::embeddedWidget()
{
	return m_widget;
}

template<std::size_t N, typename Data>
unsigned int VecValue<N, Data>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 0;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<std::size_t N, typename Data>
std::shared_ptr<QtNodes::NodeData> VecValue<N, Data>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	auto out = std::make_shared<Data>();
	out->preview = QImage(1, 1, QImage::Format_RGBA8888);
	out->preview.fill(ToColor());

	return out;
}

template<std::size_t N, typename Data>
Nz::ShaderAst::ExpressionPtr VecValue<N, Data>::GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	assert(count == 0);

	return Nz::ShaderBuilder::Constant(ToVector());
}

template<std::size_t N, typename Data>
QColor VecValue<N, Data>::ToColor() const
{
	std::array<float, 4> values = { 0.f, 0.f, 0.f, 1.f };

	for (std::size_t i = 0; i < N; ++i)
		values[i] = std::clamp<float>(m_spinboxes[i]->value(), 0.0, 1.0);

	return QColor::fromRgbF(values[0], values[1], values[2], values[3]);
}

template<std::size_t N, typename Data>
VecType<N> VecValue<N, Data>::ToVector() const
{
	std::array<float, 4> values = { 0.f, 0.f, 0.f, 1.f };

	for (std::size_t i = 0; i < N; ++i)
		values[i] = std::clamp<float>(m_spinboxes[i]->value(), 0.0, 1.0);

	if constexpr (N == 2)
		return Nz::Vector2f(values[0], values[1]);
	else if constexpr (N == 3)
		return Nz::Vector3f(values[0], values[1], values[2]);
	else if constexpr (N == 4)
		return Nz::Vector4f(values[0], values[1], values[2], values[3]);
	else
		static_assert(Nz::AlwaysFalse<std::make_integer_sequence<int, N>>(), "Unhandled vector size");
}

template<std::size_t N, typename Data>
void VecValue<N, Data>::UpdatePreview()
{
	m_pixmap.fill(ToColor());
	m_pixmapLabel->setPixmap(m_pixmap);
}

inline VecData::VecData() :
preview(64, 64, QImage::Format_RGBA8888)
{
	preview.fill(QColor::fromRgb(255, 255, 255, 0));
}
