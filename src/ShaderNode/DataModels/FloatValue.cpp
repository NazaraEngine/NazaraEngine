#include <ShaderNode/DataModels/FloatValue.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <cassert>

FloatValue::FloatValue(ShaderGraph& graph) :
ShaderNode(graph),
m_value(1.f)
{
	UpdatePreview();
}

QString FloatValue::caption() const
{
	static QString caption = "Float constant";
	return caption;
}

QString FloatValue::name() const
{
	static QString name = "float_constant";
	return name;
}

QtNodes::NodeDataType FloatValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	return FloatData::Type();
}

unsigned int FloatValue::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 0;
		case QtNodes::PortType::Out: return 1;

		default:
			break;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

std::shared_ptr<QtNodes::NodeData> FloatValue::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	auto out = std::make_shared<FloatData>();
	out->preview(0, 0) = nzsl::Vector4f32(m_value, m_value, m_value, 1.f);

	return out;
}

QString FloatValue::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);
	assert(portType == QtNodes::PortType::Out);

	return QString::number(m_value);
}

bool FloatValue::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);
	return portType == QtNodes::PortType::Out;
}

void FloatValue::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QDoubleSpinBox* spinbox = new QDoubleSpinBox;
	spinbox->setDecimals(6);
	spinbox->setRange(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
	spinbox->setValue(m_value);

	connect(spinbox, qOverload<double>(&QDoubleSpinBox::valueChanged), [=](double)
	{
		m_value = spinbox->value();
		Q_EMIT dataUpdated(0);

		UpdatePreview();
	});

	layout->addRow(tr("Value"), spinbox);
}

nzsl::Ast::NodePtr FloatValue::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 0);
	assert(outputIndex == 0);

	return nzsl::ShaderBuilder::ConstantValue(m_value);
}

bool FloatValue::ComputePreview(QPixmap& pixmap)
{
	pixmap.fill(ToColor());
	return true;
}

QColor FloatValue::ToColor() const
{
	float value = std::clamp(m_value, 0.f, 1.f);

	return QColor::fromRgbF(value, value, value, value);
}

void FloatValue::restore(const QJsonObject& data)
{
	m_value = float(data["value"].toDouble(m_value));

	ShaderNode::restore(data);
}

QJsonObject FloatValue::save() const
{
	QJsonObject data = ShaderNode::save();
	data["value"] = m_value;

	return data;
}
