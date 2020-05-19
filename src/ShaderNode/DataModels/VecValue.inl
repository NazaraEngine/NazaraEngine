#include <DataModels/VecValue.hpp>
#include <array>

template<std::size_t N>
VecValue<N>::VecValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	constexpr std::array<char, 4> componentName = { 'X', 'Y', 'Z', 'W' };
	static_assert(N <= componentName.size());

	m_layout = new QFormLayout;
	for (std::size_t i = 0; i < N; ++i)
	{
		m_values[i] = new QDoubleSpinBox;
		m_values[i]->setDecimals(6);
		m_values[i]->setValue(1.0);
		m_values[i]->setStyleSheet("background-color: rgba(255,255,255,255)");
		connect(m_values[i], qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double)
		{
			UpdatePreview();

			Q_EMIT dataUpdated(0);
		});

		m_layout->addRow(QString::fromUtf8(&componentName[i], 1), m_values[i]);
	}

	m_pixmap = QPixmap(64, 64);
	m_pixmap.fill();

	m_pixmapLabel = new QLabel;
	m_pixmapLabel->setPixmap(m_pixmap);

	m_layout->addWidget(m_pixmapLabel);

	m_widget = new QWidget;
	m_widget->setStyleSheet("background-color: rgba(0,0,0,0)");
	m_widget->setLayout(m_layout);
}

template<std::size_t N>
QWidget* VecValue<N>::embeddedWidget()
{
	return m_widget;
}

template<std::size_t N>
unsigned int VecValue<N>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 0;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<std::size_t N>
void VecValue<N>::UpdatePreview()
{
	ComputePreview(m_pixmap);
	m_pixmapLabel->setPixmap(m_pixmap);
}

Vec2Data::Vec2Data(const InternalType& vec) :
value(vec)
{
}

Vec4Data::Vec4Data(const InternalType& vec) :
value(vec)
{
}
