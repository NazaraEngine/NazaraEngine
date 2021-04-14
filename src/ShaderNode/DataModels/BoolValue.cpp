#include <ShaderNode/DataModels/BoolValue.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <QtWidgets/QCheckBox>
#include <cassert>

BoolValue::BoolValue(ShaderGraph& graph) :
ShaderNode(graph),
m_value(true)
{
	UpdatePreview();
}

QString BoolValue::caption() const
{
	static QString caption = "Boolean constant";
	return caption;
}

QString BoolValue::name() const
{
	static QString name = "bool_constant";
	return name;
}

QtNodes::NodeDataType BoolValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	return BoolData::Type();
}

unsigned int BoolValue::nPorts(QtNodes::PortType portType) const
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

std::shared_ptr<QtNodes::NodeData> BoolValue::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	float c = (m_value) ? 1.f : 0.f;

	auto out = std::make_shared<BoolData>();
	out->preview(0, 0) = Nz::Vector4f(c, c, c, 1.f);

	return out;
}

QString BoolValue::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);
	assert(portType == QtNodes::PortType::Out);

	return (m_value) ? "true" : "false";
}

bool BoolValue::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);
	return portType == QtNodes::PortType::Out;
}

void BoolValue::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QCheckBox* checkbox = new QCheckBox;
	checkbox->setCheckState((m_value) ? Qt::Checked : Qt::Unchecked);
	connect(checkbox, &QCheckBox::stateChanged, [=](int newState)
	{
		m_value = (newState == Qt::Checked);
		Q_EMIT dataUpdated(0);

		UpdatePreview();
	});

	layout->addRow(tr("Value"), checkbox);
}

Nz::ShaderAst::NodePtr BoolValue::BuildNode(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 0);
	assert(outputIndex == 0);

	return Nz::ShaderBuilder::Constant(m_value);
}

bool BoolValue::ComputePreview(QPixmap& pixmap)
{
	pixmap.fill(ToColor());
	return true;
}

QColor BoolValue::ToColor() const
{
	float value = (m_value) ? 1.f : 0.f;

	return QColor::fromRgbF(value, value, value, value);
}

void BoolValue::restore(const QJsonObject& data)
{
	m_value = float(data["value"].toBool(m_value));

	ShaderNode::restore(data);
}

QJsonObject BoolValue::save() const
{
	QJsonObject data = ShaderNode::save();
	data["value"] = m_value;

	return data;
}
