#include <ShaderNode/DataModels/VecComposition.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <stdexcept>

template<std::size_t ComponentCount>
VecComposition<ComponentCount>::VecComposition(ShaderGraph& graph) :
ShaderNode(graph)
{
	static_assert(ComponentCount <= s_vectorComponents.size());

	m_output = std::make_shared<VecData>(ComponentCount);
}

template<std::size_t ComponentCount>
nzsl::Ast::NodePtr VecComposition<ComponentCount>::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == ComponentCount);
	assert(outputIndex == 0);

	std::vector<nzsl::Ast::ExpressionPtr> params;
	for (std::size_t i = 0; i < count; ++i)
		params.emplace_back(std::move(expressions[i]));

	return nzsl::ShaderBuilder::Cast(nzsl::Ast::ExpressionType{ nzsl::Ast::VectorType{ ComponentCount, nzsl::Ast::PrimitiveType::Float32 } }, std::move(params));
}

template<std::size_t ComponentCount>
QString VecComposition<ComponentCount>::caption() const
{
	static QString caption = "Compose Vector" + QString::number(ComponentCount);
	return caption;
}

template<std::size_t ComponentCount>
QString VecComposition<ComponentCount>::name() const
{
	static QString name = "vec_compose" + QString::number(ComponentCount);
	return name;
}

template<std::size_t ComponentCount>
QtNodes::NodeDataType VecComposition<ComponentCount>::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			assert(portIndex >= 0);
			assert(portIndex < ComponentCount);
			return FloatData::Type();
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex == 0);
			return VecData::Type();
		}

		default: break;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

template<std::size_t ComponentCount>
unsigned int VecComposition<ComponentCount>::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return ComponentCount;
		case QtNodes::PortType::Out: return 1;

		default: break;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

template<std::size_t ComponentCount>
std::shared_ptr<QtNodes::NodeData> VecComposition<ComponentCount>::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	if (validationState() != QtNodes::NodeValidationState::Valid)
		return nullptr;

	return m_output;
}

template<std::size_t ComponentCount>
void VecComposition<ComponentCount>::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index >= 0 && index < ComponentCount);

	if (value && value->type().id == FloatData::Type().id)
	{
		assert(dynamic_cast<FloatData*>(value.get()) != nullptr);
		m_inputs[index] = std::static_pointer_cast<FloatData>(value);
	}
	else
		m_inputs[index].reset();

	UpdateOutput();
}

template<std::size_t ComponentCount>
QtNodes::NodeValidationState VecComposition<ComponentCount>::validationState() const
{
	for (std::size_t i = 0; i < ComponentCount; ++i)
	{
		if (!m_inputs[i])
			return QtNodes::NodeValidationState::Error;
	}

	return QtNodes::NodeValidationState::Valid;
}

template<std::size_t ComponentCount>
QString VecComposition<ComponentCount>::validationMessage() const
{
	for (std::size_t i = 0; i < ComponentCount; ++i)
	{
		if (!m_inputs[i])
			return "Missing input #" + QString::number(i + 1);
	}

	return QString();
}

template<std::size_t ComponentCount>
bool VecComposition<ComponentCount>::ComputePreview(QPixmap& pixmap)
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
		return false;

	pixmap = QPixmap::fromImage(m_output->preview.GenerateImage());
	return true;
}

template<std::size_t ComponentCount>
void VecComposition<ComponentCount>::UpdateOutput()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		m_output->preview = PreviewValues(1, 1);
		m_output->preview(0, 0) = nzsl::Vector4f32(0.f, 0.f, 0.f, 0.f);
		return;
	}

	std::array<PreviewValues, ComponentCount> previewResized;
	std::size_t maxInputWidth = 0;
	std::size_t maxInputHeight = 0;

	for (std::size_t i = 0; i < ComponentCount; ++i)
	{
		// FIXME: Prevent useless copy
		previewResized[i] = m_inputs[i]->preview;

		maxInputWidth = std::max(maxInputWidth, previewResized[i].GetWidth());
		maxInputHeight = std::max(maxInputHeight, previewResized[i].GetHeight());
	}

	PreviewValues& output = m_output->preview;
	output = PreviewValues(maxInputWidth, maxInputHeight);

	for (std::size_t i = 0; i < ComponentCount; ++i)
	{
		if (previewResized[i].GetWidth() != maxInputWidth || previewResized[i].GetHeight() != maxInputHeight)
			previewResized[i] = previewResized[i].Resized(maxInputWidth, maxInputHeight);
	}

	for (std::size_t y = 0; y < maxInputHeight; ++y)
	{
		for (std::size_t x = 0; x < maxInputWidth; ++x)
		{
			nzsl::Vector4f32 color(0.f, 0.f, 0.f, 1.f);
			for (std::size_t i = 0; i < ComponentCount; ++i)
				color[i] = previewResized[i](x, y)[0];

			output(x, y) = color;
		}
	}

	Q_EMIT dataUpdated(0);

	UpdatePreview();
}
