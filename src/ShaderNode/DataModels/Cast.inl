#include <ShaderNode/DataModels/Cast.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <stdexcept>

template<std::size_t ToComponentCount>
CastVec<ToComponentCount>::CastVec(ShaderGraph& graph) :
ShaderNode(graph)
{
	static_assert(ToComponentCount <= s_vectorComponents.size());

	for (std::size_t i = 0; i < ToComponentCount; ++i)
		m_overflowComponents[i] = 0.f;

	m_output = std::make_shared<VecData>(ToComponentCount);
}

template<std::size_t ToComponentCount>
void CastVec<ToComponentCount>::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	if (!m_input)
		return;

	std::size_t fromComponentCount = m_input->componentCount;

	if (ToComponentCount > fromComponentCount)
	{
		std::size_t overflowComponentCount = ToComponentCount - fromComponentCount;

		for (std::size_t i = 0; i < overflowComponentCount; ++i)
		{
			QDoubleSpinBox* spinbox = new QDoubleSpinBox;
			spinbox->setDecimals(6);
			spinbox->setRange(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
			spinbox->setValue(m_overflowComponents[i]);

			connect(spinbox, qOverload<double>(&QDoubleSpinBox::valueChanged), [=](double)
			{
				m_overflowComponents[i] = spinbox->value();
				UpdateOutput();
			});

			layout->addRow(QString::fromUtf8(&s_vectorComponents[fromComponentCount + i], 1), spinbox);
		}
	}
}

template<std::size_t ToComponentCount>
Nz::ShaderAst::ExpressionPtr CastVec<ToComponentCount>::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	assert(m_input);
	assert(count == 1);

	std::size_t fromComponentCount = m_input->componentCount;

	if (ToComponentCount > fromComponentCount)
	{
		std::size_t overflowComponentCount = ToComponentCount - fromComponentCount;

		std::array<Nz::ShaderAst::ExpressionPtr, 4> expr;
		expr[0] = expressions[0];
		for (std::size_t i = 0; i < overflowComponentCount; ++i)
			expr[i + 1] = Nz::ShaderBuilder::Constant(m_overflowComponents[i]);

		constexpr auto ExpressionType = VecExpressionType<ToComponentCount>;

		return Nz::ShaderBuilder::Cast<ExpressionType>(expr.data(), 1 + overflowComponentCount);
	}
	else if (ToComponentCount < fromComponentCount)
	{
		std::array<Nz::ShaderAst::SwizzleComponent, ToComponentCount> swizzleComponents;
		for (std::size_t i = 0; i < ToComponentCount; ++i)
			swizzleComponents[i] = static_cast<Nz::ShaderAst::SwizzleComponent>(static_cast<std::size_t>(Nz::ShaderAst::SwizzleComponent::First) + i);

		return std::apply([&](auto... components)
		{
			std::initializer_list<Nz::ShaderAst::SwizzleComponent> componentList{ components... };
			return Nz::ShaderBuilder::Swizzle(expressions[0], componentList);
		}, swizzleComponents);
	}
	else
		return expressions[0]; //< no-op
}

template<std::size_t ToComponentCount>
QString CastVec<ToComponentCount>::caption() const
{
	static QString caption = "To Vector" + QString::number(ToComponentCount);
	return caption;
}

template<std::size_t ToComponentCount>
QString CastVec<ToComponentCount>::name() const
{
	static QString name = "cast_vec" + QString::number(ToComponentCount);
	return name;
}

template<std::size_t ToComponentCount>
QtNodes::NodeDataType CastVec<ToComponentCount>::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);

	switch (portType)
	{
		case QtNodes::PortType::In:  return VecData::Type();
		case QtNodes::PortType::Out: return VecData::Type();
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

template<std::size_t ToComponentCount>
unsigned int CastVec<ToComponentCount>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 1;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<std::size_t ToComponentCount>
std::shared_ptr<QtNodes::NodeData> CastVec<ToComponentCount>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	if (!m_input)
		return nullptr;

	return m_output;
}

template<std::size_t ToComponentCount>
void CastVec<ToComponentCount>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0);
	if (value)
	{
		assert(dynamic_cast<VecData*>(value.get()) != nullptr);
		m_input = std::static_pointer_cast<VecData>(value);
	}
	else
		m_input.reset();

	UpdateOutput();
}

template<std::size_t ToComponentCount>
QtNodes::NodeValidationState CastVec<ToComponentCount>::validationState() const
{
	if (!m_input)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

template<std::size_t ToComponentCount>
QString CastVec<ToComponentCount>::validationMessage() const
{
	if (!m_input)
		return "Missing input";

	return QString();
}

template<std::size_t ToComponentCount>
bool CastVec<ToComponentCount>::ComputePreview(QPixmap& pixmap)
{
	if (!m_input)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview);
	return true;
}

template<std::size_t ToComponentCount>
void CastVec<ToComponentCount>::UpdateOutput()
{
	if (!m_input)
	{
		m_output->preview = QImage(1, 1, QImage::Format_RGBA8888);
		m_output->preview.fill(QColor::fromRgb(0, 0, 0, 0));
		return;
	}

	const QImage& input = m_input->preview;

	int inputWidth = input.width();
	int inputHeight = input.height();

	QImage& output = m_output->preview;
	output = QImage(inputWidth, inputHeight, QImage::Format_RGBA8888);

	std::size_t fromComponentCount = m_input->componentCount;
	std::size_t commonComponents = std::min(fromComponentCount, ToComponentCount);
	std::size_t overflowComponentCount = (ToComponentCount > fromComponentCount) ? ToComponentCount - fromComponentCount : 0;
	std::size_t voidComponents = 4 - overflowComponentCount - commonComponents;

	std::array<std::uint8_t, 4> constants;
	if (ToComponentCount > fromComponentCount)
	{
		for (std::size_t i = 0; i < overflowComponentCount; ++i)
			constants[i] = static_cast<std::uint8_t>(std::clamp(int(m_overflowComponents[i] * 255), 0, 255));
	}

	std::uint8_t* outputPtr = output.bits();
	const std::uint8_t* inputPtr = input.constBits();
	for (int y = 0; y < inputHeight; ++y)
	{
		for (int x = 0; x < inputWidth; ++x)
		{
			for (std::size_t i = 0; i < commonComponents; ++i)
				*outputPtr++ = inputPtr[i];

			for (std::size_t i = 0; i < overflowComponentCount; ++i)
				*outputPtr++ = constants[i];

			for (std::size_t i = 0; i < voidComponents; ++i)
				*outputPtr++ = (i == voidComponents - 1) ? 255 : 0;

			inputPtr += 4;
		}
	}

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}

template<std::size_t ToComponentCount>
void CastVec<ToComponentCount>::restore(const QJsonObject& data)
{
	QJsonArray vecValues = data["value"].toArray();
	std::size_t commonValues = std::min(static_cast<std::size_t>(vecValues.size()), ToComponentCount);

	for (std::size_t i = 0; i < commonValues; ++i)
		m_overflowComponents[i] = float(vecValues[int(i)].toDouble(m_overflowComponents[i]));

	ShaderNode::restore(data);
}

template<std::size_t ToComponentCount>
QJsonObject CastVec<ToComponentCount>::save() const
{
	QJsonObject data = ShaderNode::save();

	QJsonArray vecValues;
	for (std::size_t i = 0; i < ToComponentCount; ++i)
		vecValues.push_back(m_overflowComponents[i]);

	data["value"] = vecValues;

	return data;
}
