#include <ShaderNode/DataModels/Cast.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtCore/QJsonArray>
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
nzsl::Ast::NodePtr CastVec<ToComponentCount>::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(m_input);
	assert(count == 1);
	assert(outputIndex == 0);

	std::size_t fromComponentCount = m_input->componentCount;

	if (ToComponentCount > fromComponentCount)
	{
		std::size_t overflowComponentCount = ToComponentCount - fromComponentCount;

		std::vector<nzsl::Ast::ExpressionPtr> params;
		params.emplace_back(std::move(expressions[0]));
		for (std::size_t i = 0; i < overflowComponentCount; ++i)
			params.emplace_back(nzsl::ShaderBuilder::Constant(m_overflowComponents[i]));

		return nzsl::ShaderBuilder::Cast(nzsl::Ast::ExpressionType{ nzsl::Ast::VectorType{ ToComponentCount, nzsl::Ast::PrimitiveType::Float32 } }, std::move(params));
	}
	else if (ToComponentCount < fromComponentCount)
	{
		std::array<Nz::UInt32, ToComponentCount> swizzleComponents;
		for (std::size_t i = 0; i < ToComponentCount; ++i)
			swizzleComponents[i] = Nz::SafeCast<Nz::UInt32>(i);

		return std::apply([&](auto... components)
		{
			std::initializer_list<Nz::UInt32> componentList{ components... };
			return nzsl::ShaderBuilder::Swizzle(std::move(expressions[0]), componentList);
		}, swizzleComponents);
	}
	else
		return std::move(expressions[0]); //< no-op
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
		default: break;
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
	if (value && value->type().id == VecData::Type().id)
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

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

template<std::size_t ToComponentCount>
void CastVec<ToComponentCount>::UpdateOutput()
{
	if (!m_input)
	{
		m_output->preview = PreviewValues(1, 1);
		m_output->preview(0, 0) = nzsl::Vector4f(0.f, 0.f, 0.f, 0.f);
		return;
	}

	const PreviewValues& input = m_input->preview;

	std::size_t inputWidth = input.GetWidth();
	std::size_t inputHeight = input.GetHeight();

	PreviewValues& output = m_output->preview;
	output = PreviewValues(inputWidth, inputHeight);

	std::size_t fromComponentCount = m_input->componentCount;
	std::size_t commonComponents = std::min(fromComponentCount, ToComponentCount);
	std::size_t overflowComponentCount = (ToComponentCount > fromComponentCount) ? ToComponentCount - fromComponentCount : 0;
	std::size_t voidComponents = 4 - overflowComponentCount - commonComponents;

	for (std::size_t y = 0; y < inputHeight; ++y)
	{
		for (std::size_t x = 0; x < inputWidth; ++x)
		{
			nzsl::Vector4f color = input(x, y);

			float* colorPtr = &color[0];
			for (std::size_t i = 0; i < overflowComponentCount; ++i)
				*colorPtr++ = m_overflowComponents[i];

			for (std::size_t i = 0; i < voidComponents; ++i)
				*colorPtr++ = (i == voidComponents - 1) ? 1.f : 0.f;

			output(x, y) = color;
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
