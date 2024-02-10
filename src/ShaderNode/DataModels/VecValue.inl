#include <ShaderNode/DataModels/VecValue.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <QtCore/QJsonArray>
#include <array>
#include <tuple>

template<std::size_t ComponentCount>
VecValue<ComponentCount>::VecValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	static_assert(ComponentCount <= s_vectorComponents.size());

	for (std::size_t i = 0; i < ComponentCount; ++i)
		m_value[i] = (i == 3) ? 1.f : 0.f;

	UpdatePreview();
}

template<std::size_t ComponentCount>
QString VecValue<ComponentCount>::caption() const
{
	static QString caption = "Vector" + QString::number(ComponentCount) + " constant";
	return caption;
}

template<std::size_t ComponentCount>
QString VecValue<ComponentCount>::name() const
{
	static QString name = "vec" + QString::number(ComponentCount) + "_constant";
	return name;
}

template<std::size_t ComponentCount>
QtNodes::NodeDataType VecValue<ComponentCount>::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	return VecData::Type();
}

template<std::size_t ComponentCount>
unsigned int VecValue<ComponentCount>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::None:
			break;

		case QtNodes::PortType::In:  return 0;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

template<std::size_t ComponentCount>
std::shared_ptr<QtNodes::NodeData> VecValue<ComponentCount>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	auto out = std::make_shared<VecData>(ComponentCount);

	std::array<float, 4> values = { 0.f, 0.f, 0.f, 1.f };

	for (std::size_t i = 0; i < ComponentCount; ++i)
		values[i] = m_value[i];

	out->preview = PreviewValues(1, 1);
	out->preview(0, 0) = nzsl::Vector4f32(values[0], values[1], values[2], values[3]);

	return out;
}

template<std::size_t ComponentCount>
QString VecValue<ComponentCount>::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);
	assert(portType == QtNodes::PortType::Out);

	QString caption = "vec" + QString::number(ComponentCount) + "(";
	for (std::size_t i = 0; i < ComponentCount; ++i)
	{
		if (i > 0)
			caption += ", ";

		caption += QString::number(m_value[i]);
	}

	caption += ")";

	return caption;
}

template<std::size_t ComponentCount>
bool VecValue<ComponentCount>::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);
	return portType == QtNodes::PortType::Out;
}

template<std::size_t ComponentCount>
void VecValue<ComponentCount>::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	for (std::size_t i = 0; i < ComponentCount; ++i)
	{
		QDoubleSpinBox* spinbox = new QDoubleSpinBox;
		spinbox->setDecimals(6);
		spinbox->setRange(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
		spinbox->setValue(m_value[i]);

		connect(spinbox, qOverload<double>(&QDoubleSpinBox::valueChanged), [=](double)
		{
			m_value[i] = float(spinbox->value());
			Q_EMIT dataUpdated(0);

			UpdatePreview();
		});

		layout->addRow(QString::fromUtf8(&s_vectorComponents[i], 1), spinbox);
	}
}

template<std::size_t ComponentCount>
nzsl::Ast::NodePtr VecValue<ComponentCount>::BuildNode(nzsl::Ast::ExpressionPtr* /*expressions*/, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 0);
	assert(outputIndex == 0);

	return nzsl::ShaderBuilder::ConstantValue(m_value);
}

template<std::size_t ComponentCount>
bool VecValue<ComponentCount>::ComputePreview(QPixmap& pixmap)
{
	pixmap.fill(ToColor());
	return true;
}

template<std::size_t ComponentCount>
QColor VecValue<ComponentCount>::ToColor() const
{
	std::array<float, 4> values = { 0.f, 0.f, 0.f, 1.f };

	for (std::size_t i = 0; i < ComponentCount; ++i)
		values[i] = std::clamp(m_value[i], 0.f, 1.f);

	return QColor::fromRgbF(values[0], values[1], values[2], values[3]);
}

template<std::size_t ComponentCount>
void VecValue<ComponentCount>::restore(const QJsonObject& data)
{
	QJsonArray vecValues = data["value"].toArray();
	std::size_t commonValues = std::min(static_cast<std::size_t>(vecValues.size()), ComponentCount);

	for (std::size_t i = 0; i < commonValues; ++i)
		m_value[i] = float(vecValues[int(i)].toDouble(m_value[i]));

	ShaderNode::restore(data);
}

template<std::size_t ComponentCount>
QJsonObject VecValue<ComponentCount>::save() const
{
	QJsonObject data = ShaderNode::save();

	QJsonArray vecValues;
	for (std::size_t i = 0; i < ComponentCount; ++i)
		vecValues.push_back(m_value[i]);

	data["value"] = vecValues;

	return data;
}
