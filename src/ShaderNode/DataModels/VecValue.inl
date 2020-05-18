#include <DataModels/VecValue.hpp>
#include <array>

template<std::size_t N>
VecValue<N>::VecValue()
{
	constexpr std::array<char, 4> componentName = { 'X', 'Y', 'Z', 'W' };
	static_assert(N <= componentName.size());

	m_layout = new QFormLayout;
	for (std::size_t i = 0; i < N; ++i)
	{
		m_values[i] = new QDoubleSpinBox;
		m_values[i]->setStyleSheet("background-color: rgba(255,255,255,255)");
		m_layout->addRow(QString::fromUtf8(&componentName[i], 1), m_values[i]);
	}

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

Vec4Data::Vec4Data(const Nz::Vector4f& vec) :
value(vec)
{
}
