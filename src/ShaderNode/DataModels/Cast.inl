#include <DataModels/Cast.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <stdexcept>

template<typename From, typename To>
CastVec<From, To>::CastVec(ShaderGraph& graph) :
ShaderNode(graph)
{
	constexpr std::array<char, 4> componentName = { 'X', 'Y', 'Z', 'W' };
	static_assert(ComponentDiff <= componentName.size());

	QFormLayout* layout = new QFormLayout;

	if constexpr (ComponentDiff > 0)
	{
		for (std::size_t i = 0; i < ComponentDiff; ++i)
		{
			m_spinboxes[i] = new QDoubleSpinBox;
			m_spinboxes[i]->setDecimals(6);
			m_spinboxes[i]->setValue(1.0);
			m_spinboxes[i]->setStyleSheet("background-color: rgba(255,255,255,255)");
			connect(m_spinboxes[i], qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double)
			{
				UpdatePreview();
			});

			layout->addRow(QString::fromUtf8(&componentName[FromComponents + i], 1), m_spinboxes[i]);
		}
	}

	m_pixmap = QPixmap(64, 64);
	m_pixmap.fill();

	m_pixmapLabel = new QLabel;
	m_pixmapLabel->setPixmap(m_pixmap);

	layout->addWidget(m_pixmapLabel);

	m_widget = new QWidget;
	m_widget->setStyleSheet("background-color: rgba(0,0,0,0)");
	m_widget->setLayout(layout);

	m_output = std::make_shared<To>();
}

template<typename From, typename To>
Nz::ShaderAst::ExpressionPtr CastVec<From, To>::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	assert(count == 1);

	if constexpr (ComponentDiff > 0)
	{
		std::array<Nz::ShaderAst::ExpressionPtr, ComponentDiff> constants;
		for (std::size_t i = 0; i < ComponentDiff; ++i)
			constants[i] = Nz::ShaderBuilder::Constant(float(m_spinboxes[i]->value()));

		return std::apply([&](auto&&... values)
		{
			return Nz::ShaderBuilder::Cast<To::ExpressionType>(expressions[0], values...); //< TODO: Forward
		}, constants);
	}
	else
	{
		std::array<Nz::ShaderAst::SwizzleComponent, ToComponents> swizzleComponents;
		for (std::size_t i = 0; i < ToComponents; ++i)
			swizzleComponents[i] = static_cast<Nz::ShaderAst::SwizzleComponent>(static_cast<std::size_t>(Nz::ShaderAst::SwizzleComponent::First) + i);

		return std::apply([&](auto... components)
		{
			std::initializer_list<Nz::ShaderAst::SwizzleComponent> componentList{ components... };
			return Nz::ShaderBuilder::Swizzle(expressions[0], componentList);
		}, swizzleComponents);
	}
}

template<typename From, typename To>
QString CastVec<From, To>::caption() const
{
	static QString caption = From::Type().name + " to " + To::Type().name;
	return caption;
}

template<typename From, typename To>
QString CastVec<From, To>::name() const
{
	static QString name = From::Type().id + "to" + To::Type().id;
	return name;
}

template<typename From, typename To>
QtNodes::NodeDataType CastVec<From, To>::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);

	switch (portType)
	{
		case QtNodes::PortType::In:  return From::Type();
		case QtNodes::PortType::Out: return To::Type();
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

template<typename From, typename To>
QWidget* CastVec<From, To>::embeddedWidget()
{
	return m_widget;
}

template<typename From, typename To>
unsigned int CastVec<From, To>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 1;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<typename From, typename To>
std::shared_ptr<QtNodes::NodeData> CastVec<From, To>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);
	return m_output;
}

template<typename From, typename To>
void CastVec<From, To>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0);
	if (value)
	{
		assert(dynamic_cast<From*>(value.get()) != nullptr);
		m_input = std::static_pointer_cast<From>(value);
	}
	else
		m_input.reset();

	UpdatePreview();
}

template<typename From, typename To>
void CastVec<From, To>::ComputePreview(QPixmap& pixmap) const
{
	assert(m_input);

	const QImage& input = m_input->preview;

	int inputWidth = input.width();
	int inputHeight = input.height();

	QImage& output = m_output->preview;
	output = QImage(inputWidth, inputHeight, QImage::Format_RGBA8888);

	std::array<std::uint8_t, ComponentDiff> constants;
	for (std::size_t i = 0; i < ComponentDiff; ++i)
		constants[i] = static_cast<std::uint8_t>(std::clamp(int(m_spinboxes[i]->value() * 255), 0, 255));

	std::uint8_t* outputPtr = output.bits();
	const std::uint8_t* inputPtr = input.constBits();
	for (int y = 0; y < inputHeight; ++y)
	{
		for (int x = 0; x < inputWidth; ++x)
		{
			constexpr std::size_t CommonComponents = std::min(FromComponents, ToComponents);
			constexpr std::size_t VoidComponents = 4 - ComponentDiff - CommonComponents;

			for (std::size_t i = 0; i < CommonComponents; ++i)
				*outputPtr++ = inputPtr[i];

			for (std::size_t i = 0; i < ComponentDiff; ++i)
				*outputPtr++ = constants[i];

			for (std::size_t i = 0; i < VoidComponents; ++i)
				*outputPtr++ = (i == VoidComponents - 1) ? 255 : 0;

			inputPtr += 4;
		}
	}

	pixmap = QPixmap::fromImage(output).scaled(64, 64);
}

template<typename From, typename To>
void CastVec<From, To>::UpdatePreview()
{
	if (!m_input)
	{
		m_pixmap = QPixmap(64, 64);
		m_pixmap.fill(QColor::fromRgb(255, 255, 255, 0));
	}
	else
		ComputePreview(m_pixmap);

	m_pixmapLabel->setPixmap(m_pixmap);

	Q_EMIT dataUpdated(0);
}
