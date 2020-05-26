#include <ShaderNode/DataModels/FragmentOutput.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <ShaderNode/DataModels/VecValue.hpp>

Nz::ShaderAst::ExpressionPtr FragmentOutput::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	using namespace Nz::ShaderAst;
	using namespace Nz::ShaderBuilder;

	assert(count == 1);

	auto output = Nz::ShaderBuilder::Output("RenderTarget0", ExpressionType::Float4);

	return Nz::ShaderBuilder::Assign(output, *expressions);
}

QtNodes::NodeDataType FragmentOutput::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::In);
	assert(portIndex == 0);

	return Vec4Data::Type();
}

unsigned int FragmentOutput::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return 0;
	}

	return 0;
}

std::shared_ptr<QtNodes::NodeData> FragmentOutput::outData(QtNodes::PortIndex /*port*/)
{
	return {};
}

void FragmentOutput::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0);
	if (value)
	{
		assert(dynamic_cast<Vec4Data*>(value.get()) != nullptr);
		m_input = std::static_pointer_cast<Vec4Data>(value);
	}
	else
		m_input.reset();

	UpdatePreview();
}

bool FragmentOutput::ComputePreview(QPixmap& pixmap)
{
	if (!m_input)
		return false;

	pixmap = QPixmap::fromImage(m_input->preview);
	return true;
}
